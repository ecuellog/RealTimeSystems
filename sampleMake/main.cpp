#include <avr/io.h>
#include <Arduino.h>
#include <LiquidCrystal.h>


#define shift_light_up(port) port << 1
#define shift_light_down(port) port >> 1

#define register_set_output(register) register = 0b11111111
#define enable_all(port) port = 0b11111111
#define disable_all(port) port = 0b00000000

LiquidCrystal lcd(8, 11, 9, 4, 5, 6, 7);

int main (void) {
    // register_set_output(DDRB);  // Set it all to output
    // PORTB |= 1 << 7; // onboard LED

    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);

    lcd.begin(16, 2);
    lcd.print("12345678901234567890123456789012");
}
