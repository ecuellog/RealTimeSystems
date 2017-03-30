//Servo test
// PE5/OC3C -> PWM output for timer 3 = digital pin 3
// PE4/0C3B -> PWM output for timer 3 = digital pin 2
//

void servoInit() {
	// Setup ports and timers
    DDRE = 0xFF; // All output
    PORTE = 0;

    // Configure timer/counter3 as phase and frequency PWM mode
    TCNT3 = 0;  //set counter to 0
    TCCR3A = 0x00; 
    TCCR3B = 0x00;
    TCCR3A = (1<<COM3B1) | (1<<COM3C1) | (1<<WGM30);  //NON Inverted PWM
    TCCR3B |= (1<<WGM33) | (1<<CS41) | (1<<CS40); //PRESCALER=64 MODE 14 (FAST PWM)
    OCR3A = 5000;

    OCR3C = 375; // 90 Degrees
    OCR3B = 375; // 90 Degrees
}

void main(){
	servoInit();
}
