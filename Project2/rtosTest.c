#include <avr/io.h>
#include "rtosTest.h"

void init_LED() {
  DDRB = 0xFF;  // Set it all to output
}

void init_PINS() {
  DDRC = 0xFF; // Set it all to output
}

void enable_LED() {
  PORTB |= (1 << 7); // onboard LED
}

void disable_LED() {
  PORTB &= ~(1 << 7);
}

void toggle_LED() {
  PORTB ^= (1 << 7);
}

// gets turned off in assembly
void interrupt_disable_ON() {
  PORTC |= (1 << 6); // 0x40
}

/* 
 * Up to 5 tasks (0-5) 
 * Will need to do differently with 16 tasks
 */
void task_ON(uint8_t taskIndex) {
  PORTC |= (1 << taskIndex);
}

void task_OFF(uint8_t taskIndex) {
  PORTC &= ~(1 << taskIndex);
}
