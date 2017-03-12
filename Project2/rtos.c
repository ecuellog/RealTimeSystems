#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
#include "rtosTest.h"
#include "os.h"

/**
 * A basic message passing RTOS
 *
 * Authors: Konrad Schultz, Edgardo Cuello
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
  RUNNING,
  BLOCKED,
} PROCESS_STATE;

typedef enum kernel_request_type {
  NONE = 0,
  CREATE,
  NEXT,
  TERMINATE,
} KERNEL_REQUEST_TYPE;

typedef enum priority {
  SYSTEM = 0,
  PREEMPTED,
  PERIODIC,
  ROUND_ROBIN,
} PRIORITY;

typedef struct ProcessDescriptor {
  unsigned char *sp;  /* stack pointer into the "workSpace" */
  unsigned char workSpace[WORKSPACESIZE]; 
  PROCESS_STATE state;
  voidfuncptr  code;  /* function to be executed as a task */
  KERNEL_REQUEST_TYPE request;
  int arg;
  PRIORITY priority;
  
  TICK startTime; /* only used if periodic task */
  TICK period; /* only used if periodic task */
  BOOL firstRun; /* only used if periodic task */
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

/* 1 if kernel has been started; 0 otherwise. */
volatile static unsigned int KernelActive;

/* number of tasks created so far */
volatile static unsigned int Tasks;

/* time in milliseconds - see os.h */
volatile static unsigned int now;
volatile static TICK currentTick;




/**
 * When creating a new task, it is important to initialize its stack just like
 * it has called "Enter_Kernel()"; so that when we switch to it later, we
 * can just restore its execution context on its stack.
 * (See file "cswitch.S" for details.)
 */
void Kernel_Create_Task_At(PD *pd, voidfuncptr f, int arg, PRIORITY p, TICK t0, TICK T) {
  unsigned char *sp = (unsigned char *) &(pd->workSpace[WORKSPACESIZE-1]);

  /* Clear the contents of the workspace */
  memset(&(pd->workSpace), 0, WORKSPACESIZE);

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
  *(unsigned char *)sp-- = 0x00;

  /* Place return address of function at bottom of stack */
  *(unsigned char *)sp-- = ((unsigned int)f) & 0xff;
  *(unsigned char *)sp-- = (((unsigned int)f) >> 8) & 0xff;
  *(unsigned char *)sp-- = 0x00;

  /* Place stack pointer at top of stack */
  sp = sp - 34;

  pd->sp = sp;    /* stack pointer into the "workSpace" */
  pd->code = f;   /* function to be executed as a task */
  pd->request = NONE;
  pd->state = READY;
  pd->arg = arg;
  pd->priority = p;

  if (p == PERIODIC) {
    pd->startTime = t0;
    pd->period = T;
    pd->firstRun = TRUE;
  }
}

static void Kernel_Create_Task(voidfuncptr f, int arg, PRIORITY p, TICK t0, TICK T) {
  int x;

  if (Tasks == MAXTHREADCOUNT) return;

  /* find a DEAD PD that we can use  */
  for (x = 0; x < MAXTHREADCOUNT; x++) {
     if (Process[x].state == DEAD) break;
  }

  ++Tasks;
  Kernel_Create_Task_At(&(Process[x]), f, arg, p, t0, T);
}

BOOL find_next_task(PRIORITY p, PROCESS_STATE next) {
  /* Find the next READY SYSTEM task */
  for (int i = 0; i < MAXTHREADCOUNT; i++) {
    int j = (CurrentProcessIndex + i + 1) % MAXTHREADCOUNT;
    if (Process[j].state == READY && Process[j].priority == p) {
      CurrentPD->state = next;
      CurrentPD = &(Process[j]);
      CurrentPD->state = RUNNING;
      CurrentProcessIndex = j;
      return TRUE;
    }
  }

  return FALSE;
}

volatile unsigned int countTest = 0;
BOOL find_next_periodic_task(PROCESS_STATE next) {
  /* Find the next READY periodic task */
  for (int i = 0; i < MAXTHREADCOUNT; i++) {
    if (Process[i].state == READY && Process[i].priority == PERIODIC) {
      if (Process[i].firstRun) {
        if (currentTick > Process[i].startTime) {
          OS_Abort(1);
        } 
      } else {
        if (currentTick - Process[i].startTime > Process[i].period) {
          OS_Abort(1);
        }
      }

      if (Process[i].firstRun && Process[i].startTime == currentTick) {
        CurrentPD->state = next;
        CurrentPD = &(Process[i]);
        CurrentPD->state = RUNNING;
        CurrentPD->firstRun = FALSE;
        CurrentProcessIndex = i;
        return TRUE;
      } else if (Process[i].startTime + Process[i].period == currentTick) {
        CurrentPD->state = next;
        CurrentPD = &(Process[i]);
        CurrentPD->state = RUNNING;
        CurrentPD->startTime = currentTick;
        CurrentProcessIndex = i;
        return TRUE;
      }
    }
  }

  return FALSE;
}

