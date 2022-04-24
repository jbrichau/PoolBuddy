/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/Users/johanbrichau/ElectronicProjects/Particle/PoolBuddy2/src/PoolBuddy2.ino"
/*
 * Project PoolBuddy2
 * Description:
 * Author:
 * Date:
 */
#include "DS18B20.h"

void setup();
void loop();
uint32_t ieee11073_from_float(float temperature);
void measure_ph();
void measure_orp();
void measure_temp();
String executeRequest(int address, String cmd);
#line 9 "/Users/johanbrichau/ElectronicProjects/Particle/PoolBuddy2/src/PoolBuddy2.ino"
#define POWER_ATLAS D4
#define POWER_TEMP A2
#define TEMP_SENSOR A5
#define CALIBRATION_LED D7
#define PH_ADDRESS 99
#define ORP_ADDRESS 98
#define SAMPLE_INTERVAL 2500


BleUuid envSensingService(BLE_SIG_UUID_ENVIRONMENT_SENSING_SVC);
const char* tempUuid = "00000004-6383-426f-8a42-1470ba8848a1";
const char* phUuid = "00000005-6383-426f-8a42-1470ba8848a1";
const char* orpUuid = "00000006-6383-426f-8a42-1470ba8848a1";
BleCharacteristic tempCharacteristic("temp", BleCharacteristicProperty::NOTIFY, tempUuid, envSensingService);
BleCharacteristic phCharacteristic("ph", BleCharacteristicProperty::NOTIFY, phUuid, envSensingService);
BleCharacteristic orpCharacteristic("orp", BleCharacteristicProperty::NOTIFY, orpUuid, envSensingService);

DS18B20 ds18b20(TEMP_SENSOR);
double temp = 0;
double ph = 0;
double orp = 0;
double soc = 0;
bool calibrationmode = false;
bool temponly = false;

const unsigned long UPDATE_INTERVAL_MS = 2000;
unsigned long lastUpdate = 0;

SYSTEM_MODE(MANUAL);
//ApplicationWatchdog wd(180000, System.reset);

void setup() {
  pinMode(POWER_ATLAS, OUTPUT);
  pinMode(POWER_TEMP, OUTPUT);

  digitalWrite(POWER_ATLAS, LOW);
  digitalWrite(POWER_TEMP, LOW);

  if (!Wire.isEnabled())
    Wire.begin();


  BLE.on();
  BLE.addCharacteristic(tempCharacteristic);
  BLE.addCharacteristic(phCharacteristic);
  BLE.addCharacteristic(orpCharacteristic);

  BleAdvertisingData advData;
  advData.appendServiceUUID(envSensingService);
  advData.appendLocalName("PoolBuddy2");
  BLE.advertise(&advData);
}

void loop() {
  digitalWrite(POWER_ATLAS, HIGH);
  digitalWrite(POWER_TEMP, HIGH);
  delay(200);
  measure_ph();
  measure_orp();
  measure_temp();
  digitalWrite(POWER_ATLAS, LOW);
  digitalWrite(POWER_TEMP, LOW);
  delay(3000);

  if (millis() - lastUpdate >= UPDATE_INTERVAL_MS) {
    lastUpdate = millis();
    if (BLE.connected()) {
      //digitalWrite(BLUE_LED,HIGH);
      tempCharacteristic.setValue(String(temp));
      phCharacteristic.setValue(String(ph));
      orpCharacteristic.setValue(String(orp));
    } else {
      //digitalWrite(BLUE_LED,LOW);
    }

  }
}

uint32_t ieee11073_from_float(float temperature) {
    // This code is from the ARM mbed temperature demo:
    // https://github.com/ARMmbed/ble/blob/master/ble/services/HealthThermometerService.h
    // I'm pretty sure this only works for positive values of temperature, but that's OK for the health thermometer.
    uint8_t  exponent = 0xFE; // Exponent is -2
    uint32_t mantissa = (uint32_t)(temperature * 100);

    return (((uint32_t)exponent) << 24) | mantissa;
}


void measure_ph() {
  String pH_data = executeRequest(PH_ADDRESS,"R");
  Serial.print("Ph:");
  Serial.println(pH_data);
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
  Serial.print("ORP:");
  Serial.println(orp_data);
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
  Serial.print("TEMP:");
  Serial.println(temp);
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