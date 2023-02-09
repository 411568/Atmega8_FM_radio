#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#include "i2c_lib.c"

#define TEA5767_ADDRESS_W               0b11000000	 //address for writing to the TEA5767 (0xC0)


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


void search(uint8_t direction)
{
	unsigned char frequencyH;
	unsigned char frequencyL;
	unsigned int frequencyB;
    double temp = 88.0;

    if(direction == 1)
    {
        temp = frequency + 0.1; 
    }
    else
    {
        temp = frequency - 0.1; 
    }

	frequencyB = 4 * (temp * 1000000 + 225000) / 32768;
	frequencyH = frequencyB >> 8;
	frequencyL = frequencyB & 0xFF;

    I2C_Start();
	I2C_Write(TEA5767_ADDRESS_W); //send address for writing
	I2C_Write(frequencyH |= 0b01000000); //search mode
	I2C_Write(frequencyL);
    if(direction == 1)
    {
        I2C_Write(0b11010000); //search up with mid adc 
    }
    else
    {
        I2C_Write(0b01010000); //search down with mid adc
    }
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