void Dispatch(PROCESS_STATE next) {
  if (find_next_task(SYSTEM, next)) {
    return;
  }
  if (find_next_task(PREEMPTED, next)) {
    return;
  }
  if (find_next_periodic_task(next)) {
    return;
  }
  find_next_task(ROUND_ROBIN, next);
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
  Dispatch(READY);

  while(1) {
    CurrentPD->request = NONE;

    /* activate this newly selected task */
    CurrentSp = CurrentPD->sp;
    Exit_Kernel();

    /* if this task makes a system call, it will return to here! */

    /* save the CurrentPD's stack pointer */
    CurrentPD->sp = CurrentSp;

    switch (CurrentPD->request) {
      case CREATE:
        Kernel_Create_Task(CurrentPD->code, CurrentPD->arg, CurrentPD->priority, CurrentPD->startTime, CurrentPD->period);
        break;
      case NEXT:
        /* voluntary yield of current process */
        Dispatch(READY);
        break;
      case NONE:
        /* next tick, possible for same task to be rescheduled */
        CurrentPD->state = READY;
        Dispatch(READY);
        break;
      case TERMINATE:
        /* deallocate all resources used by this task */
        Dispatch(DEAD);
        break;
      default:
        /* Shouldn't get here */
        break;
    }
  } 
}

void OS_Init() {
  Tasks = 0;
  KernelActive = 0;
  CurrentProcessIndex = 0;

  for (int x = 0; x < MAXTHREADCOUNT; x++) {
    memset(&(Process[x]), 0, sizeof(PD));
    Process[x].state = DEAD;
  }
}

void OS_Start() {   
  now = 0;
  currentTick = 0;

  if ((!KernelActive) && (Tasks > 0)) {
    Disable_Interrupt();
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
  if (KernelActive) {
    Disable_Interrupt();
    CurrentPD->request = CREATE;
    CurrentPD->code = f;
    CurrentPD->arg = arg;
    CurrentPD->priority = SYSTEM;
    Enter_Kernel();
  } else { 
    /* call the RTOS function directly */
    Kernel_Create_Task(f, arg, SYSTEM, -1, -1);
  }
  return Tasks - 1;
}

PID Task_Create_RR(voidfuncptr f, int arg) {
  if (KernelActive) {
    Disable_Interrupt();
    CurrentPD->request = CREATE;
    CurrentPD->code = f;
    CurrentPD->arg = arg;
    CurrentPD->priority = ROUND_ROBIN;
    Enter_Kernel();
  } else { 
    /* call the RTOS function directly */
    Kernel_Create_Task(f, arg, ROUND_ROBIN, -1, -1);
  }
  return Tasks - 1;
}

PID Task_Create_Period(voidfuncptr f, int arg, TICK period, TICK wcet, TICK offset) {
  if (wcet > period) {
    return -1;
  }

  if (KernelActive) {
    Disable_Interrupt();
    CurrentPD->request = CREATE;
    CurrentPD->code = f;
    CurrentPD->arg = arg;
    CurrentPD->priority = PERIODIC;
    CurrentPD->startTime = offset;
    CurrentPD->period = period;
    Enter_Kernel();
  } else { 
    /* call the RTOS function directly */
    Kernel_Create_Task(f, arg, PERIODIC, offset, period);
  }
  return Tasks - 1;
}

/**
 * The calling task gives up its share of the processor voluntarily.
 */
void Task_Next() {
  if (KernelActive) {
    Disable_Interrupt();
    CurrentPD->request = NEXT;
    Enter_Kernel();
  }
}

void Tick_Task_Next() {
if (KernelActive) {
    Disable_Interrupt();
    CurrentPD->request = NONE;
    Enter_Kernel();
  } 
}

int Task_GetArg(void) {
  return CurrentPD->arg;
}

CHAN Chan_Init() {
  /* TODO */
  return -1;
}

void Send(CHAN ch, int v) {
  /* TODO */
}

int Recv(CHAN ch) {
  /* TODO */
  return -1;
}

void Write(CHAN ch, int v) {
  /* TODO */
}

unsigned int Now() {
  return now;
}

void Task_Terminate() {
  if (KernelActive) {
    Disable_Interrupt();
    CurrentPD->request = TERMINATE;
    Enter_Kernel();
    /* never returns here! */
  }
}

/*
 * Testing functions
 */
void Ping() {
  for(;;) {
    enable_LED();
    Task_Next();
  }
}

void Pong() {
  for(;;) {
    disable_LED();
    Task_Next();
  }
}

void Loop() {
  for(;;) {
    asm("");
  }
}

/* do nothing for 2 seconds */
void WreckTiming() {
  for (int i = 0; i < 16000; i++) {
    for (int j = 0; j < 2000; j++) {
      asm("");
    }
  }
}

void setup() {
  Disable_Interrupt();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  /* compare match register 16MHz/256/2Hz */
  OCR1A = 16000;
  /* CTC mode */
  TCCR1B |= (1 << WGM12);
  /* 1 prescaler */
  TCCR1B |= (1 << CS10); 
  /* enable timer compare interrupt */
  TIMSK1 |= (1 << OCIE1A);
  Enable_Interrupt();

  init_LED();
  init_PINS();
}

int main() {
  setup();
  OS_Init();

  Task_Create_Period(Ping, 0, 100, 1, 0);

  Task_Create_Period(Pong, 0, 100, 1, 50);

  Task_Create_System(WreckTiming, 2);

  Task_Create_RR(Loop, 0);

  OS_Start();
}



/* don't rely on overflow and % 10 behaviour */
volatile unsigned int counter = 0;

/* f = 1000Hz */
/* T = 1 mS */
ISR(TIMER1_COMPA_vect) {
  now++;

  counter++;
  if (counter == MSECPERTICK) {
    counter = 0;

    currentTick++;
    Tick_Task_Next();
  }
}