#include <avr/io.h>

void initAdc();
uint16_t readAdc(uint8_t pin);

void initButton();
uint8_t readButton();