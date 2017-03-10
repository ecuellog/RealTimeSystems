#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
#include "rtosTest.h"
#include "os.h"

/**
 * A basic message passing RTOS
 *
 * Authors: Konrad Schultz, Edguardo Cuello
 * Date: 6 March 2017
 */

typedef void (*voidfuncptr) (void);

#define Disable_Interrupt() asm volatile ("cli"::)
#define Enable_Interrupt() asm volatile ("sei"::)

/* This internal kernel function is the context switching mechanism (see switch.S) */
extern void CSwitch();
extern "C" void Exit_Kernel();
extern "C" void Enter_Kernel();

void Task_Terminate(void);

typedef enum process_state { 
  DEAD = 0, 
  READY, 
  RUNNING 
} PROCESS_STATE;

typedef enum kernel_request_type {
  NONE = 0,
  CREATE,
  NEXT,
  TERMINATE
} KERNEL_REQUEST_TYPE;

typedef struct ProcessDescriptor {
  unsigned char *sp;  /* stack pointer into the "workSpace" */
  unsigned char workSpace[WORKSPACESIZE]; 
  PROCESS_STATE state;
  voidfuncptr  code;  /* function to be executed as a task */
  KERNEL_REQUEST_TYPE request;
  int arg;
} PD;

/* This table contains ALL process descriptors. It doesn't matter what */
static PD Process[MAXTHREADCOUNT];

volatile PD* CurrentPD; 

/** 
 * Since this is a "full-served" model, the kernel is executing using its own
 * stack. We can allocate a new workspace for this kernel stack, or we can
 * use the stack of the "main()" function, i.e., the initial C runtime stack.
 */
volatile unsigned char *KernelSp;

unsigned char *CurrentSp;

/* index to current task */
volatile static uint8_t CurrentProcessIndex;  

/* index to next task to run */
volatile static unsigned int NextPocessIndex;  

/* 1 if kernel has been started; 0 otherwise. */
volatile static unsigned int KernelActive;  

/* number of tasks created so far */
volatile static unsigned int Tasks;  


/**
 * When creating a new task, it is important to initialize its stack just like
 * it has called "Enter_Kernel()"; so that when we switch to it later, we
 * can just restore its execution context on its stack.
 * (See file "cswitch.S" for details.)
 */
void Kernel_Create_Task_At(PD *p, voidfuncptr f, int arg) {
  unsigned char *sp = (unsigned char *) &(p->workSpace[WORKSPACESIZE-1]);

  /* Clear the contents of the workspace */
  memset(&(p->workSpace), 0,WORKSPACESIZE);

  /** 
   * Notice that we are placing the address of the functions
   * onto the stack in reverse byte order (least significant first, followed
   * by most significant).  This is because the "return" assembly instructions 
   * (rtn and rti) pop addresses off in BIG ENDIAN (most sig. first, least sig. 
   * second), even though the AT90 is LITTLE ENDIAN machine.
   */

  /* Store terminate at the bottom of stack to protect against stack underrun. */
  *(unsigned char *)sp-- = ((unsigned int)Task_Terminate) & 0xff;
  *(unsigned char *)sp-- = (((unsigned int)Task_Terminate) >> 8) & 0xff;

  /* Place return address of function at bottom of stack */
  *(unsigned char *)sp-- = ((unsigned int)f) & 0xff;
  *(unsigned char *)sp-- = (((unsigned int)f) >> 8) & 0xff;
  *(unsigned char *)sp-- = 0x00;

  /* Place stack pointer at top of stack */
  sp = sp - 34;

  p->sp = sp;    /* stack pointer into the "workSpace" */
  p->code = f;   /* function to be executed as a task */
  p->request = NONE;
  p->state = READY;
  p->arg = arg;
}

static void Kernel_Create_Task(voidfuncptr f, int arg) {
  int x;

  if (Tasks == MAXTHREADCOUNT) return;

  /* find a DEAD PD that we can use  */
  for (x = 0; x < MAXTHREADCOUNT; x++) {
     if (Process[x].state == DEAD) break;
  }

  ++Tasks;
  Kernel_Create_Task_At(&(Process[x]), f, arg);
}

extern "C" void Dispatch() {
  /* 
   * Find the next READY task
   * Note: if there is no READY task, then this will loop forever!.
   */
  while(Process[NextPocessIndex].state != READY) {
    NextPocessIndex = (NextPocessIndex + 1) % MAXTHREADCOUNT;
  }

  CurrentPD = &(Process[NextPocessIndex]);
  CurrentPD->state = RUNNING;
  CurrentProcessIndex = NextPocessIndex;

  NextPocessIndex = (NextPocessIndex + 1) % MAXTHREADCOUNT;
}

