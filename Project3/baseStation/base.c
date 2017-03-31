/*
 * Project 3: Base Station
 * Authors: Konrad Schultz, Edgardo Cuello
 */

#include <util/delay.h>
#include "../common/os.h"
#include "../common/utility.h"
#include "baseUtility.h"

void readJsRoombaTask() {
  uint16_t rx, ry; // value between 0-1024

  for(;;) {
    rx = readAdc(8);
    ry = readAdc(9);

    sendBluetooth('<');
    sendBluetooth(ROOMBA);
    sendBluetooth(rx >> 8);
    sendBluetooth(rx & 0xff);
    sendBluetooth(ry >> 8);
    sendBluetooth(ry & 0xff);
    
    Task_Next();
  }
}

void readJsServoTask() {
  uint16_t sx, sy; // value between 0-1024

  for(;;) {
    sx = readAdc(10);
    sy = readAdc(11);

    sendBluetooth('<');
    sendBluetooth(SERVO);
    sendBluetooth(sx >> 8);
    sendBluetooth(sx & 0xff);
    sendBluetooth(sy >> 8);
    sendBluetooth(sy & 0xff);

    Task_Next();
  }
}

void readButtonTask() {
  uint8_t b; // 0 for pressed 1 for open
  for(;;) {
    b = readButton();
    sendBluetooth('<');
    sendBluetooth(LASER);
    sendBluetooth(b);

    Task_Next();
  }
}

void initialize() {
  initAdc();
  initBluetooth();
  initButton();
  initLed();
}

void a_main() {
  Task_Create_System(initialize, 0);
  // Task_Create_Period(readJsRoombaTask, 0, 10, 1, 0);
  // Task_Create_Period(readJsServoTask, 0, 10, 1, 1);
  // Task_Create_Period(readButtonTask, 0, 10, 1, 2);
}