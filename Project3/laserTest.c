//Laser test
//LaserPin = 51   = PB2 ( MOSI/PCINT2 )

#include <avr/io.h> //standard include for ATMega16
#include <stdlib.h>
#include <avr/interrupt.h>
#define sbi(x,y) x |= _BV(y) //set bit - using bitwise OR operator 
#define cbi(x,y) x &= ~(_BV(y)) //clear bit - using bitwise AND operator
#define tbi(x,y) x ^= _BV(y) //toggle bit - using bitwise XOR operator

int main(){
	DDRB=0xff; //PORTB as OUTPUT
   	PORTB=0x00; //All pins of PORTB LOW
   	
   	int i;
   	while(1){
	   	for(i=0;i<65535;i++); //delay
		for(i=0;i<65535;i++);
	   	tbi(PORTB, PB2);
	   	tbi(PORTB, PB7);
   	}
   	return 0;
}
