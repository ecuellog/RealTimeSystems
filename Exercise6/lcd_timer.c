#/* port_test.c
 * The following program just  keeps toggling pin 0 of port B
 */

#include <avr/io.h> //standard include for ATMega16
#include <stdlib.h>
#include <avr/interrupt.h>
#include <LiquidCrystal.h>
#define sbi(x,y) x |= _BV(y) //set bit - using bitwise OR operator 
#define cbi(x,y) x &= ~(_BV(y)) //clear bit - using bitwise AND operator
#define tbi(x,y) x ^= _BV(y) //toggle bit - using bitwise XOR operator
#define is_high(x,y) (x & _BV(y) == _BV(y)) //check if the y'th bit of register 'x' is high ... test if its AND with 1 is 1

/* _BV(a) is a macro which returns the value corresponding to 2 to the power 'a'. Thus _BV(PX3) would be 0x08 or 0b00001000 */

int msec = 0;
int sec = 0;
int min= 0;
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int main(void){
	
	lcd.begin(16, 2);  
	lcd.clear();
	//Clear timer config.
  	TCCR3A = 0;
  	TCCR3B = 0;  
  	//Set to CTC (mode 4)
  	TCCR3B |= (1<<WGM32);
  
  	//Set prescaller to 256
  	TCCR3B |= (1<<CS32);
  	
  	//Set TOP value (0.001s = 1000Hz)
  	OCR3A = 625;
  
  	//Enable interupt A for timer 3.
  	TIMSK3 |= (1<<OCIE3A);
  
  	//Set timer to 0 (optional here).
  	TCNT3 = 0;
  	//while(1);
  	sei();
  	
   // DDRB=0xff; //PORTB as OUTPUT
   // PORTB=0x00; //All pins of PORTB LOW

   	/* unsigned int i;
    while(1==1) //Infinite loop
    {
    	PORTB=0x00;
        for(i=0;i<65535;i++); //delay
        for(i=0;i<65535;i++); //delay
        //tbi(PORTB,PB7);   //here the toggling takes place
        PORTB=0xFF;
        for(i=0;i<65535;i++); //delay
        for(i=0;i<65535;i++); //delay
    }*/
    while(1);

}

ISR(TIMER3_COMPA_vect)
{	
	msec++;
	if(msec > 99){
		sec++;
		msec = 0;
		if(sec > 59){
			min++;
			sec = 0;
		}
	}
	lcd.print(min);
	if(min < 10)
		lcd.print(" ");
	lcd.print(":");
	lcd.print(sec);
	if(sec < 10)
		lcd.print(" ");
	lcd.print(":");
	lcd.print(msec);
	lcd.setCursor(0,0);
}

/*void printtime(){
	if(msec > 99){
		sec++;
		msec = 0;
		if(sec > 59){
			min++;
			sec = 0;
		}
	}
	lcd.print(min);
	lcd.print(":");
	lcd.print(sec);
	lcd.print(":");
	lcd.print(msec);
	lcd.print(":");
	lcd.setCursor(0, 1);
}*/