/**
 * This internal kernel function is the "main" driving loop of this full-served
 * model architecture. Basically, on OS_Start(), the kernel repeatedly
 * requests the next user task's next system call and then invokes the
 * corresponding kernel function on its behalf.
 *
 * This is the main loop of our kernel, called by OS_Start().
 */
static void Next_Kernel_Request() {
  Dispatch();

  while(1) {
    CurrentPD->request = NONE;

    /* activate this newly selected task */
    CurrentSp = CurrentPD->sp;
    task_ON(CurrentProcessIndex);
    Exit_Kernel();

    /* if this task makes a system call, it will return to here! */

    /* save the CurrentPD's stack pointer */
    CurrentPD->sp = CurrentSp;

    switch(CurrentPD->request) {
      case CREATE:
        Kernel_Create_Task(CurrentPD->code, CurrentPD->arg);
        break;
      case NEXT:
      case NONE:
        /* NONE could be caused by a timer interrupt */
        CurrentPD->state = READY;
        Dispatch();
        break;
      case TERMINATE:
        /* deallocate all resources used by this task */
        CurrentPD->state = DEAD;
        Dispatch();
        break;
      default:
        /* Shouldn't get here */
        break;
    }
  } 
}

void OS_Init() {
  int x;

  Tasks = 0;
  KernelActive = 0;
  NextPocessIndex = 0;
  CurrentProcessIndex = 0;

  for (x = 0; x < MAXTHREADCOUNT; x++) {
    memset(&(Process[x]),0,sizeof(PD));
    Process[x].state = DEAD;
  }
}

void OS_Start() {   
  if ( (! KernelActive) && (Tasks > 0)) {
    Disable_Interrupt();
    interrupt_disable_ON();
    KernelActive = 1;
    Next_Kernel_Request();
  }
}

/*
 * RTOS  API  and Stubs
 *
 */

void OS_Abort(unsigned int error) {
  exit(error);
}

PID Task_Create_System(voidfuncptr f, int arg) {
  return 0;
}

PID Task_Create_RR(voidfuncptr f, int arg) {
  if (KernelActive) {
    Disable_Interrupt();
    interrupt_disable_ON();
    CurrentPD->request = CREATE;
    CurrentPD->code = f;
    CurrentPD->arg = arg;
    Enter_Kernel();
  } else { 
    /* call the RTOS function directly */
    Kernel_Create_Task(f, arg);
  }
  return 0;
}

PID Task_Create_Period(voidfuncptr f, int arg, TICK period, TICK wcet, TICK offset) {
  return 0;
}

/**
 * The calling task gives up its share of the processor voluntarily.
 */
void Task_Next() {
  if (KernelActive) {
    task_OFF(CurrentProcessIndex);
    Disable_Interrupt();
    interrupt_disable_ON();
    CurrentPD->request = NEXT;
    Enter_Kernel();
  }
}

int Task_GetArg(void) {
  return CurrentPD->arg;
}

CHAN Chan_Init() {
  return 0;
}

void Send(CHAN ch, int v) {
  /* do something */
}

int Recv(CHAN ch) {
  return 0;
}

void Write(CHAN ch, int v) {
  /* do something */
}

unsigned int Now() {
  return 0;
}

void Task_Terminate() {
  if (KernelActive) {
    Disable_Interrupt();
    interrupt_disable_ON();
    CurrentPD->request = TERMINATE;
    Enter_Kernel();
    /* never returns here! */
  }
}

/*
 * Testing functions
 */
void Ping() {
  int  x;
  init_LED();
  for(;;) {
    if (Task_GetArg() == 1) {
      enable_LED();
    }
  }
}

void Pong() {
  int  x;
  init_LED();
  for(;;) {
    if (Task_GetArg() == 2) {
      disable_LED();
    }
  }
}

void setup() {
  Disable_Interrupt();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  /* compare match register 16MHz/256/2Hz */
  OCR1A = 6250;
  /* CTC mode */
  TCCR1B |= (1 << WGM12);
  /* 256 prescaler */
  TCCR1B |= (1 << CS12); 
  /* enable timer compare interrupt */
  TIMSK1 |= (1 << OCIE1A);
  Enable_Interrupt();

  init_LED();
  init_PINS();
}

void loop() {
  asm("");
}

int main() {
  setup();
  OS_Init();
  Task_Create_RR(Ping, 1);
  Task_Create_RR(Pong, 2);
  OS_Start();
}

/* f = 100Hz */
/* T = 10 mS */
ISR(TIMER1_COMPA_vect) {
  Task_Next();
}
