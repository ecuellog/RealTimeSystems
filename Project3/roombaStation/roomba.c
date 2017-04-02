/*
 * Project 3: Roomba Station
 * Authors: Konrad Schultz, Edgardo Cuello
 */

#include "../common/os.h"
#include "../common/utility.h"
#include "roombaUtility.h"

void checkBluetoothTask() {
  /* Servo */
  uint8_t joyXhigh;
  uint8_t joyXlow;
  uint8_t joyYhigh;
  uint8_t joyYlow;
  int joyX;
  int joyY;
  
  /* Roomba */
  uint8_t speedHigh;
  uint8_t speedLow;
  uint8_t radHigh;
  uint8_t radLow;
  int speed;
  int rad;
  
  for(;;) {
    /* consume until flag */
    while(receiveBluetooth() != '<');
    
    uint8_t command = receiveBluetooth();

    switch(command) {
    case SERVO:
      joyXhigh = receiveBluetooth();
      joyXlow = receiveBluetooth();
      joyX = joyXhigh * 256 + joyXlow;
      setServoX(joyX);

      joyYhigh = receiveBluetooth();
      joyYlow = receiveBluetooth();
      joyY = joyYhigh * 256 + joyYlow;
      setServoY(joyY);
      break;
    case ROOMBA:
      speedHigh = receiveBluetooth();
      speedLow = receiveBluetooth();
      speed = speedHigh * 256 + speedLow;

      radHigh = receiveBluetooth();
      radLow = receiveBluetooth();
      rad = radHigh * 256 + radLow;
      moveRoomba(speed, rad);
      break;
    case LASER:
      if(receiveBluetooth() == 0) {
        enableLaser();
      } else {
        disableLaser();
      }
      break;
    default: break;
    }
  }
}

/*
 * Check Roomba sensors. If a sensor is triggered, create a system task so that it 
 * overrides any roomba movement and move the roomba accordingly.
 */
void checkRoombaTask() { }

void initialize() {
  initLed();
  initLaser();
  initBluetooth();
  initServo();
  initRoomba();
}

void a_main() {
  Task_Create_System(initialize, 0);
  Task_Create_System(checkBluetoothTask, 0);
}
