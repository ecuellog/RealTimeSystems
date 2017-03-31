#include "roombaUtility.h"

void initLaser() {
  DDRB |= 1 << 2;
}

void enableLaser() {
  PORTB |= 1 << 2;
}

void disableLaser() {
  PORTB &= ~(1 << 2);
}