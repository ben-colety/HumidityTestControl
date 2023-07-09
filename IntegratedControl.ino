//adapted from FDC-Serial example code from FDC2214 library https://github.com/zharijs/FDC2214
//Created by Benjamin Colety

#include <Wire.h>
#include "FDC2214.h"
FDC2214 capsense(FDC2214_I2C_ADDR_0); // Use FDC2214_I2C_ADDR_1 

#define CHAN_COUNT 1  //number of channels to expect to read from FDC
#define CLOSED LOW
#define OPEN  HIGH
#define STARTUP_DELAY_MS 10000 //10s startup delay
#define PRE_STEP_MS 5000
#define POST_STEP_MS 5000
#define PULSE_DURATION_MS 1000
#define INSPECT_TIME 5*60000


unsigned long time_us;  //time in microseconds
unsigned long time_ms;  //time in milliseconds


const int pumpPin = 2;      // pin number of the humid pump
const int valvePinN2 = 3;   // pin number of dry valve
const int valvePinH2O = 4;  // pin number of humid valve
int pumpState = CLOSED;     // pump off
int valveStateH2O = CLOSED; // valve closed
int valveStateN2 = CLOSED;  // valve closed

const int calibrationSignalPin = 12; //pin number of calibration signal


void setup() {

  delay(5000);
  //setup valves
  pinMode(pumpPin, OUTPUT);
  pinMode(valvePinN2, OUTPUT);
  pinMode(valvePinH2O, OUTPUT);
  digitalWrite(pumpPin, pumpState);
  digitalWrite(valvePinN2, valveStateN2);
  digitalWrite(valvePinH2O, valveStateH2O);

  pinMode(calibrationSignalPin, OUTPUT);
  digitalWrite(calibrationSignalPin, LOW);

  //setup FDC2214
  // ### Start I2C 
  Wire.begin();  
  // ### Start serial
  Serial.begin(115200);
  Serial.println("\nFDC2x1x test");
  
  // ### Start FDC
  //setup channel 0, single channel, deglitch at 10MHz, external oscillator 
  bool capOk = capsense.begin(0b00, 0x0, 0x5, false); 
  if (capOk) Serial.println("FDC Sensor OK");  
  else Serial.println("FDC Sensor Fail");
}


void loop() {
  //printDataFDC();
  if (Serial.available() > 0) {
  // read the incoming string:
  String incomingString = Serial.readStringUntil(';');

  // prints the received data
  Serial.print("I received: ");
  Serial.println(incomingString);
  if(incomingString == "humid step")
    humidStep();
  if(incomingString == "dry step")
    dryStep();
  if(incomingString == "humid pulse")
    humidPulse();
  if(incomingString == "dry pulse")
    dryPulse();
  if(incomingString == "inspect")
    inspect(INSPECT_TIME);
  if(incomingString == "test")
    test();
  if(incomingString == "cold")
    coldTest();
  if(incomingString == "depressurize")
    depressurize();
  if(incomingString == "pressurize")
    pressurize();
  if(incomingString == "humid open only"){
    digitalWrite(valvePinH2O, OPEN);
    digitalWrite(valvePinN2, CLOSED);
    }
  if(incomingString == "dry open only"){
    digitalWrite(valvePinH2O, CLOSED);
    digitalWrite(valvePinN2, OPEN);
    }
  if(incomingString == "humid open"){
    digitalWrite(valvePinH2O, OPEN);
    }
  if(incomingString == "dry open"){
    digitalWrite(valvePinN2, OPEN);
    }
  if(incomingString == "both open"){
    digitalWrite(valvePinH2O, OPEN);
    digitalWrite(valvePinN2, OPEN);    
    }
  if(incomingString == "humid close"){
    digitalWrite(valvePinH2O, CLOSED);
    }
  if(incomingString == "dry close"){
    digitalWrite(valvePinN2, CLOSED);
    }
  if(incomingString == "both close"){
    digitalWrite(valvePinH2O, CLOSED);
    digitalWrite(valvePinN2, CLOSED);    
    }
  if(incomingString == "pump open")
    digitalWrite(pumpPin, OPEN);
  if(incomingString == "pump close")
    digitalWrite(pumpPin, CLOSED);
  }
}

