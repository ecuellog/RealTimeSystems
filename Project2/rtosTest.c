#include <avr/io.h>
#include <string.h>
#include "os.h"

#define NUMLOGS 10
int trace[NUMLOGS] = { 0 };
int curTraceIndex = 0;

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

int expectedTrace[NUMLOGS] = { 0 };
void compare_traces() {
  for (int i = 0; i < NUMLOGS; i++) {
    if (expectedTrace[i] != trace[i]) {
      disable_LED();
      OS_Abort(0);
    }
  }
  enable_LED();
  OS_Abort(0);
}

void traceArg() {
  // identify tasks by argument
  trace[curTraceIndex++] = Task_GetArg();
  if (curTraceIndex == 10) {
    compare_traces();
  }
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

void PeriodicTrace() {
  for(;;) {
    traceArg();
    Task_Next();
  }
}

CHAN channel;

void SendON() {
  enable_LED();
  Send(channel, 0);
  disable_LED();
}

void RecvON() {
  enable_LED();
  Recv(channel);
  disable_LED();
}

void WriteON() {
  disable_LED();
  Write(channel, 0);
  enable_LED();
}

uint8_t taskToEnable = 0;
void SendArg() {
  for(;;) {
    Write(channel, taskToEnable++);
    if (taskToEnable >= 4) {
      taskToEnable = 0;
    }

    Task_Next();
  }
}

void RecvPing() {
  for(;;) {
    int value = Recv(channel);
    if (value == Task_GetArg()) {
      enable_LED();  
    }
  }
}

void RecvPong() {
  for(;;) {
    int value = Recv(channel);
    if (value == Task_GetArg()) {
      disable_LED();  
    }
  }
}

void SendRecvTrace() {
  for(;;) {
    traceArg();
    Send(channel, 0);
    Recv(channel);
  }
}

void RecvSendTrace() {
  for(;;) {
    traceArg();
    Recv(channel);
    Send(channel, 0);
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
  createWreckTimingTask();
  Task_Create_Period(PeriodicPong, 0, 100, 1, 50);
  Task_Create_Period(PeriodicPing, 0, 100, 1, 0);
}

/* 5: after 2.5 seconds OS will abort, no more flashing */
void test_in_progress_timing_violation() {
  Task_Create_Period(createWreckTimingTask, 0, 10000, 1, 200);
  Task_Create_Period(PeriodicPing, 0, 100, 1, 0);
  Task_Create_Period(PeriodicPong, 0, 100, 1, 50);
}

/* 11: blocks on send LED never disabled */
void test_block_on_send() {
  Task_Create_System(SendON, 0);
}

/* 12: blocks on recv LED never disabled */
void test_block_on_recv() {
  Task_Create_System(RecvON, 0);
}

/* 13: does not block on write LED is enabled */
void test_no_block_on_write() {
  Task_Create_System(WriteON, 0);
}

/* 14: syncronize periodic tasks with multicast */
void test_synchronize_multicast() {
  Task_Create_Period(SendArg, 0, 25, 1, 0);

  Task_Create_RR(RecvPing, 2);
  Task_Create_RR(RecvPong, 1);
  Task_Create_RR(RecvPing, 0);
  Task_Create_RR(RecvPong, 3);
}

/* 
 * COMPARE TRACE TESTS
 *   The LED should be on at after NUMLOGS traces if the comparison
 *   is successful. Otherwise LED will be off. Either way the system
 *   aborts. Tasks are identified by argument.
 */

/* 6: test that periodic tasks are scheduled properly*/
void test_period_order() {
  int e[NUMLOGS] = { 0, 1, 2, 3, 0, 1, 2, 3, 0, 1 };
  memcpy(expectedTrace, e, NUMLOGS * sizeof(int));

  Task_Create_Period(PeriodicTrace, 3, 100, 1, 60);
  Task_Create_Period(PeriodicTrace, 1, 100, 1, 20);
  Task_Create_Period(PeriodicTrace, 2, 100, 1, 40);
  Task_Create_Period(PeriodicTrace, 0, 100, 1, 0);
}

/* 7: test that round robin tasks are scheduled properly*/
void test_RR_order() {
  int e[NUMLOGS] = { 0, 1, 2, 3, 0, 1, 2, 3, 0, 1 };
  memcpy(expectedTrace, e, NUMLOGS * sizeof(int));

  Task_Create_RR(PeriodicTrace, 0);
  Task_Create_RR(PeriodicTrace, 1);
  Task_Create_RR(PeriodicTrace, 2);
  Task_Create_RR(PeriodicTrace, 3);
}

/* 8: test that system tasks are scheduled properly*/
void test_System_order() {
  int e[NUMLOGS] = { 0, 1, 2, 3, 0, 1, 2, 3, 0, 1 };
  memcpy(expectedTrace, e, NUMLOGS * sizeof(int));

  Task_Create_System(PeriodicTrace, 0);
  Task_Create_System(PeriodicTrace, 1);
  Task_Create_System(PeriodicTrace, 2);
  Task_Create_System(PeriodicTrace, 3);
}

/* 9: test that system tasks are scheduled ahead of RR */
void test_RR_starved() {
  int e[NUMLOGS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  memcpy(expectedTrace, e, NUMLOGS * sizeof(int));

  Task_Create_RR(PeriodicTrace, 1);
  Task_Create_System(PeriodicTrace, 0);
}

/* 10: test that RR tasks are scheduled behind periodic */
void test_RR_interleaved() {
  int e[NUMLOGS] = { 0, 1, 2, 3, 1, 2, 3, 1, 2, 3 };
  memcpy(expectedTrace, e, NUMLOGS * sizeof(int));

  Task_Create_RR(PeriodicTrace, 1);
  Task_Create_RR(PeriodicTrace, 2);
  Task_Create_RR(PeriodicTrace, 3);
  Task_Create_Period(PeriodicTrace, 0, 10000, 1, 0);
}

/* 15: test that two tasks can synchronize repeatedly */
void test_repeated_synchronization() {
  int e[NUMLOGS] = { 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 };
  memcpy(expectedTrace, e, NUMLOGS * sizeof(int));

  Task_Create_RR(SendRecvTrace, 0);
  Task_Create_RR(RecvSendTrace, 1);
}

/* TODO testcases */
/* test all task types at once. */
/* try to create too many tasks should be no-op. */
/* inifinite task creation as long as always below limit. */

/* PERFORMANCE TESTS */
/* context switching overhead */
/* dispatch overhead */
/* channel overhead */

void a_main() {
  init_LED();

  channel = Chan_Init();

  /* use argument value for test case */
  switch(Task_GetArg()) {
  case 0: 
    test_system_priority_led_ON(); 
    break;
  case 1: 
    test_system_priority_led_OFF(); 
    break;
  case 2: 
    test_many_system_priority_led_ON(); 
    break;
  case 3: 
    test_basic_period();
    break;
  case 4: 
    test_initial_timing_violation();
    break;
  case 5: 
    test_in_progress_timing_violation();
    break;
  case 6:
    test_period_order();
    break;
  case 7:
    test_RR_order();
    break;
  case 8:
    test_System_order();
    break;
  case 9:
    test_RR_starved();
    break;
  case 10:
    test_RR_interleaved();
    break;
  case 11:
    test_block_on_send();
    break;
  case 12:
    test_block_on_recv();
    break;
  case 13:
    test_no_block_on_write();
    break;
  case 14:
    test_synchronize_multicast();
    break;
  case 15:
    test_repeated_synchronization();
    break;
  default: break;
  }
}
