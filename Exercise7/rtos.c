#include <avr/interrupt.h>
#include <string.h>
#include "rtosTest.h"

/**
 *
 * \brief A Skeleton Implementation of an RTOS
 *
 * \author Dr. Mantis Cheng
 * \date 2 October 2006
 *
 * ChangeLog:
 *   Alexander M. Hoole, October 2006.
 *     -Rectified errors and enabled context switching.
 *     -LED Testing code added for development (remove later).
 *   Konrad Schultz, March 2017.
 *     -Updated to address atmega2560 and 17 bit address problem.
 *   
 */

#define DEBUG

typedef void (*voidfuncptr) (void);

#define WORKSPACESIZE 256
#define MAXPROCESSCOUNT 4

#define Disable_Interrupt() asm volatile ("cli"::)
#define Enable_Interrupt() asm volatile ("sei"::)


/*
 * RTOS Internal
 *
 */

/**
 * This internal kernel function is the context switching mechanism.
 * Fundamentally, the CSwitch() function saves the current task CurrentP's
 * context, selects a new running task, and then restores the new CurrentP's
 * context.
 * (See file "switch.S" for details.)
 */
extern void CSwitch();
/**
  * Exit_kernel() is used when OS_Start() or Task_Terminate() needs to 
  * switch to a new running task.
  */
extern "C" void Exit_Kernel();
extern "C" void Enter_Kernel();

void Task_Terminate(void);

/**
 *  This is the set of states that a task can be in at any given time.
 */
typedef enum process_states 
{ 
   DEAD = 0, 
   READY, 
   RUNNING 
} PROCESS_STATES;


/**
 * This is the set of kernel requests, i.e., a request code for each system call.
 */
typedef enum kernel_request_type 
{
   NONE = 0,
   CREATE,
   NEXT,
   TERMINATE
} KERNEL_REQUEST_TYPE;

/**
 * Each task is represented by a process descriptor, which contains all
 * relevant information about this task. For convenience, we also store
 * the task's stack, i.e., its workspace, in here.
 */
typedef struct ProcessDescriptor 
{
   unsigned char *sp;   /* stack pointer into the "workSpace" */
   unsigned char workSpace[WORKSPACESIZE]; 
   PROCESS_STATES state;
   voidfuncptr  code;   /* function to be executed as a task */
   KERNEL_REQUEST_TYPE request;
} PD;

/**
 * This table contains ALL process descriptors. It doesn't matter what
 * state a task is in.
 */
static PD Process[MAXPROCESSCOUNT];

/**
 * The process descriptor of the currently RUNNING task.
 */
volatile PD* CurrentP; 

/** 
 * Since this is a "full-served" model, the kernel is executing using its own
 * stack. We can allocate a new workspace for this kernel stack, or we can
 * use the stack of the "main()" function, i.e., the initial C runtime stack.
 * (Note: This and the following stack pointers are used primarily by the
 *   context switching code, i.e., CSwitch(), which is written in assembly
 *   language.)
 */         
volatile unsigned char *KernelSp;

/**
 * This is a "shadow" copy of the stack pointer of "Cp", the currently
 * running task. During context switching, we need to save and restore
 * it into the appropriate process descriptor.
 */
unsigned char *CurrentSp;

/** index to current task */
volatile static uint8_t CurrentPocessIndex;  

/** index to next task to run */
volatile static unsigned int NextPocessIndex;  

/** 1 if kernel has been started; 0 otherwise. */
volatile static unsigned int KernelActive;  

/** number of tasks created so far */
volatile static unsigned int Tasks;  


/**
 * When creating a new task, it is important to initialize its stack just like
 * it has called "Enter_Kernel()"; so that when we switch to it later, we
 * can just restore its execution context on its stack.
 * (See file "cswitch.S" for details.)
 */
void Kernel_Create_Task_At( PD *p, voidfuncptr f ) {
  unsigned char *sp;
#ifdef DEBUG
  int counter = 0;
#endif

  sp = (unsigned char *) &(p->workSpace[WORKSPACESIZE-1]);

  //Clear the contents of the workspace
  memset(&(p->workSpace),0,WORKSPACESIZE);

  /** 
   * Notice that we are placing the address of the functions
   * onto the stack in reverse byte order (least significant first, followed
   * by most significant).  This is because the "return" assembly instructions 
   * (rtn and rti) pop addresses off in BIG ENDIAN (most sig. first, least sig. 
   * second), even though the AT90 is LITTLE ENDIAN machine.
   */

  // Store terminate at the bottom of stack to protect against stack underrun.
  *(unsigned char *)sp-- = ((unsigned int)Task_Terminate) & 0xff;
  *(unsigned char *)sp-- = (((unsigned int)Task_Terminate) >> 8) & 0xff;

  // Place return address of function at bottom of stack
  *(unsigned char *)sp-- = ((unsigned int)f) & 0xff;
  *(unsigned char *)sp-- = (((unsigned int)f) >> 8) & 0xff;
  *(unsigned char *)sp-- = 0x00;

#ifdef DEBUG
  // Registers 0 -> 31, SREG, EIND
  for (counter = 0; counter < 34; counter++) {
    *(unsigned char *)sp-- = counter;
  }
#else
  // Place stack pointer at top of stack
  sp = sp - 34;
#endif

  p->sp = sp;    /* stack pointer into the "workSpace" */
  p->code = f;   /* function to be executed as a task */
  p->request = NONE;
  p->state = READY;
}


