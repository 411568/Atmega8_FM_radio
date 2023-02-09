#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#define _BV(bit)			(1 << (bit))
#define bit_is_set(sfr, bit)		(_SFR_BYTE(sfr) & _BV(bit))
#define bit_is_clear(sfr, bit)		(!(_SFR_BYTE(sfr) & _BV(bit)))

#define encoder_port PINB
#define A_pin PB0
#define B_pin PB1

uint8_t read_encoder()
{
    uint8_t val = 0;

    if(!bit_is_clear(encoder_port, A_pin))
    {
        val |= (1<<1);
    }

    if(!bit_is_clear(encoder_port, B_pin))
    {
        val |= (1<<0);
    }

    return val;
}

