#include <avr/io.h>

void initLed();
void enableLed();
void disableLed();
void toggleLed();
void initAdc();
uint16_t readAdc(uint8_t pin);
