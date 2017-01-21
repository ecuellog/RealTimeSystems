#include <Servo.h>

int JoyStick_X = A0; // x
int JoyStick_Y = A1; // y
Servo servoX;
int curAngleX;
int angleRateX;

void setup ()
{
  pinMode (JoyStick_X, INPUT);
  pinMode (JoyStick_Y, INPUT);
  //servoX.attach(52);
 // curAngleX = 90;
 // servoX.write(curAngleX);
  Serial.begin (9600); // 9600 bps
}

void loop ()
{
  int x, y, z;
  x = analogRead (JoyStick_X);
  y = analogRead (JoyStick_Y);

  angleRateX = (x-513)/103; //for a maximum of 5 degrees for full joystick input

 /* if( curAngleX > 20 or curAngleX < 160){
    curAngleX = curAngleX + angleRateX;
    servoX.write(curAngleX + angleRateX);
  }*/


  //Serial.print ("x value - "); // 0 - 445 - 904
  //Serial.print (x);
  Serial.print(angleRateX);
  Serial.print(" -  ");
  //Serial.print ("y value - "); //0 - 452 - 904 
  //Serial.print (y, DEC);
 // Serial.print (",");
 // Serial.println (z, DEC);
  delay (1000);
}
