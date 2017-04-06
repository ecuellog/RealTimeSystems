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

void initRoomba() {
  initRoombaUart();

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

void queryRoomba() {
  sendRoomba(QUERYLIST);
  sendRoomba(2);
  sendRoomba(BUMPPACKET);
  sendRoomba(IRPACKET);
}

void moveRoomba(int rawSpeed, int rawRadius) {
  rawSpeed = -(rawSpeed - 512);
  int actRad;
  int actSpeed = rawSpeed / 2;
  
  if(rawRadius > 900) {
    actRad = -1;
    actSpeed = 50;
  } else if(rawRadius > 800) {
    actRad = -200;
  } else if(rawRadius > 700) {
    actRad = -400;
  } else if(rawRadius > 600) {
    actRad = -750;
  } else if(rawRadius > 530) {
    actRad = -1500;
  } else if(rawRadius > 490) {
    actRad = 32767;
  } else if(rawRadius > 400) {
    actRad = 1500;
  } else if(rawRadius > 300) {
    actRad = 750;
  } else if(rawRadius > 200) {
    actRad = 400;
  } else if(rawRadius > 100){
    actRad = 200;
  } else {
    actRad = 1;
    actSpeed = 50;
  }
  
  driveRoomba(actSpeed, actRad);
}

/*
 * Servo test
 * PE5/OC3C -> PWM output for timer 3 = digital pin 3
 * PE4/0C3B -> PWM output for timer 3 = digital pin 2
 */
int curAngleX;
int curAngleY;
void initServo() {
  // Setup ports and timers
  DDRE = 0xFF; // All output
  PORTE = 0;

  // Configure timer/counter3 as phase and frequency PWM mode
  TCNT3 = 0;  //set counter to 0
  TCCR3A = 0x00; 
  TCCR3B = 0x00;
  TCCR3A = (1 << COM3B1) | (1 << COM3C1) | (1 << WGM30);  //NON Inverted PWM
  TCCR3B |= (1 << WGM33) | (1 << CS41) | (1 << CS40); //PRESCALER=64 MODE 14 (FAST PWM)
  OCR3A = 2500;

  curAngleX = 188;
  curAngleY = 188;
  OCR3C = curAngleX; // 188 = 90 Degrees 125 = 0 d, 250 = 180 d
  OCR3B = curAngleY; // 90 Degrees
}

void setServoX(int joyX) {
  int angleRateX = (joyX - 513) / 103;

  if( curAngleX < 135 ) {
    curAngleX = curAngleX + 5;  
  } else if(curAngleX > 240) {
    curAngleX = curAngleX - 5;  
  } else {
    curAngleX = curAngleX + angleRateX;
  }
    
  OCR3C = curAngleX;
}

void setServoY(int joyY) {
  int angleRateY = (joyY - 513) / 103;

  if( curAngleY < 135 ) {
    curAngleY = curAngleY + 5;  
  } else if(curAngleY > 240) {
    curAngleY = curAngleY - 5;  
  } else {
    curAngleY = curAngleY + angleRateY;
  }

  OCR3B = curAngleY;
}
