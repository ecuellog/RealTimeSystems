#include "baseUtility.h"

void initAdc() {
  ADMUX |= 1 << REFS0;
  ADCSRA |= (1 << ADEN) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2); // prescaler 128
}

uint16_t readAdc(uint8_t pin) {
  ADMUX &= 0xE0;
  ADMUX |= pin & 0x07;
  ADCSRB = pin & (1 << 3);
  ADCSRA |= 1 << ADSC;
  
  while(ADCSRA & (1 << ADSC));

  return ADCW;
}

void initButton() {
  DDRB &= ~(1 << 1); // pin 52
}

uint8_t readButton() {
  return PINB & (1 << 1); // pin 52
}