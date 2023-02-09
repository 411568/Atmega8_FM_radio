//TODO
//Fix searching for stations
//find a better way to set frequency via rotary encoder
//write the LED driver lib

#ifndef F_CPU
#define F_CPU	8000000
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#include "tea5767.c"
#include "encoder.c"


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
    DDRB = 0x00; //B pins as inputs 
    DDRD = 0xFF; //D pins as outputs
    PORTC = 0xFF; //C pins with internal pullup resistors
    PORTB = 0xFF; //C pins with internal pullup resistors
                  
    //for rotary encoder
    uint8_t val_tmp = 0;
    uint8_t val = 0;
    val_tmp = read_encoder();


	//main loop
	while(1)
	{
		// ------ ENCODER ------ //
        val_tmp = read_encoder();

        if(val != val_tmp)
        {
            if( (val == 3 && val_tmp == 1) || (val == 0 && val_tmp == 2) )
            {
                if(frequency < 108.0)
                {
                    frequency += 0.1;
                    send_freq(); 
                }
            }
            else if( (val == 2 && val_tmp == 0) || (val == 1 && val_tmp == 3) )
            {
                if(frequency > 88.0)
                {
                    frequency -= 0.1;
                    send_freq();
                }
            }
        }

        val = val_tmp;


		// ------ SEARCH UP ------ //
		if(bit_is_clear(button_1_port, button_1_pin)) 
		{
			_delay_ms(15); //debounce
			if(bit_is_clear(button_1_port, button_1_pin))
			{
                PORTD ^= (1 << PD0);
                search(1);
                _delay_ms(500);
			}
		}


		// ------ SEARCH DOWN ------ //
		if(bit_is_clear(button_2_port, button_2_pin)) 
		{
			_delay_ms(15); //debounce
			if(bit_is_clear(button_2_port, button_2_pin))
			{
                PORTD ^= (1 << PD0);
                search(0);
                _delay_ms(500);
			}
		}
	}



}