/**
 *  Create a new task
 */
static void Kernel_Create_Task( voidfuncptr f ) {
  int x;

  if (Tasks == MAXPROCESSCOUNT) return;

  /* find a DEAD PD that we can use  */
  for (x = 0; x < MAXPROCESSCOUNT; x++) {
     if (Process[x].state == DEAD) break;
  }

  ++Tasks;
  Kernel_Create_Task_At( &(Process[x]), f );
}

/**
 * This internal kernel function is a part of the "scheduler". It chooses the
 * next task to run, i.e., CurrentP.
 */
extern "C" void Dispatch() {
  /* 
   * Find the next READY task
   * Note: if there is no READY task, then this will loop forever!.
   */
  while(Process[NextPocessIndex].state != READY) {
    NextPocessIndex = (NextPocessIndex + 1) % MAXPROCESSCOUNT;
  }

  CurrentP = &(Process[NextPocessIndex]);
  CurrentP->state = RUNNING;
  CurrentPocessIndex = NextPocessIndex;

  NextPocessIndex = (NextPocessIndex + 1) % MAXPROCESSCOUNT;
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
    CurrentP->request = NONE;

    /* activate this newly selected task */
    CurrentSp = CurrentP->sp;
    kernel_OFF();
    task_ON(CurrentPocessIndex);
    Exit_Kernel();

    /* if this task makes a system call, it will return to here! */

    /* save the CurrentP's stack pointer */
    CurrentP->sp = CurrentSp;

    switch(CurrentP->request) {
      case CREATE:
        Kernel_Create_Task( CurrentP->code );
        break;
      case NEXT:
      case NONE:
        /* NONE could be caused by a timer interrupt */
        CurrentP->state = READY;
        Dispatch();
        break;
      case TERMINATE:
        /* deallocate all resources used by this task */
        CurrentP->state = DEAD;
        Dispatch();
        break;
      default:
        /* Shouldn't get here */
        break;
    }
  } 
}

/*
 * RTOS  API  and Stubs
 *
 */

/**
 * This function initializes the RTOS and must be called before any other
 * system calls.
 */
void OS_Init() {
  int x;

  Tasks = 0;
  KernelActive = 0;
  NextPocessIndex = 0;
  CurrentPocessIndex = 0;

  for (x = 0; x < MAXPROCESSCOUNT; x++) {
    memset(&(Process[x]),0,sizeof(PD));
    Process[x].state = DEAD;
  }
}


/**
 * This function starts the RTOS after creating a few tasks.
 */
void OS_Start() {   
  if ( (! KernelActive) && (Tasks > 0)) {
    Disable_Interrupt();
    KernelActive = 1;
    Next_Kernel_Request();
  }
}


/**
 * For this example, we only support cooperatively multitasking, i.e.,
 * each task gives up its share of the processor voluntarily by calling
 * Task_Next().
 */
void Task_Create( voidfuncptr f) {
  if (KernelActive) {
    Disable_Interrupt();
    CurrentP->request = CREATE;
    CurrentP->code = f;
    Enter_Kernel();
  } else { 
    /* call the RTOS function directly */
    Kernel_Create_Task( f );
  }
}

/**
 * The calling task gives up its share of the processor voluntarily.
 */
void Task_Next() {
  if (KernelActive) {
    task_OFF(CurrentPocessIndex);
    kernel_ON();
    Disable_Interrupt();
    CurrentP->request = NEXT;
    Enter_Kernel();
  }
}


/**
  * The calling task terminates itself.
  */
void Task_Terminate() {
  if (KernelActive) {
    Disable_Interrupt();
    CurrentP->request = TERMINATE;
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
    enable_LED();
  }
}

void Pong() {
  int  x;
  init_LED();
  for(;;) {
    disable_LED();
  }
}

void setup() {
  Disable_Interrupt();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  // compare match register 16MHz/256/2Hz
  OCR1A = 625;
  // CTC mode
  TCCR1B |= (1 << WGM12);
  // 256 prescaler
  TCCR1B |= (1 << CS12); 
  // enable timer compare interrupt
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
  Task_Create( Ping );
  Task_Create( Pong );
  OS_Start();
}

// f = 100Hz
// T = 10 mS
ISR(TIMER1_COMPA_vect) {
  Task_Next();
}
