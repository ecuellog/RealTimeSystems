/*
 * Project 3: Roomba Station
 * Authors: Konrad Schultz, Edgardo Cuello
 */

#include "../common/os.h"
#include "../common/utility.h"
#include "roombaUtility.h"

void servoX(int curAngle) { }

void servoY(int curAngle) { }

void laser(uint8_t laserOn) { }

void roomba_movement(int speed, int radius) { }

void checkBluetoothTask() {
  uint8_t command = receiveBluetooth();

  switch(command) {
    case SERVO:
      break;
    case ROOMBA:
      break;
    case LASER:
      break;
    }
}

/*
 * Check Roomba sensors. If a sensor is triggered, create a system task so that it 
 * overrides any roomba movement and move the roomba accordingly.
 */
void checkRoombaTask() { }

void initialize() {
  initLed();
  initLaser();
  initBluetooth();
}

/*
 * Servo test
 * PE5/OC3C -> PWM output for timer 3 = digital pin 3
 * PE4/0C3B -> PWM output for timer 3 = digital pin 2
 */
void servoInit() {
  // Setup ports and timers
  DDRE = 0xFF; // All output
  PORTE = 0;

  // Configure timer/counter3 as phase and frequency PWM mode
  TCNT3 = 0;  //set counter to 0
  TCCR3A = 0x00; 
  TCCR3B = 0x00;
  TCCR3A = (1 << COM3B1) | (1 << COM3C1) | (1 << WGM30);  //NON Inverted PWM
  TCCR3B |= (1 << WGM33) | (1 << CS41) | (1 << CS40); //PRESCALER=64 MODE 14 (FAST PWM)
  OCR3A = 5000;

  OCR3C = 375; // 90 Degrees
  OCR3B = 375; // 90 Degrees
}

void a_main() {
  // Task_Create_System(initialize, 0);
  
  // // voidfuncptr f, int arg, TICK period, TICK wcet, TICK offset
  // Task_Create_Period(checkBluetoothTask, 0, 5, 1, 0);
  // Task_Create_Period(checkRoombaTask, 0, 10, 1, 10);

  servoInit();
}
