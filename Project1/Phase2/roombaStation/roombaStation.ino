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
  Scheduler_StartTask(0, 10, checkBluetooth);
}

bool initialized = true;

void loop() {
  Scheduler_Dispatch();
}

void checkBluetooth() {
  uint8_t command = -1;
  
  if(Serial1.available()) {
    command = Serial1.read();
  } else {
    return;
  }
  
  if(!initialized) {
      r.init();
      initialized = true;
  }

  switch(command) {
    // servoX
    case 0:
      if(Serial1.available()) {
        uint8_t curAngleX = Serial1.read();
        servoX.write(curAngleX);
      }
      break;
   
    // servoY
    case 1:  
      if(Serial1.available()) {
        uint8_t curAngleY = Serial1.read();
        servoY.write(curAngleY);
      }
      break;
    
    // laser
    case 2:
      if(Serial1.available()) {
        uint8_t comInfo = Serial1.read();
        if(comInfo){
          digitalWrite(laserPin, LOW);
        } else {
          digitalWrite(laserPin, HIGH);
        }
      }
      break;
     //Roomba movements
    case 3:
      Serial.println("In case 3");
      delay(5);
      //if(Serial1.available()){
        int8_t speedHigh = Serial1.read();
        delay(5);
        int8_t speedLow = Serial1.read();
        delay(5);
        int8_t radHigh = Serial1.read();
        delay(5);
        int8_t radLow = Serial1.read();

        int actSpeed = speedHigh*256 + speedLow;
        int actRad = radHigh*256 + radLow;

        Serial.println(actSpeed);
        Serial.println(actRad);
     // }
     
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

