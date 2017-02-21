#include <avr/io.h>

#define shift_light_up(port) port << 1
#define shift_light_down(port) port >> 1

#define register_set_output(register) register = 0b11111111
#define enable_all(port) port = 0b11111111
#define disable_all(port) port = 0b00000000

int main (void){
    register_set_output(DDRB);  // Set it all to output
    enable_all(PORTB);
}
