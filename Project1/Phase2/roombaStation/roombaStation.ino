/*
 * Project 1 Phase 2
 * Authors: Konrad Schultz, Edguardo Cuello
 */

#include <Servo.h>

#include "scheduler.h"

Servo servoX;
Servo servoY;

// Pins
uint8_t laserPin = 51;
uint8_t servoXPin = 52;
uint8_t servoYPin = 50;

void setup() {
  pinMode(laserPin, OUTPUT);
  servoX.attach(servoXPin);
  servoY.attach(servoYPin);
  
  // Bluetooth
  Serial1.begin(9600);

  // Scheduler
  Scheduler_Init();
  Scheduler_StartTask(10, 0, checkBluetooth);
}

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
  }
}