void printDataFDC(){
  unsigned long data = capsense.getReading28(0);  //read channel 0
  Serial.print(millis());
  Serial.print(';');
  Serial.println(data);
}

void inspect(unsigned long duration_ms){
  Serial.println("inspecting");
  Serial.println("time;humidity");
  unsigned long start_time = millis();
  while(millis()-start_time <= duration_ms){
    printDataFDC();
  }
}

void test(){
  depressurize();
  Serial.println("start test");
  Serial.println("time;humidity");
  digitalWrite(calibrationSignalPin, LOW);
  unsigned long start_time = millis();
  digitalWrite(valvePinH2O, CLOSED);
  digitalWrite(pumpPin, OPEN);
  digitalWrite(valvePinN2, OPEN);
  //dry input during 30s to purge
  while(millis()-start_time <= 30000){ //guess and check value
    printDataFDC();
  }
  //Serial.println("purge Complete");
  start_time = millis();
  digitalWrite(calibrationSignalPin, HIGH); //rising signal
  digitalWrite(valvePinH2O, OPEN);
  digitalWrite(valvePinN2, CLOSED);
  // humid input during 30s
  while(millis()-start_time <= 30000){ //guess and check value
    printDataFDC();
  }
  start_time = millis();
  digitalWrite(calibrationSignalPin, LOW);  //descending signal
  digitalWrite(valvePinH2O, CLOSED);
  digitalWrite(pumpPin, CLOSED);
  digitalWrite(valvePinN2, HIGH);
  // dry input during 30s
  while(millis()-start_time <= 30000){ //guess and check value
    printDataFDC();
  }
  digitalWrite(valvePinH2O, CLOSED);
  digitalWrite(valvePinN2, CLOSED);

  digitalWrite(calibrationSignalPin, HIGH); //end pulse signal
  delay(1000);
  digitalWrite(calibrationSignalPin, LOW); 

  Serial.println("End Calibration");
}

void humidStep(){ //steps from dry to humid
  Serial.println("start humid step");
  Serial.println("time;humidity");
  unsigned long start_time = millis();
  digitalWrite(valvePinH2O, CLOSED);
  digitalWrite(valvePinN2, OPEN);

  while(millis()-start_time <= PRE_STEP_MS){
    printDataFDC();
  }
  Serial.println("step");
  digitalWrite(valvePinN2, CLOSED);
  digitalWrite(valvePinH2O, OPEN);

  while(millis()-start_time-PRE_STEP_MS <= POST_STEP_MS){
    printDataFDC();
  }
  
  digitalWrite(valvePinH2O, CLOSED);
  digitalWrite(valvePinN2, CLOSED);
  
  Serial.println("end humid step");
}

void dryStep(){
  Serial.println("start humid step");
  Serial.println("time;humidity");

  unsigned long start_time = millis();
  digitalWrite(valvePinH2O, OPEN);
  digitalWrite(valvePinN2, CLOSED);

  while(millis()-start_time <= PRE_STEP_MS){
    printDataFDC();
  }

  Serial.println("step");
  digitalWrite(valvePinN2, OPEN);
  digitalWrite(valvePinH2O, CLOSED);

  while(millis()-start_time-PRE_STEP_MS <= POST_STEP_MS){
    printDataFDC();
  }
  
  digitalWrite(valvePinH2O, CLOSED);
  digitalWrite(valvePinN2, CLOSED);
  
  Serial.println("end dry step");
}

void humidPulse(){
  Serial.println("start humid pulse");
  Serial.println("time;humidity");

  unsigned long start_time = millis();
  digitalWrite(valvePinH2O, CLOSED);  //drying
  digitalWrite(valvePinN2, OPEN);

  while(millis()-start_time <= PRE_STEP_MS){
    printDataFDC();
  }

  //Serial.println("valves");
  digitalWrite(valvePinN2, CLOSED); //pulse
  digitalWrite(valvePinH2O, OPEN);

  while(millis()-start_time-PRE_STEP_MS <= PULSE_DURATION_MS){
    printDataFDC();
  }
  //Serial.println("valves");
  digitalWrite(valvePinH2O, CLOSED);  //drying
  digitalWrite(valvePinN2, OPEN);

  while(millis()-start_time-PRE_STEP_MS-PULSE_DURATION_MS <= POST_STEP_MS){
    printDataFDC();
  }

  digitalWrite(valvePinH2O, CLOSED);
  digitalWrite(valvePinN2, CLOSED);
  
  Serial.println("end humid pulse");
}

