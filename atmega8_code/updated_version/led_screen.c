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

#define OE PD6
#define LE PD5


void animate()
{
    for(double i = 88.0; i < 108.0; i = i + 1.0)
    {
        display(i);
        _delay_ms(1);
    }
    for(double i = 107.0; i > 88.0; i = i - 1.0)
    {
        display(i);
        _delay_ms(1);
    }
}


//displays the current frequency
void display(double input)
{
    uint8_t led_strip;
    
    uint8_t curr_freq = floor(input) - 88;
    
    //should never occur
    if(curr_freq < 0) 
        curr_freq = 0;
    if(curr_freq > 19)
        curr_freq = 19;

    //setting the optocouplers
    if(curr_freq < 8)
    {
        PORTD |= (1 << PD0);
        PORTD &= ~(1 << PD1);
        PORTD &= ~(1 << PD2);
    }
    else if(curr_freq < 16)
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
    uint8_t temp8 = curr_freq % 8; //data for the driver
    temp8 = 8 - temp8;
    uint8_t curr_freq_8 = 0;                                   
    if(temp8 > 0)
        curr_freq_8 = 1 << (temp8 - 1);

    PORTD &= ~(1 << LE); //LE to low
    PORTD |= (1 << OE); // OE to high - output off
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
    PORTD |= (1 << LE); //set LE high for some time
    _delay_ms(1);
    PORTD &= ~(1 << LE); //LE to low
    _delay_ms(1);
    PORTD &= ~(1 << OE); //OE to low - turn the output ON
}
