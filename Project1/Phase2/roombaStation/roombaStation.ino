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
Roomba r(3,53);

void setup() {
  pinMode(laserPin, OUTPUT);
  servoX.attach(servoXPin);
  servoY.attach(servoYPin);
  
  // Bluetooth
  Serial1.begin(9600);

  // Scheduler
  Scheduler_Init();
  Scheduler_StartTask(0, 5, checkBluetooth);

  // Roomba
  r.init();
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
      clearSerial(3);
      if(comInfo){
        digitalWrite(laserPin, LOW);
      } else {
        digitalWrite(laserPin, HIGH);
      }
      break;
    
    //Roomba movements
    case 3:
      uint8_t speedHigh = Serial1.read();
      uint8_t speedLow = Serial1.read();
      uint8_t radHigh = Serial1.read();
      uint8_t radLow = Serial1.read();

      int speedSig = speedHigh*256 + speedLow;  //Value from 0 - 1024
      int radSig = radHigh*256 + radLow;  //Value from 0 - 1024

      speedSig = -(speedSig - 512);
      radSig = radSig;
    
      int actRad;
      int actSpeed = speedSig / 2;

      if(radSig > 900) {
        actRad = -1;
        actSpeed = 50;
      } else if(radSig > 800) {
        actRad = -200;
      } else if(radSig > 700) {
        actRad = -400;
      } else if(radSig > 600) {
        actRad = -750;
      } else if(radSig > 530) {
        actRad = -1500;
      } else if(radSig > 490) {
        actRad = 32767;
      } else if(radSig > 400) {
        actRad = 1500;
      } else if(radSig > 300) {
        actRad = 750;
      } else if(radSig > 200) {
        actRad = 400;
      } else if(radSig > 100){
        actRad = 200;
      } else {
        actRad = 1;
        actSpeed = 50;
      }

      r.drive(actSpeed, actRad);
      
      break;
  }
}

