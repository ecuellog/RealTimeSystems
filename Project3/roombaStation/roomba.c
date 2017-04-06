/*
 * Project 3: Roomba Station
 * Authors: Konrad Schultz, Edgardo Cuello
 */

#include "../common/os.h"
#include "../common/utility.h"
#include "roombaUtility.h"


int speed;
int rad;
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

    // yield to the roomba check
    Task_Next();
  }
}

/*
 * Check Roomba sensors. If a sensor is triggered, create a system task so that it 
 * overrides any roomba movement and move the roomba accordingly.
 */
void checkRoombaTask() {
  for(;;) {
    queryRoomba();

    // 0 = no bump, 1 = bump
    uint8_t bumpState = receiveRoomba();

    // 0 = no virtual wall detected, 1 = virtual wall detected
    uint8_t irWallState = receiveRoomba();

    if (bumpState || irWallState) {
      // drive opposite current direction
      moveRoomba(-speed, -rad);
      _delay_ms(250);
    }

    // yield to bluetooth check
    Task_Next();
  }
}

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
  Task_Create_System(checkRoombaTask, 0);
}
