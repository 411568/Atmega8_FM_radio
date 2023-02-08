#ifndef F_CPU
#define F_CPU	8000000
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#include "i2c_lib.c"



#define _BV(bit)			(1 << (bit))
#define bit_is_set(sfr, bit)		(_SFR_BYTE(sfr) & _BV(bit))
#define bit_is_clear(sfr, bit)		(!(_SFR_BYTE(sfr) & _BV(bit)))

#define TEA5767_ADDRESS_W               0b11000000	 //address for writing to the TEA5767 (0xC0)


double radioFreq = PRESET1;
uint8_t rcvCmd = 0;
char msg[150];

//global variables
double frequency;

//preset radio stations
double frequencytab[] = {89.2, 91.5, 95.7, 96.4, 97.6, 99.7, 102.2, 104.5, 105, 106.7};


// send the frequency to the TEA module
void send_freq()
{
	unsigned char frequencyH;
	unsigned char frequencyL;
	unsigned int frequencyB;

	frequencyB = 4 * (frequency * 1000000 + 225000) / 32768;
	frequencyH = frequencyB >> 8;
	frequencyL = frequencyB & 0xFF;

    //data transmition 
    I2C_Start();
	I2C_Write(TEA5767_ADDRESS_W);
	I2C_Write(frequencyH);
	I2C_Write(frequencyL);
	I2C_Write(0xB0);
	I2C_Write(0x10);
	I2C_Write(0x00);
    I2C_Stop();

    _delay_ms(10);
}

void setup()
{
    I2C_Init();

    //starting frequency from the table
	frequency = 89.2;
	send_freq();

	_delay_ms(100);
}

int main(void)
{
    //setup the I2C, start TEA5767 with frequency from the table
	setup();

	//setup GPIO



	//main loop
	while(1)
	{
		// ------ SEARCH UP ------ //
		if(bit_is_clear(PIND, button_1)) //change pin
		{
			_delay_ms(50); //debounce
			if(bit_is_clear(PIND, button_1))
			{
				//search_up
			}
		}
		// ------ SEARCH DOWN ------ //
		if(bit_is_clear(PIND, button_1)) //change pin
		{
			_delay_ms(50); //debounce
			if(bit_is_clear(PIND, button_1))
			{
				//search_down
			}
		}
	}
}

