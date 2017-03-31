#include <avr/io.h>

typedef enum commands { 
  ROOMBA = 0, 
  SERVO, 
  LASER,
} COMMANDS;

void initLed();
void enableLed();
void disableLed();
void toggleLed();

void initBluetooth();
void sendBluetooth(uint8_t data);
uint8_t receiveBluetooth();
