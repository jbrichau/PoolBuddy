#include "DS18B20.h"
#include "SparkFunMAX17043.h"

#define POWER D6
#define TEMP_SENSOR D5
#define PH_ADDRESS 99
#define ORP_ADDRESS 98
#define SAMPLE_INTERVAL 2500
#define WAKE_INTERVAL 60000;

// Set external antenna (remembered after power off)
STARTUP(WiFi.selectAntenna(ANT_EXTERNAL));
//STARTUP(WiFi.selectAntenna(ANT_AUTO));

// Reset application if unresponsive for 3 minutes
ApplicationWatchdog wd(180000, System.reset);

DS18B20 ds18b20(TEMP_SENSOR);
int nextSampleTime, nextSleepTime, sleepInterval;

double temp = 0;
double ph = 0;
double orp = 0;
double soc = 0;
int wifi;
bool calibrationmode = false;

void setup() {
  //Serial.begin(9600);
  temp = 0;
  ph = 0;
  orp = 0;
  soc = 0;
  if (!Wire.isEnabled())
    Wire.begin();
  pinMode(POWER,OUTPUT);
  pinMode(TEMP_SENSOR, INPUT);
  digitalWrite(POWER, HIGH);

  lipo.begin();
  lipo.quickStart();

  Particle.variable("temp", temp);
  Particle.variable("pH", ph);
  Particle.variable("ORP", orp);
  Particle.variable("soc", soc);
  Particle.variable("calibrating",calibrationmode);
  Particle.function("switchmode",switchcalibrationmode);
  Particle.function("calibratePh",calibrate_ph);
  Particle.function("calibrateORP",calibrate_orp);
  Particle.function("check",check_calibrated);
  Particle.function("measure",measure);
  Particle.function("slope",check_slope);
  Particle.variable("wifi",wifi);

  nextSleepTime = millis() + WAKE_INTERVAL;
}

void loop() {
  if (!calibrationmode) {
    if (millis() > nextSampleTime) {
      measure_temp();
      compensate_temp_ph();
      measure_ph();
      measure_orp();
      measure_wifi();
      nextSampleTime = millis() + SAMPLE_INTERVAL;
      soc = lipo.getSOC();
    }
    if(millis() > nextSleepTime) {
      Particle.publish("waterdata", water_data(), PRIVATE);
      if(soc <= 10)
        sleepInterval = 60 * 15;
      else if(soc <= 20)
        sleepInterval = 60 * 10;
      else
        sleepInterval = 60 * 5;
      nextSleepTime = millis() + (sleepInterval * 1000) + WAKE_INTERVAL;
      executeRequest(PH_ADDRESS,"Sleep");
      executeRequest(ORP_ADDRESS,"Sleep");
      digitalWrite(POWER, LOW);
      System.sleep(SLEEP_MODE_DEEP,sleepInterval);
    }
  }
}

String water_data() {
  return String::format("{\"temperature\":%f,\"ph\":%f,\"orp\":%f,\"soc\":%f,\"wifi\":%d}",temp,ph,orp,soc,wifi);
}

int measure(String arg) {
  ph = 0;
  orp = 0;
  temp = 0;
  measure_temp();
  measure_ph();
  measure_orp();
  measure_wifi();
}

void measure_ph() {
  String pH_data = executeRequest(PH_ADDRESS,"R");
  //Serial.print("Ph:");
  //Particle.publish("ph_reading", pH_data, PRIVATE);
  //Serial.println(pH_data);
  if (isdigit(pH_data[0])) {
    float current = String(pH_data).toFloat();
    if(ph == 0)
      ph = current;
    else
      ph = (ph + current) / 2;
  }
}

void measure_orp() {
  String orp_data = executeRequest(ORP_ADDRESS,"R");
  //Serial.print("ORP:");
  //Serial.println(orp_data);
  if (isdigit(orp_data[0])) {
    float current = String(orp_data).toFloat();
    if(orp == 0)
      orp = current;
    else
      orp = (orp + current) / 2;
  }
}

void measure_temp() {
  int dsAttempts = 0;
  double celsius = 0;

  if(!ds18b20.search())
    ds18b20.resetsearch();

  celsius = ds18b20.getTemperature();
  //Serial.printlnf("%f celsius",celsius);
  while (!ds18b20.crcCheck() && dsAttempts < 4){
    //Serial.println("Caught bad value.");
    dsAttempts++;
    //Serial.print("Attempts to Read: ");
    //Serial.println(dsAttempts);
    delay(1000);
    ds18b20.resetsearch();
    delay(250);
    celsius = ds18b20.getTemperature();
  }
  if(dsAttempts < 4) {
    if(temp == 0)
      temp = celsius;
    else
      temp = (temp + celsius) / 2;
  }
}

void measure_wifi() {
  float wifi_db = WiFi.RSSI();
  if(wifi_db < 0)
    wifi = (int)((-wifi_db / 127)*100);
}

int switchcalibrationmode(String arg) {
  calibrationmode = !calibrationmode;
}

int calibrate_ph(String arg) {
  String commandString = String("Cal");
  commandString.concat(",");
  commandString.concat(arg);
  commandString.concat(",");
  if(arg=="mid")
    commandString.concat("7.00");
  if(arg=="low")
    commandString.concat("4.00");
  if(arg=="high")
    commandString.concat("10.00");
  Serial.println(executeRequest(PH_ADDRESS,commandString));
  return 0;
}

int calibrate_orp(String arg) {
  String commandString = String("Cal,");
  commandString.concat(arg);
  executeRequest(ORP_ADDRESS,commandString);
  return 0;
}

void compensate_temp_ph() {
  String commandString = String("T,");
  commandString.concat(String(temp, 2));
  executeRequest(PH_ADDRESS,commandString);
}

int check_calibrated(String args) {
  Serial.println("Ph:");
  Serial.println(executeRequest(PH_ADDRESS,"Cal,?"));
  Serial.println(executeRequest(PH_ADDRESS,"Status"));
  Serial.println("ORP:");
  Serial.println(executeRequest(ORP_ADDRESS,"Cal,?"));
  Serial.println(executeRequest(ORP_ADDRESS,"Status"));
  return 0;
}

int check_slope(String args) {
  executeRequest(PH_ADDRESS,"Slope,?");
  return 0;
}

String executeRequest(int address, String cmd) {
  char data[20];
  int i=0;

  Wire.beginTransmission(address);
  Wire.write(cmd);
  Wire.endTransmission();
  if(cmd=="R" | cmd.startsWith("Cal"))
    delay(900);
  else
    delay(300);
  Wire.requestFrom(address, 20);
  int val=Wire.read();
  switch (val) {
    case 1:
      Serial.println("Success");
      break;
    case 2:
      Serial.println("Failed");
      return String("Failed");
      break;
    case 254:
      Serial.println("Pending");
      break;
    case 255:
      Serial.println("No Data");
      break;
    default:
      Serial.printlnf("Invalid response %d",val);
      return String("Failed");
  }

  while(Wire.available()) {
    data[i]= Wire.read();
    i+=1;
  }
  Wire.endTransmission();
  //Serial.println(String(data));
  return String(data);
}
