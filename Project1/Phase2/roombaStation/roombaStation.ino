#include <Servo.h>

Servo servoX;
Servo servoY;
int command;
int comInfo;
int curAngleX;
int curAngleY;
int laserPin = 53;
int servoXpin = 52;
int servoYpin = 50;

void setup() {
  pinMode(laserPin, OUTPUT);
  servoX.attach(servoXpin);
  servoY.attach(servoYpin);
  Serial1.begin(9600);
  Serial.begin(9600);
}

void loop() {
  //get command from base station
  command = Serial1.read();

  switch(command){
    case 0:
      curAngleX = Serial1.read();
      Serial.print(curAngleX); //for testing purposes to protect servo motor
      delay(500);
      //servoX.write(curAngleX);
      break;
    case 1:  
      curAngleY = Serial1.read();
      Serial.print(curAngleY); //for testing purposes to protect servo motor
      delay(500);
      //servoY.write(curAngleY);
      break;
    case 2:
      comInfo = Serial1.read();
      if(comInfo){
        digitalWrite(laserPin, LOW);
      } else {
        digitalWrite(laserPin, HIGH);
      }
      break;
  }
  
}
