// http://codinglab.blogspot.com/2008/10/i2c-on-avr-using-bit-banging.html

// Port for the I2C
#define I2C_DDR DDRD
#define I2C_PIN PIND
#define I2C_PORT PORTD

// Pins to be used in the bit banging
#define I2C_CLK 0
#define I2C_DAT 1

#define I2C_DATA_HI()\
I2C_DDR &= ~ (1 << I2C_DAT);\
I2C_PORT |= (1 << I2C_DAT);
#define I2C_DATA_LO()\
I2C_DDR |= (1 << I2C_DAT);\
I2C_PORT &= ~ (1 << I2C_DAT);

#define I2C_CLOCK_HI()\
I2C_DDR &= ~ (1 << I2C_CLK);\
I2C_PORT |= (1 << I2C_CLK);
#define I2C_CLOCK_LO()\
I2C_DDR |= (1 << I2C_CLK);\
I2C_PORT &= ~ (1 << I2C_CLK);

void I2C_WriteBit(unsigned char c)
{
    if (c > 0)
    {
        I2C_DATA_HI();
    }
    else
    {
        I2C_DATA_LO();
    }

    I2C_CLOCK_HI();
    delay(1);

    I2C_CLOCK_LO();
    delay(1);

    if (c > 0)
    {
        I2C_DATA_LO();
    }

    delay(1);
}

unsigned char I2C_ReadBit()
{
    I2C_DATA_HI();

    I2C_CLOCK_HI();
    delay(1);

    unsigned char c = I2C_PIN;

    I2C_CLOCK_LO();
    delay(1);

    return (c >> I2C_DAT) & 1;
}

// Inits bitbanging port, must be called before using the functions below
//
void I2C_Init()
{
    I2C_PORT &= ~ ((1 << I2C_DAT) | (1 << I2C_CLK));

    I2C_CLOCK_HI();
    I2C_DATA_HI();

    delay(1);
}

// Send a START Condition
//
void I2C_Start()
{
    // set both to high at the same time
    I2C_DDR &= ~ ((1 << I2C_DAT) | (1 << I2C_CLK));
    delay(1);

    I2C_DATA_LO();
    delay(1);

    I2C_CLOCK_LO();
    delay(1);
}

// Send a STOP Condition
//
void I2C_Stop()
{
    I2C_CLOCK_HI();
    delay(1);

    I2C_DATA_HI();
    delay(1);
}

// write a byte to the I2C slave device
//
unsigned char I2C_Write(unsigned char c)
{
    for (char i = 0; i < 8; i++)
    {
        I2C_WriteBit(c & 128);

        c <<= 1;
    }

    //return I2C_ReadBit();
    return 0;
}


// read a byte from the I2C slave device
//
unsigned char I2C_Read(unsigned char ack)
{
    unsigned char res = 0;

    for (char i = 0; i < 8; i++)
    {
        res <<= 1;
        res |= I2C_ReadBit();
    }

    if (ack > 0)
    {
        I2C_WriteBit(0);
    }
    else
    {
        I2C_WriteBit(1);
    }

    delay(1);

    return res;
}

