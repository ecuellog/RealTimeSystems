#include <avr/io.h>
#include <util/delay.h>

#define START 128
#define SAFE_MODE 131
#define DRIVE 137

void initRoombaUart();
void initRoomba();
void driveRoomba(int16_t speed, int16_t radius);

void sendRoomba(uint8_t data);
uint8_t receiveRoomba();

void testRoomba();
