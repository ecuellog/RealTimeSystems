#include <avr/io.h>
#include "os.h"

void init_LED() {
  DDRB = 0xFF;  // Set it all to output
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

void WreckTiming() {
  for (int i = 0; i < 16000; i++) {
    for (int j = 0; j < 2000; j++) {
      asm("");
    }
  }
}
void createWreckTimingTask() {
  Task_Create_System(WreckTiming, 0);
}

void PeriodicPing() {
  for(;;) {
    enable_LED();
    Task_Next();
  }
}

void PeriodicPong() {
  for(;;) {
    disable_LED();
    Task_Next();
  }
}

/* 0: RR task will run after and enable LED */
void test_system_priority_led_ON() {
    Task_Create_System(disable_LED, 0);
    Task_Create_RR(enable_LED, 0);
}

/* 1: RR task will run after and disable LED */
void test_system_priority_led_OFF() {
  Task_Create_RR(disable_LED, 0);
  Task_Create_System(enable_LED, 0);
}

/* 2: RR task will run after and enable LED */
void test_many_system_priority_led_ON() {
  Task_Create_System(disable_LED, 0);
  Task_Create_System(disable_LED, 0);
  Task_Create_System(disable_LED, 0);
  Task_Create_RR(enable_LED, 0);
  Task_Create_System(disable_LED, 0);
  Task_Create_System(disable_LED, 0);
  Task_Create_System(disable_LED, 0);
}

/* 3: ping and pong will alternate flashing the LED at 1Hz */
void test_basic_period() {
  Task_Create_Period(PeriodicPing, 0, 100, 1, 0);
  Task_Create_Period(PeriodicPong, 0, 100, 1, 50);
}

/* 4: OS will abort, no flashing */
void test_initial_timing_violation() {
  Task_Create_Period(PeriodicPing, 0, 100, 1, 0);
  Task_Create_Period(PeriodicPong, 0, 100, 1, 50);
  createWreckTimingTask();
}

/* 5: after 2.5 seconds OS will abort, no more flashing */
void test_in_progress_timing_violation() {
  Task_Create_Period(PeriodicPing, 0, 100, 1, 0);
  Task_Create_Period(PeriodicPong, 0, 100, 1, 50);
  Task_Create_Period(createWreckTimingTask, 0, 10000, 1, 250);
}

void a_main() {
  init_LED();

  /* use argument value for test case */
  switch(Task_GetArg()) {
  case 0: test_system_priority_led_ON(); break;
  case 1: test_system_priority_led_OFF(); break;
  case 2: test_many_system_priority_led_ON(); break;
  case 3: test_basic_period(); break;
  case 4: test_initial_timing_violation(); break;
  case 5: test_in_progress_timing_violation(); break;
  default: break;
  }
}
