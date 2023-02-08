#ifndef F_CPU
#define F_CPU	8000000
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#include "TEA5767.c"

#define TEA5767_ADDRESS_W               0b11000000	 //address for writing to the TEA5767 (0xC0)
//#define TEA5767_ADDRESS_W               0b01100000	 //address for writing to the TEA5767 (0xC0)
#define PRESET1	89.9
#define PRESET2	95.1
#define PRESET3	95.7
#define PRESET4	97.5
#define PRESET5	106.7

double radioFreq = PRESET5;
uint8_t rcvCmd = 0;
char msg[150];

//global variables
double frequency;
double frequencytab[] = {89.2, 91.5, 95.7, 96.4, 97.6, 99.7, 102.2, 104.5, 105, 106.7};
int station_number;
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
	station_number = 0;
	send_freq();
	_delay_ms(100);
}

int main(void)
{
	setup();

    /*
    //check if initialized properly
    //
	if(!TEA5767_Init(radioFreq))
	{
        return -1;
	}
	
    //scan up for station
    //TEA5767_ScanFreq(TEA5767_SCAN_UP);

    // MAIN LOOP
    while (1) 
    {	
        ;
    }*/
}

