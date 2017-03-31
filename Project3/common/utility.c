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

void sendBluetooth(uint8_t data) {      
    while(!(UCSR1A & (1 << UDRE1)));
    UDR1 = data;
}

uint8_t receiveBluetooth() {      
    while(!(UCSR1A & (1 << RXC1)));
    return UDR1;
}