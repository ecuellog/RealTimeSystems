/*
 * Project 3: Base Station
 * Authors: Konrad Schultz, Edgardo Cuello
 */

#include "utility.h"

uint16_t rx, ry;
void readJsRoomba() {
  rx = readAdc(8);
  ry = readAdc(9);
}

uint16_t sx, sy;
void readJsServo() {
  sx = readAdc(10);
  sy = readAdc(11);
}

int main() {
  initLed();
  initAdc();

  for(;;) {
    readJsServo();
    // readJsRoomba();

    if(sx < 500 || sy < 500) {
      enableLed();
    } else {
      disableLed();
    }
  }
  return 0;
}