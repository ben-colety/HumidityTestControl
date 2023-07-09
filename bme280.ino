/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2650

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface. The device's I2C address is either 0x76 or 0x77.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
  See the LICENSE file for details.
 ***************************************************************************/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C

const int calibrationSignalPin = 3;

bool calRead = LOW;
int calibrationState = 0;

unsigned long last_cal_change;  //time of last calibration state change
unsigned long previousMillis;
unsigned long BMEdelayTime;

void setup() {
  //configure calibration pin
  pinMode(calibrationSignalPin, INPUT);
  
  //configure BME
  Serial.begin(9600);
  while(!Serial);    // time to get serial running
  Serial.println(F("BME280 test"));

  unsigned status;
  
  // default settings
  status = bme.begin();  
  // You can also pass in a Wire library object like &Wire2
  // status = bme.begin(0x76, &Wire2)
  if (!status) {
      Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
      Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
      Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
      Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
      Serial.print("        ID of 0x60 represents a BME 280.\n");
      Serial.print("        ID of 0x61 represents a BME 680.\n");
      while (1) delay(10);
  }
  
  Serial.println("-- Default Test --");
  BMEdelayTime = 100;
  previousMillis = millis();
  Serial.println("Time;RH;Temp");
}


void loop(){
  checkCalibration();
  if(millis() - previousMillis >= BMEdelayTime){
     printValues();
     previousMillis = millis();
  }
}


void printValues() {
  Serial.print(millis());
  Serial.print(';');
  Serial.print(bme.readHumidity());
  Serial.print(';');
  Serial.println(bme.readTemperature());


}

void checkCalibration(){
  bool new_calRead = digitalRead(calibrationSignalPin);
  if(calRead != new_calRead && (millis() - last_cal_change >= 750)){
    calRead = new_calRead;
    last_cal_change = millis();
    calibrationState = calibrationState + 1;
    Serial.println(calibrationState);
  }
}
