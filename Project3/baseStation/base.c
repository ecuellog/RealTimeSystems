#include <avr/io.h>

void initLed() {
  DDRB |= 1 << 7;  // Set it all to output
}

void enableLed() {
  PORTB |= 1 << 7; // onboard LED
}

void disableLed() {
  PORTB &= ~(1 << 7);
}

void toggleLed() {
  PORTB ^= 1 << 7;
}

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