#include <DS18B20.h>

#define TEMP_SENSOR D6

DS18B20 ds18b20(TEMP_SENSOR);                         //Sets Pin D6 for Water Temp Sensor
double celsius;
unsigned int Metric_Publish_Rate = 30000;
unsigned int MetricnextPublishTime;
int DS18B20nextSampleTime;
int DS18B20_SAMPLE_INTERVAL = 2500;
int dsAttempts = 0;

String inputstring = "";                              //a string to hold incoming data from the PC
String sensorstring = "";                             //a string to hold the data from the Atlas Scientific product
boolean input_string_complete = false;                //have we received all the data from the PC
boolean sensor_string_complete = false;               //have we received all the data from the Atlas Scientific product
float pH;                                            //used to hold a floating point number that is the pH
float ORP;

void setup() {                                        //set up the hardware
  Serial.begin(9600);                                //set baud rate for the hardware serial port_0 to 9600
  Serial1.begin(9600,SERIAL_8N1);                     //set baud rate for the software serial port to 9600
  pinMode(TEMP_SENSOR, INPUT);
  Particle.variable("temp", celsius);
  Particle.function("calibratePh",calibrate_ph);
  Particle.function("calibrateORP",calibrate_orp);
  Particle.function("check",check_calibrated);
  Particle.function("slope",check_slope);
}

void serialEvent1() {
  if (Serial1.available() > 0) {                     //if we see that the Atlas Scientific product has sent a character
    char inchar = (char)Serial1.read();              //get the char we just received
    sensorstring += inchar;                           //add the char to the var called sensorstring
    if (inchar == '\r') {                             //if the incoming character is a <CR>
      sensor_string_complete = true;                  //set the flag
    }
  }
}

void loop() {                                         //here we go...
  //getPh();
  getORP();
  if (millis() > DS18B20nextSampleTime){
    getTemp();
    }
}

void getPh() {
  if (sensor_string_complete == true) {               //if a string from the Atlas Scientific product has been received in its entirety
    Serial.print("Ph:");
    Serial.println(sensorstring);                     //send that string to the PC's serial monitor
    if (isdigit(sensorstring[0])) {                   //if the first character in the string is a digit
      pH = sensorstring.toFloat();                    //convert the string to a floating point number so it can be evaluated by the Arduino
      if (pH >= 7.0) {                                //if the pH is greater than or equal to 7.0
        Serial.println("high");                       //print "high" this is demonstrating that the Arduino is evaluating the pH as a number and not as a string
      }
      if (pH <= 6.999) {                              //if the pH is less than or equal to 6.999
        Serial.println("low");                        //print "low" this is demonstrating that the Arduino is evaluating the pH as a number and not as a string
      }
    }
    sensorstring = "";                                //clear the string
    sensor_string_complete = false;                   //reset the flag used to tell if we have received a completed string from the Atlas Scientific product
  }
}

void getORP() {
  if (sensor_string_complete == true) {               //if a string from the Atlas Scientific product has been received in its entirety
    Serial.print("ORP:");
    Serial.println(sensorstring);                     //send that string to the PC's serial monitor
    if (isdigit(sensorstring[0])) {
     ORP = sensorstring.toFloat();
     if (ORP >= 500.0) {
     Serial.println("high");
     }
     if (ORP <= 499.9) {
     Serial.println("low");
     }
    }
    sensorstring = "";                                //clear the string
    sensor_string_complete = false;                   //reset the flag used to tell if we have received a completed string from the Atlas Scientific product
  }
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
  commandString.concat("\r");
  Serial.println(commandString);
  Serial1.print(commandString);
  return 0;
}

int calibrate_orp(String arg) {
  String commandString = String("Cal");
  commandString.concat(",");
  commandString.concat(arg);
  commandString.concat("\r");
  Serial.println(commandString);
  Serial1.print(commandString);
  return 0;
}

int check_calibrated(String args) {
  Serial1.print("Cal,?\r");
  return 0;
}

int check_slope(String args) {
  Serial1.print("Slope,?\r");
  return 0;
}

void getTemp(){
    if(!ds18b20.search()){
      ds18b20.resetsearch();
      celsius = ds18b20.getTemperature();
      Serial.printlnf("%f celsius",celsius);
      while (!ds18b20.crcCheck() && dsAttempts < 4){
        Serial.println("Caught bad value.");
        dsAttempts++;
        Serial.print("Attempts to Read: ");
        Serial.println(dsAttempts);
        if (dsAttempts == 3){
          delay(1000);
        }
        ds18b20.resetsearch();
        celsius = ds18b20.getTemperature();
        continue;
      }
      dsAttempts = 0;
      DS18B20nextSampleTime = millis() + DS18B20_SAMPLE_INTERVAL;
    }
  }
