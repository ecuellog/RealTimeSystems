/*Laser test
  LaserPin = Digital Pin 51 = PB2 ( MOSI/PCINT2 )
*/
#include <avr/io.h> //standard include for ATMega16
#include <stdlib.h>
#include <avr/interrupt.h>
#define sbi(x,y) x |= _BV(y) //set bit - using bitwise OR operator 
#define cbi(x,y) x &= ~(_BV(y)) //clear bit - using bitwise AND operator
#define tbi(x,y) x ^= _BV(y) //toggle bit - using bitwise XOR operator
#define is_high(x,y) (x & _BV(y) == _BV(y)) //check if the y'th bit of register 'x' is high ... test if its AND with 1 is 1

/* _BV(a) is a macro which returns the value corresponding to 2 to the power 'a'. Thus _BV(PX3) would be 0x08 or 0b00001000 */

int main(){
  	
   DDRB=0xFF; //PORTB as OUTPUT
   PORTB=0x00; //All pins of PORTB LOW

   	unsigned int i;
    while(1==1) //Infinite loop
    {
        for(i=0;i<65535;i++); //delay
        for(i=0;i<65535;i++); //delay
        for(i=0;i<65535;i++); //delay
        for(i=0;i<65535;i++); //delay
        tbi(PORTB,PB7);   	//toggle built in LED
        tbi(PORTB,PB2);		//toggle laser
        
    }
	return 0;
}
