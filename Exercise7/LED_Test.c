#include <avr/io.h>
#include "LED_Test.h"
/**
 * \file LED_Test.c
 * \brief Small set of test functions for controlling LEDs on a AT90USBKey
 * 
 * \mainpage Simple set of functions to control the state of the onboard
 *  LEDs on the AT90USBKey. 
 *
 * \author Alexander M. Hoole
 * \date October 2006
 */

void init_LED(void)
{
	DDRB = 0xFF;  // Set it all to output
}

void enable_LED()
{
	PORTB |= (1 << 7); // onboard LED
}

void disable_LED(void)
{
	PORTB &= ~(1 << 7);
}

void toggle_LED(void)
{
    PORTB ^= (1 << 7);
}
