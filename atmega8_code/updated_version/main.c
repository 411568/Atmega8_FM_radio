//TODO
//Fix searching for stations
//find a better way to set frequency via rotary encoder

#ifndef F_CPU
#define F_CPU	8000000
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

//#include "tea5767.c"
#include "encoder.c"
#include "led_screen.c"


#define _BV(bit)			(1 << (bit))
#define bit_is_set(sfr, bit)		(_SFR_BYTE(sfr) & _BV(bit))
#define bit_is_clear(sfr, bit)		(!(_SFR_BYTE(sfr) & _BV(bit)))

#define button_1_pin PC0
#define button_1_port PINC

#define button_2_pin PC1
#define button_2_port PINC

#define button_3_pin PC2
#define button_3_port PINC

#define button_4_pin PC3
#define button_4_port PINC

#define button_5_pin PC4
#define button_5_port PINC


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

    //led start
    animate();

    //for station tab
    uint8_t station_number = 0;

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
                    frequency += 1.0;
                    send_freq(); 
                    display(frequency); //display the current frequency
                    search(1); //search up from the set freq
                }
            }
            else if( (val == 2 && val_tmp == 0) || (val == 1 && val_tmp == 3) )
            {
                if(frequency > 88.0)
                {
                    frequency -= 1.0;
                    send_freq();
                    display(frequency); //display the current frequency
                    search(0); //search down from the set freq
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
                search(1);
                _delay_ms(500);
                read_freq();
                display(frequency); //display the current frequency
			}
		}


		// ------ SEARCH DOWN ------ //
		if(bit_is_clear(button_2_port, button_2_pin)) 
		{
			_delay_ms(15); //debounce
			if(bit_is_clear(button_2_port, button_2_pin))
			{
                search(0);
                _delay_ms(500);
                read_freq();
                display(frequency); //display the current frequency
			}
		}


		// ------ ANIMATION ------ //
		if(bit_is_clear(button_3_port, button_3_pin)) 
		{
			_delay_ms(15); //debounce
			if(bit_is_clear(button_3_port, button_3_pin))
			{
                animate();
			}
		}


		// ------ TAB UP ------ //
		if(bit_is_clear(button_4_port, button_4_pin)) 
		{
			_delay_ms(15); //debounce
			if(bit_is_clear(button_4_port, button_4_pin))
			{
                if(station_number < 10)
                    station_number++;
                else
                    station_number = 0;
                frequency = frequencytab[station_number];
                display(frequency); //display the current frequency
                send_freq();
			}
		}


		// ------ TAB DOWN ------ //
		if(bit_is_clear(button_5_port, button_5_pin)) 
		{
			_delay_ms(15); //debounce
			if(bit_is_clear(button_5_port, button_5_pin))
			{
                if(station_number > 0)
                    station_number--;
                else
                    station_number = 10;
                frequency = frequencytab[station_number];
                display(frequency); //display the current frequency
                send_freq();
			}
		}
	}
}
