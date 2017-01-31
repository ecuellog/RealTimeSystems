#include <Servo.h>

Servo servoX;
Servo servoY;
int curAngleX;
int curAngleY;
int laserPin = 51;
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
  checkBT();
  
}

void checkBT() {
  //get command from base station
  uint8_t command = -1;
  
  if(Serial1.available()){
    command = Serial1.read();
  } else {
    return;
  }

  delay(5);
  switch(command){
    case 0:
      if(Serial1.available()){
        curAngleX = Serial1.read();
        Serial.print("servox");
        Serial.println(curAngleX); //for testing purposes to protect servo motor
        servoX.write(curAngleX);
      }
      break;
    case 1:  
      if(Serial1.available()){
        curAngleY = Serial1.read();
        Serial.print("servoy");
        Serial.println(curAngleY); //for testing purposes to protect servo motor
        servoY.write(curAngleY);
      }
      break;
    case 2:
    
      if(Serial1.available()){
        uint8_t comInfo = Serial1.read();
        Serial.println("laser");
        if(comInfo){
          digitalWrite(laserPin, LOW);
        } else {
          digitalWrite(laserPin, HIGH);
        }
      }
     
      break;
  }
  
}
