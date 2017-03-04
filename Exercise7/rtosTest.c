#include <avr/io.h>
#include "rtosTest.h"

void init_LED(void) {
  DDRB = 0xFF;  // Set it all to output
}

void init_PINS(void) {
  DDRC = 0xFF; // Set it all to output
}

void enable_LED() {
  PORTB |= (1 << 7); // onboard LED
}

void disable_LED(void) {
  PORTB &= ~(1 << 7);
}

void toggle_LED(void) {
  PORTB ^= (1 << 7);
}

void kernel_ON(void) {
  PORTC |= (1 << 7);  
}

void kernel_OFF(void) {
  PORTC &= ~(1 << 7);
}

void task_ON(uint8_t taskIndex) {
  PORTC |= (1 << taskIndex);
}

void task_OFF(uint8_t taskIndex) {
  PORTC &= ~(1 << taskIndex);
}
