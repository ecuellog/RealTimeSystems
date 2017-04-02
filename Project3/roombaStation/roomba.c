/*
 * Project 3: Roomba Station
 * Authors: Konrad Schultz, Edgardo Cuello
 */

#include "../common/os.h"
#include "../common/utility.h"
#include "roombaUtility.h"
#include "roombaMotion.h"

int curAngleX;
int curAngleY;

void servoX(int joyX) {
  int angleRateX = (joyX - 513) / 103;
	
  if( curAngleX < 135 ) {
    curAngleX = curAngleX + 5;  
  } else if(curAngleX > 240) {
    curAngleX = curAngleX - 5;  
  } else {
    curAngleX = curAngleX + angleRateX;
  }
    
  OCR3C = curAngleX;
}

void servoY(int joyY) {
  int angleRateY = (joyY - 513) / 103;

  if( curAngleY < 135 ) {
    curAngleY = curAngleY + 5;  
  } else if(curAngleY > 240) {
    curAngleY = curAngleY - 5;  
  } else {
    curAngleY = curAngleY + angleRateY;
  }
	
  OCR3B = curAngleY;
}

void laser(uint8_t laserButton) {
  if(laserButton) enableLaser();
  else disableLaser();
}

void roomba_movement(int speed, int radius) {
  speed = -(speed - 512);
  int actRad;
  int actSpeed = speed / 2;
  
  if(radius > 900) {
    actRad = -1;
    actSpeed = 50;
  } else if(radius > 800) {
    actRad = -200;
  } else if(radius > 700) {
    actRad = -400;
  } else if(radius > 600) {
    actRad = -750;
  } else if(radius > 530) {
    actRad = -1500;
  } else if(radius > 490) {
    actRad = 32767;
  } else if(radius > 400) {
    actRad = 1500;
  } else if(radius > 300) {
    actRad = 750;
  } else if(radius > 200) {
    actRad = 400;
  } else if(radius > 100){
    actRad = 200;
  } else {
    actRad = 1;
    actSpeed = 50;
  }
  
  driveRoomba(actSpeed, actRad);
}

void checkBluetoothTask() {
  //for(;;){
  uint8_t command = receiveBluetooth();
  /* Servo variables */
  uint8_t joyXhigh;
  uint8_t joyXlow;
  uint8_t joyYhigh;
  uint8_t joyYlow;
  int joyX;
  int joyY;
  
  /* Laser Variables */
  uint8_t laserButton;
  
  /*Roomba variables */
  uint8_t speedHigh;
  uint8_t speedLow;
  uint8_t radHigh;
  uint8_t radLow;
  int speed;
  int rad;
  
  switch(command) {
    case SERVO:
      joyXhigh = receiveBluetooth();
      joyXlow = receiveBluetooth();
      joyYhigh = receiveBluetooth();
      joyYlow = receiveBluetooth();
      joyX = joyXhigh*256 + joyXlow;
      joyY = joyYhigh*256 + joyYlow;
      servoX(joyX);
      servoY(joyY);
      break;
    case ROOMBA:
      speedHigh = receiveBluetooth();
      speedLow = receiveBluetooth();
      radHigh = receiveBluetooth();
      radLow = receiveBluetooth();
      speed = speedHigh*256 + speedLow;
      rad = radHigh*256 + radLow;
      roomba_movement(speed, rad);
      break;
    case LASER:
      laserButton = receiveBluetooth();
      laser(laserButton);
      break;
    }
   // Task_Next();
   //}
  
}

/*
 * Check Roomba sensors. If a sensor is triggered, create a system task so that it 
 * overrides any roomba movement and move the roomba accordingly.
 */
void checkRoombaTask() { }


/*
 * Servo test
 * PE5/OC3C -> PWM output for timer 3 = digital pin 3
 * PE4/0C3B -> PWM output for timer 3 = digital pin 2
 */
void initServo() {
  // Setup ports and timers
  DDRE = 0xFF; // All output
  PORTE = 0;

  // Configure timer/counter3 as phase and frequency PWM mode
  TCNT3 = 0;  //set counter to 0
  TCCR3A = 0x00; 
  TCCR3B = 0x00;
  TCCR3A = (1 << COM3B1) | (1 << COM3C1) | (1 << WGM30);  //NON Inverted PWM
  TCCR3B |= (1 << WGM33) | (1 << CS41) | (1 << CS40); //PRESCALER=64 MODE 14 (FAST PWM)
  OCR3A = 2500;

  curAngleX = 188;
  curAngleY = 188;
  OCR3C = curAngleX; // 188 = 90 Degrees 125 = 0 d, 250 = 180 d
  OCR3B = curAngleY; // 90 Degrees
}

void initialize() {
  initLed();
  initLaser();
  initBluetooth();
  initServo();
  initRoombaUart();
  initRoomba();
}

void a_main() {
  Task_Create_System(initialize, 0);
  // // voidfuncptr f, int arg, TICK period, TICK wcet, TICK offset
  //Task_Create_Period(checkBluetoothTask, 0, 5, 1, 0);
  for(;;){
  	checkBluetoothTask();
  }
  // Task_Create_Period(checkRoombaTask, 0, 10, 1, 10);
  
}
