#include "scheduler.h"

int ledPin = 13;

void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT);
    
  Scheduler_Init();
  Scheduler_StartTask(0, 1000, on);
  Scheduler_StartTask(500, 1000, off);
}

void loop() {
  // put your main code here, to run repeatedly:
  uint32_t idleTime = Scheduler_Dispatch();
}

void on() {
  digitalWrite(ledPin, HIGH);
}

void off() {
  digitalWrite(ledPin, LOW);
}

