#include <avr/io.h>
#include <util/delay.h>

#define START 128
#define SAFE_MODE 131
#define DRIVE 137
#define QUERYLIST 149
#define BUMPPACKET 7
#define WALLPACKET 13

void initLaser();
void enableLaser();
void disableLaser();

void initRoombaUart();
void initRoomba();
void driveRoomba(int16_t speed, int16_t radius);

void sendRoomba(uint8_t data);
uint8_t receiveRoomba();

void queryRoomba();

void initServo();

void moveRoomba(int rawSpeed, int rawRadius);
