#include "utility.h"

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

void initButton() {
  DDRB &= ~(1 << 1);
}

uint8_t readButton() {
  return PINB & (1 << 1);
}

void initBluetooth() {
    // Set baud rate to 19.2k
    UBRR1 = 103;
    
    // Enable receiver, transmitter
    UCSR1B = (1 << RXEN1) | (1 << TXEN1);

    // 8-bit data
    UCSR1C = ((1 << UCSZ11) | (1 << UCSZ10));

    // disable 2x speed
    UCSR1A &= ~(1 << U2X1);
}

void sendBluetooth(uint8_t data){      
    while(!(UCSR1A & (1 << UDRE1)));
    UDR1 = data;
}
