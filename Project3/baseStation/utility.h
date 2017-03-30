#include <avr/io.h>

void initLed();
void enableLed();
void disableLed();
void toggleLed();

void initAdc();
uint16_t readAdc(uint8_t pin);

void initButton();
uint8_t readButton();

void initBluetooth();
void sendBluetooth(uint8_t data);
