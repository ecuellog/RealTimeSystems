/*
 * Project 1 Phase 2
 * Authors: Konrad Schultz, Edgardo Cuello
 */

#include <Servo.h>
#include "scheduler.h"
#include "Roomba_Driver.h"

Servo servoX;
Servo servoY;

// Pins
uint8_t laserPin = 51;
uint8_t servoXPin = 52;
uint8_t servoYPin = 50;
Roomba r(2,30);

void setup() {
  pinMode(laserPin, OUTPUT);
  servoX.attach(servoXPin);
  servoY.attach(servoYPin);
  
  // Roomba
  r.init();
  
  // Bluetooth
  Serial1.begin(9600);
  Serial.begin(9600);

  // Scheduler
  Scheduler_Init();
  Scheduler_StartTask(0, 25, checkBluetooth);
}

bool initialized = true;

void loop() {
  Scheduler_Dispatch();
}

void clearSerial(uint8_t count) {
  for(int i = 0; i < count; i++) {
    Serial1.read();
  }
}

void checkBluetooth() {
  if(Serial1.available() < 5) {
    return;
  }

  uint8_t command = Serial1.read();
  
  if(!initialized) {
      r.init();
      initialized = true;
  }

  uint8_t curAngle, comInfo;
  switch(command) {
    // servoX
    case 0:
      curAngle = Serial1.read();
      clearSerial(3);
      servoX.write(curAngle);
      break;
   
    // servoY
    case 1:  
        curAngle = Serial1.read();
        clearSerial(3);
        servoY.write(curAngle);
      break;
    
    // laser
    case 2:
      comInfo = Serial1.read();
      if(comInfo){
        digitalWrite(laserPin, LOW);
      } else {
        digitalWrite(laserPin, HIGH);
      }
      clearSerial(3);
      break;
    
    //Roomba movements
    case 3:
      int8_t speedHigh = Serial1.read();
      int8_t speedLow = Serial1.read();
      int8_t radHigh = Serial1.read();
      int8_t radLow = Serial1.read();

      int actSpeed = speedHigh*256 + speedLow;
      int actRad = radHigh*256 + radLow;

      Serial.println(actSpeed);
      Serial.println(actRad);
      break;
     
    /* case 'f': 
        r.drive(150, 32768);
        Serial.println("FORWARD");
        break;
      case 'b':
        r.drive(-150, 32768);
        Serial.println("BACK");
        break;
      case 'r':
        r.drive(50, -1);
        break;
      case 'l':
        r.drive(50, 1);
        break;
      case 's':
        r.drive(0,0);
       // Serial.println("STOP");
        break;
      case 'd':
        r.dock();
        break;
      case 'p':
        r.power_off();
        initialized = false;
        break;*/

      
  }
}

