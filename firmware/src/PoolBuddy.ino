#include "DS18B20.h"
#include "SparkFunMAX17043.h"

PRODUCT_ID(4513);
PRODUCT_VERSION(2);

#define POWER D6
#define TEMP_SENSOR D5
#define CALIBRATION_LED D7
#define PH_ADDRESS 99
#define ORP_ADDRESS 98
#define SAMPLE_INTERVAL 2500
#define WAKE_INTERVAL 60000;

// Set external antenna (remembered after power off)
STARTUP(WiFi.selectAntenna(ANT_EXTERNAL));

// Reset application if unresponsive for 3 minutes
ApplicationWatchdog wd(180000, System.reset);

DS18B20 ds18b20(TEMP_SENSOR);
unsigned int nextSampleTime, nextSleepTime, sleepInterval;

double temp = 0;
double ph = 0;
double orp = 0;
double soc = 0;
int wifi;
bool calibrationmode = false;
bool temponly = false;

void setup() {
  if (!Wire.isEnabled())
    Wire.begin();

  pinMode(POWER,OUTPUT);
  pinMode(TEMP_SENSOR, INPUT);
  pinMode(CALIBRATION_LED, OUTPUT);
  digitalWrite(POWER, HIGH);
  digitalWrite(CALIBRATION_LED, LOW);

  lipo.begin();
  lipo.quickStart();

  Particle.variable("temp", temp);
  Particle.variable("pH", ph);
  Particle.variable("ORP", orp);
  Particle.variable("soc", soc);
  Particle.variable("calibrating", calibrationmode);
  Particle.variable("temponly", temponly);
  Particle.function("switchcalmode", switchcalibrationmode);
  Particle.function("switchtemponly", switchtemponly);
  Particle.function("calibratePH", calibrate_ph);
  Particle.function("calibrateORP", calibrate_orp);
  Particle.function("check", check_calibrated);
  Particle.function("slope", check_slope);
  Particle.variable("wifi", wifi);

  nextSleepTime = millis() + WAKE_INTERVAL;
  //Serial.begin(9600);
}

void loop() {
  // Always measure every sample time interval
  if (millis() > nextSampleTime) {
      measure_temp();
      if(temponly == 0) {
        compensate_temp_ph();
        measure_ph();
        measure_orp();
      }
      measure_wifi();
      nextSampleTime = millis() + SAMPLE_INTERVAL;
      soc = lipo.getSOC();
  }

  // Publish results before going to sleep
  if(millis() > nextSleepTime) {
    Particle.publish("pooldata", water_data(), PRIVATE);
    // Only go to sleep when not in calibration mode
    if (!calibrationmode) {
      if(soc <= 10)
        sleepInterval = 60 * 60;
      else if(soc <= 20)
        sleepInterval = 60 * 30;
      else
        sleepInterval = 60 * 15;
      nextSleepTime = millis() + (sleepInterval * 1000) + WAKE_INTERVAL;
      executeRequest(PH_ADDRESS,"Sleep"); 
      executeRequest(ORP_ADDRESS,"Sleep");
      digitalWrite(POWER, LOW);
      System.sleep(SLEEP_MODE_DEEP, sleepInterval);
    } else {
      nextSleepTime = millis() + SAMPLE_INTERVAL;
      orp = ph = 0;
    }
  }
}

String water_data() {
  if(temponly)
    return String::format("{\"temperature\":%f,\"soc\":%f,\"wifi\":%d}",temp,soc,wifi);
  else 
    return String::format("{\"temperature\":%f,\"ph\":%f,\"orp\":%f,\"soc\":%f,\"wifi\":%d}",temp,ph,orp,soc,wifi);
}

void measure_ph() {
  String pH_data = executeRequest(PH_ADDRESS,"R");
  //Serial.print("Ph:");
  //Particle.publish("ph_reading", pH_data, PRIVATE);
  //Serial.println(pH_data);
  if (isdigit(pH_data.charAt(0))) {
    float current = String(pH_data).toFloat();
    if((ph == 0) | calibrationmode)
      ph = current;
    else
      ph = (ph + current) / 2;
  }
}

void measure_orp() {
  String orp_data = executeRequest(ORP_ADDRESS,"R");
  //Serial.print("ORP:");
  //Serial.println(orp_data);
  if (isdigit(orp_data.charAt(0))) {
    float current = String(orp_data).toFloat();
    if((orp == 0) | calibrationmode)
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
  if (dsAttempts < 4) {
    if (!isnan(celsius))
      temp = celsius;
  }
}

void measure_wifi() {
  int wifi_db = WiFi.RSSI();
  if(wifi_db < 0)
    wifi = map(wifi_db, -1, -127, 100, 0);
}

int switchcalibrationmode(String arg) {
  calibrationmode = !calibrationmode;
  if(calibrationmode) {
    digitalWrite(CALIBRATION_LED, HIGH);
    nextSleepTime = millis() + SAMPLE_INTERVAL;
  } else {
    digitalWrite(CALIBRATION_LED, LOW);
    nextSleepTime = millis() + WAKE_INTERVAL;
  }
  return 0;
}

int switchtemponly(String arg) {
  temponly = !temponly;
  return 0;
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
  if(arg=="clear")
    commandString.concat("clear");
  Particle.publish("calibrate", executeRequest(PH_ADDRESS,commandString), PRIVATE);
  return 0;
}

int calibrate_orp(String arg) {
  String commandString = String("Cal,");
  commandString.concat(arg);
  Particle.publish("calibrate", executeRequest(ORP_ADDRESS,commandString), PRIVATE);
  return 0;
}

void compensate_temp_ph() {
  String commandString = String("T,");
  commandString.concat(String(temp, 2));
  executeRequest(PH_ADDRESS,commandString);
}

int check_calibrated(String args) {
  Particle.publish("check",String::format("Ph:%s %s", executeRequest(PH_ADDRESS,"Cal,?").c_str(), executeRequest(PH_ADDRESS,"Status").c_str()), PRIVATE);
  Particle.publish("check",String::format("ORP:%s %s", executeRequest(ORP_ADDRESS,"Cal,?").c_str(), executeRequest(ORP_ADDRESS,"Status").c_str()), PRIVATE);
  return 0;
}

int check_slope(String args) {
  Particle.publish("check", executeRequest(PH_ADDRESS,"Slope,?"), PRIVATE);
  return 0;
}

String executeRequest(int address, String cmd) {
  char data[20];
  int i=0;

  Wire.beginTransmission(address);
  Wire.write(cmd);
  Wire.endTransmission();
  if((cmd == "R") | cmd.startsWith("Cal"))
    delay(900);
  else
    delay(300);
  Wire.requestFrom(address, 20);
  int val=Wire.read();
  switch (val) {
    case 1:
      //Serial.println("Success");
      break;
    case 2:
      //Serial.println("Failed");
      return String("Failed");
      break;
    case 254:
      //Serial.println("Pending");
      break;
    case 255:
      //Serial.println("No Data");
      break;
    default:
      //Serial.printlnf("Invalid response %d",val);
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
