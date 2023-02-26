#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "tea5767.c"

#define _BV(bit)			(1 << (bit))
#define bit_is_set(sfr, bit)		(_SFR_BYTE(sfr) & _BV(bit))
#define bit_is_clear(sfr, bit)		(!(_SFR_BYTE(sfr) & _BV(bit)))

#define encoder_port PINB
#define A_pin PB0
#define B_pin PB1



//displays the current frequency
void display()
{
    uint8_t led_strip;
    
    uint8_t curr_freq = floor(frequency) - 88;
    
    //should never occur
    if(curr_freq < 0) 
        curr_freq = 0;
    if(curr_freq > 19)
        curr_freq = 19;

    //setting the optocouplers
    if(curr_freq < 6)
    {
        PORTD |= (1 << PD0);
        PORTD &= ~(1 << PD1);
        PORTD &= ~(1 << PD2);
    }
    else if(curr_freq < 12)
    {
        PORTD &= ~(1 << PD0);
        PORTD |= (1 << PD1);
        PORTD &= ~(1 << PD2);
    }
    else
    {
        PORTD &= ~(1 << PD0);
        PORTD &= ~(1 << PD1);
        PORTD |= (1 << PD2);
    }

    //setting the led driver
    uint8_t curr_freq_8 = curr_freq % 8; //data for the driver

    PORTD &= ~(1 << PD6); //LE to low
    PORTD |= (1 << PD5); // OE to high - output off
    _delay_ms(1);

	for(int i = 0; i < 8; i++)
	{
		if(curr_freq_8 & (1 << i)) 
		{
            PORTD |= (1 << PD3); //set SDI high
            _delay_ms(1);
            PORTD |= (1 << PD4); //set clock high
            _delay_ms(1);
        }
        else
        {
            PORTD &= ~(1 << PD3); //set SDI low
            _delay_ms(1);
            PORTD |= (1 << PD4); //set clock high
            _delay_ms(1);
        }
        PORTD &= ~(1 << PD4); //set clock low
    }

    _delay_ms(1);
    PORTD |= (1 << PD6); //set LE high for some time
    _delay_ms(1);
    PORTD &= ~(1 << PD6); //LE to low
    _delay_ms(1);
    PORTD &= ~(1 << PD5); //OE to low - turn the output ON
}
