/*
 * Project 1 Phase 1
 * Authors: Konrad Schultz, Edguardo Cuello
 */
#include <LiquidCrystal.h>
#include <Servo.h>

#include "scheduler.h"

// initialize the library with the numbers of the interface pins.
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

uint8_t laserPin = 53;
uint8_t lightSensorPin = 7;

uint32_t currentLight = 0;
uint32_t ambientLight = 0;

uint8_t JoyStick_X = A9; // x
uint8_t JoyStick_Y = A8; // y
uint8_t curAngleX;
int angleRateX;
uint8_t curAngleY;
int angleRateY;
uint8_t zbutton = 38;

// Bluetooth commands
uint8_t angleXCmd = 0;
uint8_t angleYCmd = 1;
uint8_t laserCmd = 2;

void setup() {
  // light sensor
  ambientLight = analogRead(lightSensorPin);
  currentLight = ambientLight;
  
  lcd.begin(16,2);

  Serial1.begin(9600);
  
  //Joystick
  pinMode (JoyStick_X, INPUT);
  pinMode (JoyStick_Y, INPUT);
  pinMode (zbutton, INPUT);
  curAngleX = 90;
  curAngleY = 90;

  // Scheduler
  Scheduler_Init();
  Scheduler_StartTask(10, 500, refreshLcd);
  Scheduler_StartTask(17, 100, checkLight);
  Scheduler_StartTask(0, 25, servoMove);
  Scheduler_StartTask(6, 50, buttonLaser);
}

void loop() {
  Scheduler_Dispatch();
}

void servoMove() {
  int x, y, z;
  x = analogRead (JoyStick_X);
  y = analogRead (JoyStick_Y);
  
  angleRateX = (x-513)/103; //for a maximum of 5 degrees for full joystick input
  angleRateY = (y-513)/103;

  if( curAngleX < 20 ){
    curAngleX = curAngleX + 5;  
  }
  else if(curAngleX > 160){
    curAngleX = curAngleX - 5;  
  }
  else{
    curAngleX = curAngleX + angleRateX;
  }
  
  if( curAngleY < 20 ){
    curAngleY = curAngleY + 5;  
  }
  else if(curAngleY > 160){
    curAngleY = curAngleY - 5;  
  }
  else{
    curAngleY = curAngleY + angleRateY;
  }

  Serial1.write(angleXCmd);
  Serial1.write(curAngleX);
  Serial1.write(angleYCmd);
  Serial1.write(curAngleY);
}

void checkLight() {
    // light goes up by 100
    currentLight = analogRead(lightSensorPin);
}

void refreshLcd() {
  lcd.clear();
  int lightDelta = currentLight - ambientLight;
  if (lightDelta > 100) {
    lcd.print("ON : ");
  } else {
    lcd.print("OFF: ");
  }
  lcd.print(lightDelta);
  lcd.setCursor(0, 1);
  lcd.print("x: ");
  lcd.print(curAngleX);
  lcd.print(" y: ");
  lcd.print(curAngleY);
}

void buttonLaser() {
  Serial1.write(laserCmd);
  Serial1.write(digitalRead(zbutton));
}

