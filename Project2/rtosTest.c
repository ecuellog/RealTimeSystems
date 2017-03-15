#include <avr/io.h>
#include "os.h"

#define FOSC 16000000 //Clock Speed
#define BAUD 115200
#define MYUBRR FOSC/16/BAUD-1
#define F_CPU 16000000
#define NUMLOGS 100

int trace[NUMLOGS]; 
int curTraceIndex = 0;
int curTaskNum = 1;

void USART_Init( unsigned int ubrr)
{
	/* Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;

	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void USART_Transmit(unsigned char data)
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

void logTrace(int task_number) {
	trace[curTraceIndex] = task_number;
	curTraceIndex++;
}

int assignTaskNum() {
	int taskNum = curTaskNum;
	curTaskNum++;
	return taskNum;
}

void init_LED() {
  DDRB = 0xFF;  // Set it all to output
  logTrace(Task_GetArg());
}

void enable_LED() {
  PORTB |= (1 << 7); // onboard LED
  logTrace(Task_GetArg());
}

void disable_LED() {
  PORTB &= ~(1 << 7);
  logTrace(Task_GetArg());
}

void toggle_LED() {
  PORTB ^= (1 << 7);
  logTrace(Task_GetArg());
}

void WreckTiming() {
  for (int i = 0; i < 16000; i++) {
    for (int j = 0; j < 2000; j++) {
      asm("");
    }
  }
  logTrace(Task_GetArg());
}
void createWreckTimingTask() {
  int taskNum = assignTaskNum();
  Task_Create_System(WreckTiming, taskNum);
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
	int taskNum = assignTaskNum();
    Task_Create_System(disable_LED, taskNum);
    taskNum = assignTaskNum();
    Task_Create_RR(enable_LED, taskNum);
}

/* 1: RR task will run after and disable LED */
void test_system_priority_led_OFF() {
  int taskNum = assignTaskNum();
  Task_Create_RR(disable_LED, taskNum);
  taskNum = assignTaskNum();
  Task_Create_System(enable_LED, taskNum);
}

/* 2: RR task will run after and enable LED */
void test_many_system_priority_led_ON() {
  int taskNum = assignTaskNum();
  Task_Create_System(disable_LED, taskNum);
  taskNum = assignTaskNum();
  Task_Create_System(disable_LED, taskNum);
  taskNum = assignTaskNum();
  Task_Create_System(disable_LED, taskNum);
  taskNum = assignTaskNum();
  Task_Create_RR(enable_LED, taskNum);
  taskNum = assignTaskNum();
  Task_Create_System(disable_LED, taskNum);
  taskNum = assignTaskNum();
  Task_Create_System(disable_LED, taskNum);
  taskNum = assignTaskNum();
  Task_Create_System(disable_LED, taskNum);
}

/* 3: ping and pong will alternate flashing the LED at 1Hz */
void test_basic_period() {
  int taskNum = assignTaskNum();
  Task_Create_Period(PeriodicPing, taskNum, 100, 1, 0);
  taskNum = assignTaskNum();
  Task_Create_Period(PeriodicPong, taskNum, 100, 1, 50);
}

/* 4: OS will abort, no flashing */
void test_initial_timing_violation() {
  int taskNum = assignTaskNum();
  Task_Create_Period(PeriodicPing, taskNum, 100, 1, 0);
  taskNum = assignTaskNum();
  Task_Create_Period(PeriodicPong, taskNum, 100, 1, 50);
  createWreckTimingTask();
}

/* 5: after 2.5 seconds OS will abort, no more flashing */
void test_in_progress_timing_violation() {
  int taskNum = assignTaskNum();
  Task_Create_Period(PeriodicPing, taskNum, 100, 1, 0);
  taskNum = assignTaskNum();
  Task_Create_Period(PeriodicPong, taskNum, 100, 1, 50);
  taskNum = assignTaskNum();
  Task_Create_Period(createWreckTimingTask, taskNum, 10000, 1, 250);
}

void test_transmission(){
	USART_Init(MYUBRR); //MYUBRR = 51 for current settings

		while(1){
			USART_Transmit('A');
		}
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
  case 6: test_transmission(); break;
  default: break;
  }
}
