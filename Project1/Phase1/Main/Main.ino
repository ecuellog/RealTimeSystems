/*
 * Project 1 Phase 1
 * Authors: Konrad Schultz, Edgardo Cuello
 */
#include <LiquidCrystal.h>
#include <Servo.h>

// initialize the library with the numbers of the interface pins.
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int laserPin = 53;
int lightSensorPin = 7;

int currentLight = 0;
int ambientLight = 0;

int JoyStick_X = A9; // x
int JoyStick_Y = A8; // y
Servo servoX;
Servo servoY;
int curAngleX;
int angleRateX;
int curAngleY;
int angleRateY;
int zbutton = 38;

void setup() {
  pinMode(laserPin, OUTPUT);
  ambientLight = analogRead(lightSensorPin);
  currentLight = ambientLight;
  lcd.begin(16,2);
  //Joystick and Servo Setup
  pinMode (JoyStick_X, INPUT);
  pinMode (JoyStick_Y, INPUT);
  pinMode (zbutton, INPUT);
  servoX.attach(52);
  servoY.attach(50);
  curAngleX = 90;
  curAngleY = 90;
  servoX.write(curAngleX);
  servoY.write(curAngleY);
}

void loop() {
  checkLight();
  refreshLcd();
  servoMove();
  buttonLaser();
  delay(25);
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

  servoY.write(curAngleY);
  servoX.write(curAngleX);
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
  if(digitalRead(zbutton)){
    digitalWrite(laserPin, LOW);
  } else {
    digitalWrite (laserPin, HIGH);
  }
}

