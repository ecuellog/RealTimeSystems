#include "roombaMotion.h"

void initRoomba() {
  // Pin 25
  DDRA |= (1 << PA3);
  PORTA |= (1 << PA3);
  
  PORTA &= ~(1 << PA3);
  
  PORTA |= (1 << PA3);
  
  // start the OI
  sendRoomba(START);

  _delay_ms(2100);

  PORTA |= (1 << PA3);
  PORTA &= ~(1 << PA3);
  _delay_ms(100);

  PORTA |= (1 << PA3);
  PORTA &= ~(1 << PA3);
  _delay_ms(100);

  PORTA |= (1 << PA3);
  PORTA &= ~(1 << PA3);
  _delay_ms(100);

  sendRoomba(SAFE_MODE);  
}

void driveRoomba(int16_t speed, int16_t radius) {    
  sendRoomba(DRIVE);
  sendRoomba(speed >> 8);
  sendRoomba(speed);
  sendRoomba(radius >> 8);
  sendRoomba(radius);   
}

void initRoombaUart() {   
  // Set baud rate to 19.2k
  UBRR3 = 0x33;
  // Enable receiver, transmitter
  UCSR3B = (1<<RXEN3) | (1<<TXEN3);
  // 8-bit data
  UCSR3C = ((1<<UCSZ31)|(1<<UCSZ30));
  // disable 2x speed
  UCSR3A &= ~(1<<U2X3);
}

void sendRoomba(uint8_t data) {      
  while(!(UCSR3A & (1 << UDRE3)));
  UDR3 = data;
}

uint8_t receiveRoomba() {      
  while(!(UCSR3A & (1 << RXC3)));
  return UDR3;
}

void testRoomba() {
  for(;;) {
    driveRoomba(75, 32768);
    _delay_ms(5000);
    driveRoomba(-75, 32768);
    _delay_ms(5000);
  }
}
