/*
 * Project 3: Roomba Station
 * Authors: Konrad Schultz, Edgardo Cuello
 */
 
#include <Servo.h>
#include "Roomba_Driver.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "os.h"

void servoX(int curAngle){

}

void servoY(int curAngle){

}

void laser(bool laserOn){

}

void roomba_movement(int speed, int radius){

}

void check_bluetooth(){
	switch(command) {
		// servoX
		case 0:
		  //servoX();
		  break;
	   
		// servoY
		case 1:  
		  servoY();
		  break;
		
		// laser
		case 2:
		 //laser();
		  break;
		
		//Roomba movements
		case 3:
		  // roomba_movement();
		  break;
	  }
}

void checkSensors(){
/*
	Check Roomba sensors. If a sensor is triggered, create a system task so that it 
	overrides any roomba movement and move the roomba accordingly.
*/
}

void a_main(){
 //voidfuncptr f, int arg, TICK period, TICK wcet, TICK offset  --- for reference
	Task_Create_Period(checkBluetooth, 0, 5, 1, 0);
	Task_Create_Period(checkSensors, 0, 10, 1, 10);
}


