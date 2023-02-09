#ifndef F_CPU
#define F_CPU	8000000
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#include "tea5767.c"


#define _BV(bit)			(1 << (bit))
#define bit_is_set(sfr, bit)		(_SFR_BYTE(sfr) & _BV(bit))
#define bit_is_clear(sfr, bit)		(!(_SFR_BYTE(sfr) & _BV(bit)))

#define button_1_pin PC0
#define button_1_port PINC
#define button_2_pin PC1
#define button_2_port PINC



int main(void)
{
    //setup the I2C, start TEA5767 with frequency from the table
	setup();

	//setup GPIO
    DDRC = 0x00; //C pins as inputs 
    PORTC = 0xFF; //C pins with internal pullup resistors



	//main loop
	while(1)
	{
		// ------ SEARCH UP ------ //
		if(bit_is_clear(button_1_port, button_1_pin)) 
		{
			_delay_ms(50); //debounce
			if(bit_is_clear(button_1_port, button_1_pin))
			{
                search(1);
                _delay_ms(500);
			}
		}


		// ------ SEARCH DOWN ------ //
		if(bit_is_clear(button_2_port, button_2_pin)) 
		{
			_delay_ms(50); //debounce
			if(bit_is_clear(button_2_port, button_2_pin))
			{
                search(0);
                _delay_ms(500);
			}
		}
	}
}