void dryPulse(){
  Serial.println("start dry pulse");
  Serial.println("time;humidity");
  unsigned long start_time = millis();
  digitalWrite(valvePinH2O, OPEN);
  digitalWrite(valvePinN2, CLOSED);

  while(millis()-start_time <= PRE_STEP_MS){
    printDataFDC();
  }

  Serial.println("valves");
  digitalWrite(valvePinN2, OPEN);
  digitalWrite(valvePinH2O, CLOSED);

  while(millis()-start_time-PRE_STEP_MS <= PULSE_DURATION_MS){
    printDataFDC();
  }
  Serial.println("valves");
  digitalWrite(valvePinH2O, OPEN);
  digitalWrite(valvePinN2, CLOSED);

  while(millis()-start_time-PRE_STEP_MS-PULSE_DURATION_MS <= POST_STEP_MS){
    printDataFDC();
  }

  digitalWrite(valvePinH2O, CLOSED);
  digitalWrite(valvePinN2, CLOSED);
  
  Serial.println("end dry pulse");
}



void depressurize(){
  Serial.println("releasing pressure");
  digitalWrite(pumpPin, CLOSED);
  digitalWrite(valvePinH2O, OPEN);
  digitalWrite(valvePinN2, OPEN);
  delay(5000);
  digitalWrite(valvePinH2O, CLOSED);
  digitalWrite(valvePinN2, CLOSED);
  Serial.println("depressurization complete");
}

void pressurize(){ //fill humid reservoir
  Serial.println("filling reservoir");
  digitalWrite(valvePinH2O, CLOSED);
  digitalWrite(valvePinN2, CLOSED);
  digitalWrite(pumpPin, OPEN);
  delay(10000);
  digitalWrite(pumpPin, CLOSED);
}

void coldTest(){
  depressurize();
  Serial.println("start test");
  Serial.println("time;humidity");
  digitalWrite(calibrationSignalPin, LOW);
  unsigned long start_time = millis();
  digitalWrite(valvePinH2O, CLOSED);
  digitalWrite(pumpPin, CLOSED);
  digitalWrite(valvePinN2, OPEN);
  //dry input during 30s to purge & decrease test temperature
  while(millis()-start_time <= 20000){ //guess and check value
    printDataFDC();
  }
  start_time = millis();
  digitalWrite(pumpPin, OPEN);
  while(millis()-start_time <= 10000){ //guess and check value
    printDataFDC();
  }
  //Serial.println("purge Complete");
  start_time = millis();
  digitalWrite(calibrationSignalPin, HIGH); //rising signal
  digitalWrite(valvePinH2O, OPEN);
  digitalWrite(valvePinN2, CLOSED);
  // humid impulse during 30s
  while(millis()-start_time <= 10000){
    printDataFDC();
  }
  digitalWrite(pumpPin, CLOSED);
  start_time = millis();
  while(millis()-start_time <= 20000){
    printDataFDC();
  }
  start_time = millis();
  digitalWrite(calibrationSignalPin, LOW);  //descending signal
  digitalWrite(valvePinH2O, CLOSED);
  digitalWrite(pumpPin, CLOSED);
  digitalWrite(valvePinN2, HIGH);
  // dry input during 30s
  while(millis()-start_time <= 30000){ //guess and check value
    printDataFDC();
  }
  digitalWrite(valvePinH2O, CLOSED);
  digitalWrite(valvePinN2, CLOSED);

  digitalWrite(calibrationSignalPin, HIGH); //end pulse signal
  delay(1000);
  digitalWrite(calibrationSignalPin, LOW); 

  Serial.println("End Calibration");
}