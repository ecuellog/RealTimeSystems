/*
 * Project 1 Phase 2
 * Authors: Konrad Schultz, Edguardo Cuello
 */
#include <LiquidCrystal.h>

#include "scheduler.h"

// Pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
uint8_t lightSensorPin = A15;
uint8_t servoJsX = A8;
uint8_t servoJsY = A9;
uint8_t zbutton = 38;
uint8_t roombaJsX = A10;
uint8_t roombaJsY = A11;

// Light sensor state
uint32_t currentLight = 0;
uint32_t ambientLight = 0;

// Servo state
uint8_t curAngleX;
int angleRateX;
uint8_t curAngleY;
int angleRateY;

// Bluetooth commands
uint8_t angleXCmd = 0;
uint8_t angleYCmd = 1;
uint8_t laserCmd = 2;
uint8_t roombaCmd = 3;

void setup() {
  Serial.begin(9600);
  
  // Light sensor
  ambientLight = analogRead(lightSensorPin);
  currentLight = ambientLight;
  
  lcd.begin(16,2);

  // BlueTooth
  Serial1.begin(9600);
  
  // Joystick
  pinMode(servoJsX, INPUT);
  pinMode(servoJsY, INPUT);
  pinMode(zbutton, INPUT);
  curAngleX = 90;
  curAngleY = 90;
  pinMode(roombaJsX, INPUT);
  pinMode(roombaJsY, INPUT);
  
  // Scheduler
  Scheduler_Init();
  Scheduler_StartTask(6, 1000, roombaMove);
//  Scheduler_StartTask(20, 500, refreshLcd);
//  Scheduler_StartTask(3, 100, checkLight);
//  Scheduler_StartTask(0, 25, servoMoveX);
//  Scheduler_StartTask(13, 25, servoMoveY);
//  Scheduler_StartTask(9, 100, buttonLaser);
}

void loop() {
   Scheduler_Dispatch();
}

void padSerial(uint8_t count) {
  for(int i = 0; i < count; i++) {
    Serial1.write(0);
  }
}

// speed from -150 to 150
// turning radius from -32768 to 32768
void roombaMove() {
  int v = analogRead(roombaJsY);
  int r = analogRead(roombaJsX);

  uint8_t vHigh = v / 256;
  uint8_t vLow = v % 256;
  
  Serial1.write(roombaCmd);
  Serial1.write(vHigh);
  Serial1.write(vLow);
  Serial1.write(r / 256);
  Serial1.write(r % 256);

  Serial.println(vHigh);
  Serial.println(vLow);
  Serial.println(vHigh * 256 + vLow);
}

void servoMoveY() {
  int y = analogRead(servoJsY);
  
  angleRateY = (y-513)/103;
  
  if( curAngleY < 20 ){
    curAngleY = curAngleY + 5;  
  } else if(curAngleY > 160){
    curAngleY = curAngleY - 5;  
  } else{
    curAngleY = curAngleY + angleRateY;
  }

  Serial1.write(angleYCmd);
  Serial1.write(curAngleY);
  padSerial(3);
}
void servoMoveX() {
  int x = analogRead(servoJsX);
  
  angleRateX = (x-513)/103; //for a maximum of 5 degrees for full joystick input

  if( curAngleX < 20 ){
    curAngleX = curAngleX + 5;  
  }
  else if(curAngleX > 160){
    curAngleX = curAngleX - 5;  
  }
  else{
    curAngleX = curAngleX + angleRateX;
  }

  Serial1.write(angleXCmd);
  Serial1.write(curAngleX);
  padSerial(3);
}

void checkLight() {
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
  uint8_t buttonVal = digitalRead(zbutton);
  
  Serial1.write(laserCmd);
  Serial1.write(buttonVal);
  padSerial(3);
}

