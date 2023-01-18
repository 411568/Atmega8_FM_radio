#include <avr/io.h>
#include <util/delay.h>
#include <avr/power.h>

#include "pinDefines.h"
#include "i2c.h"

#define _BV(bit)			(1 << (bit))
#define bit_is_set(sfr, bit)		(_SFR_BYTE(sfr) & _BV(bit))
#define bit_is_clear(sfr, bit)		(!(_SFR_BYTE(sfr) & _BV(bit)))

#define TEA5767_ADDRESS_W               0b11000000	 //address for writing to the TEA5767 (0xC0)


//NOTE: those are old pin defines, have to change them later
#define TLC5916_CLK	            	PB3 			 //clk port for the led driver
#define TLC5916_SDI			PB2			             //data port 
#define TLC5916_LEED1			PB1 			     //port LE(ED1) 
#define TLC5916_OEED2			PB4			         //port OE(ED2) 

#define PC847_1				PC1			             //port pierwszego transoptora
#define PC847_2				PC2		             	 //port drugiego transoptora
#define PC847_3				PC3			             //port trzeciego transoptora

#define button_1			PD0			             //port przycisku frequency++
#define button_2			PD1			             //port przycisku frequency--

#define debounce_t 		 	20



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
	i2cStart();
	i2cSend(TEA5767_ADDRESS_W);
	i2cSend(frequencyH);
	i2cSend(frequencyL);
	i2cSend(0xB0);
	i2cSend(0x10);
	i2cSend(0x00);
	i2cStop();
}


void LED_loop(char setting)
{
	for(int i = 0; i < 8; i++)
	{
		if(setting & (1 << i)) //if the bit in "setting" is set to "1" then it sends a "1" to the serial port
		{
			PORTB = 0b00010100;
			_delay_ms(2);
			PORTB = 0b00011100;
			_delay_ms(2);

		}
		else
		{
			PORTB = 0b00010000;
			_delay_ms(2);
			PORTB = 0b00011000;
			_delay_ms(2);

		}
	}
}



//led set
void LED_send(char setting, int opto)
{
	PORTC = 2 << opto; //testing this option now

	if(opto == 0) //jeszcze nie wiem co ma tu byc
	{
		LED_loop(setting);
	}
	else if(opto == 1) //jesli jestem na srodku tam gdzie sa zamienione diody
	{
		char temporary = setting;
		setting = setting >> 2;

		if(temporary && 0x02 == 0x02)
		{
			setting = setting || 0x80;
		}
		if(temporary && 0x01 == 0x01)
		{
			setting = setting || 0x40;
		}

		LED_loop(setting);
	}
	else if(opto == 2) //tu jest chyba wszystko ok
	{
		LED_loop(setting);
	}


	_delay_ms(2);
	PORTB = 0b00010010;
	_delay_ms(2);
	PORTB = 0b00000000;
	_delay_ms(2);
}


//led animation
void animateLED()
{
	for(int i = 0; i < 8; i++)
	{
		LED_send(1 << i, 1); //te bity sa na odwrot na wyswietlaczu
		_delay_ms(10);
	}
	LED_send(0b10101010, 0); //od lewej bity sa od lewej na wyswietlaczu (chyba)
	_delay_ms(100);
	LED_send(0b01000001, 2); //tu sa 3 srodkowe bity ktores XDDD
	_delay_ms(100);
	LED_send(0b11000010, 1); //tutaj 7 i 8 sa na miejscu 1 i 2 a reszta jest przesunieta
}

void setup()
{
	initI2C();
	DDRB = 0xFF; //port B w trybie wyjścia
	DDRC = 0xFF; //port C w trybie wyjścia
	DDRD = 0x00; //port D ustawiony w tryb wejścia
	PORTD = 0x3; //ustawienie oporników podciągających (tylko piny PD0 i PD1)


    //for testing only
	animateLED();

    //starting frequency from the table
	frequency = frequencytab[0];
	station_number = 0;
	send_freq();
	_delay_ms(100);
}



// -------- MAIN -------- //
int main(void)
{
	setup();

	while(1)
	{
		// ------ Zwiększenie częstotliwości ------ //
		if(bit_is_clear(PIND, button_1))
		{
			_delay_ms(debounce_t);
			if(bit_is_clear(PIND, button_1))
			{
				if(station_number < 9)
					station_number++;
				else
					station_number = 0;

				frequency = frequencytab[station_number];
				send_freq();
			}
		}


		// ------ Zmniejszanie częstotliwości ------ //
		if(bit_is_clear(PIND, button_2))
		{
			_delay_ms(debounce_t);
			if(bit_is_clear(PIND, button_2))
			{
				if(station_number > 0)
					station_number--;
				else
					station_number = 9;

				frequency = frequencytab[station_number];
				send_freq();
			}
		}
	}

	return (0);
}
