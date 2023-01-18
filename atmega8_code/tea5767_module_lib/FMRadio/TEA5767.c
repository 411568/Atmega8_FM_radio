/*
 * TEA5767.c
 *
 * Created: 2018-05-15 오후 3:49:59
 *  Author: kiki
 */ 

#include "TEA5767.h"
#include "128A_TWI.h"

_radioStatus radioStatus;

static bool HLSI;
static uint8_t	radioTxBuffer[5];
static uint8_t	radioRxBuffer[5];

static bool TEA5767_WriteBuffer(uint8_t *buffer);
static bool TEA5767_ReadBuffer(uint8_t *buffer);
static inline int ABS(int x);

bool TEA5767_Init(float freqMHz)
{
	TWI_Init();
	
	if(!TEA5767_SetFreq(freqMHz)) return false;
	
	return true;
}

bool TEA5767_SetFreq(float freqMHz)
{
	uint16_t PLL;
	uint8_t LevelHigh, LevelLow;
	
	/* Default Configurations */
	radioTxBuffer[2] = 0x50;			/* Search Down, Level Mid, High Injection */
	radioTxBuffer[3] = 0x1E;			/* XTAL, SMUTE, HCC, SNC */
	radioTxBuffer[4] = 0x40;			/* DTC */
		
	/* Limit frequency range */
	if(freqMHz < TEA5767_MIN_FREQ)	freqMHz = TEA5767_MIN_FREQ;
	if(freqMHz > TEA5767_MAX_FREQ)	freqMHz = TEA5767_MAX_FREQ;
	
	/* +450kHz & Measure level */
	PLL = (4 * ((freqMHz + 0.45) * 1000000 + 225000) / 32768) + 0.5;		/* Rounded PLL */
	radioTxBuffer[0] = PLL >> 8;
	radioTxBuffer[1] = PLL & 0xFF;	
		
	if(!TEA5767_WriteBuffer(radioTxBuffer)) return false;	
	
	_delay_ms(10);	
	
	if(!TEA5767_GetStatus()) return false;
	LevelHigh = radioStatus.signalLevel;		/* Measure signal strength */
	
	/* -450kHz & Measure level */
	PLL = (4 * ((freqMHz - 0.45) * 1000000 + 225000) / 32768) + 0.5;		/* Rounded PLL */
	radioTxBuffer[0] = PLL >> 8;
	radioTxBuffer[1] = PLL & 0xFF;
	
	if(!TEA5767_WriteBuffer(radioTxBuffer)) return false;
	
	_delay_ms(10);	
	
	if(!TEA5767_GetStatus()) return false;
	LevelLow = radioStatus.signalLevel;			/* Measure signal strength */
	
	/* Compare signal level */
	if(LevelHigh < LevelLow)
	{
		/* Use High Injection */
		HLSI = true;
		PLL = (4 * (freqMHz * 1000000 + 225000) / 32768) + 0.5;
		radioTxBuffer[0] = PLL >> 8;
		radioTxBuffer[1] = PLL & 0xFF;
		
		if(!TEA5767_WriteBuffer(radioTxBuffer)) return false;
	}
	else
	{
		/* Use Low Injection */		
		HLSI = false;
		PLL = (4 * (freqMHz * 1000000L - 225000) / 32768) + 0.5;
		radioTxBuffer[0] = PLL >> 8;
		radioTxBuffer[1] = PLL & 0xFF;
		radioTxBuffer[2] = 0x40;			/* Search Down, Level Mid, Low Injection */		
		
		if(!TEA5767_WriteBuffer(radioTxBuffer)) return false;
	}
	
	return true;
}

bool TEA5767_ScanFreq(uint8_t updown)
{
	float currentFreq;	
	uint16_t PLL;
	uint8_t Level1, Level2;
	int8_t LevelGap;
	uint8_t IFC;
	
	/* Default Configurations */
	radioTxBuffer[2] = 0x50;			/* Search Down, Level Mid, High Injection */
	radioTxBuffer[3] = 0x1E;			/* XTAL, SMUTE, HCC, SNC */
	radioTxBuffer[4] = 0x40;			/* DTC */
	
	/* When tunning is correct, stop looping */
	while(true)
	{
		if(!TEA5767_GetStatus()) return false;
		
		currentFreq = radioStatus.freqMHZ;
				
		/* Start Search */
		if(updown == TEA5767_SCAN_UP)
		{
			currentFreq += 0.1;
			if(HLSI)
			{
				PLL = (4 * (currentFreq * 1000000 + 225000) / 32768) + 0.5;				
			}
			else
			{
				PLL = (4 * (currentFreq * 1000000 - 225000) / 32768) + 0.5;
			}
			
			radioTxBuffer[0] = (PLL >> 8) | 0x40;		/* Enable Search Flag */
			radioTxBuffer[1] = PLL & 0xFF;
			radioTxBuffer[2] |= 0x80;					/* Search Upward */
		}
		else
		{
			currentFreq -= 0.1;
			
			if(HLSI)
			{
				PLL = (4 * (currentFreq * 1000000 + 225000) / 32768) + 0.5;
			}
			else
			{
				PLL = (4 * (currentFreq * 1000000 - 225000) / 32768) + 0.5;
			}
			
			radioTxBuffer[0] = (PLL >> 8) | 0x40;		/* Enable Search Flag */
			radioTxBuffer[1] = PLL & 0xFF;
			radioTxBuffer[2] &= ~0x80;					/* Search Downward */
		}
		
		radioStatus.readyFlag = false;
		if(!TEA5767_WriteBuffer(radioTxBuffer)) return false;
								
		/* wait until ready flag is set */
		while(true)
		{
			_delay_ms(10);
			
			if(!TEA5767_GetStatus()) return false;
			
			/* Stop searching when reached band limit */
			if(radioStatus.bandLimitFlag) return false;
			
			/* Measure level first time */			
			if(radioStatus.readyFlag == true)
			{
				/* Record first level */
				Level1 = radioStatus.signalLevel;
				
				/* Toggle HLSI & Measure second level */
				radioTxBuffer[0] = radioRxBuffer[0] & 0x3F;
				radioTxBuffer[1] = radioRxBuffer[1];
				
				if(HLSI)
				{					
					radioTxBuffer[2] &= ~0x10;
					HLSI = 0;
				}
				else
				{				
					radioTxBuffer[2] |= 0x10;
					HLSI = 1;
				}
				
				if(!TEA5767_WriteBuffer(radioTxBuffer)) return false;
				
				_delay_ms(10);
				
				if(!TEA5767_GetStatus()) return false;
				
				Level2 = radioStatus.signalLevel;
				IFC = radioRxBuffer[2] & 0x7F;
				
				LevelGap = Level1 - Level2;
				
				if((ABS(LevelGap) < 2) && (IFC > 0x31) && (IFC < 0x3E))
				{
					/* Correct tunning, stop searching */
					TEA5767_SetFreq(radioStatus.freqMHZ);
					return true;
				}
				else
				{
					/* Wrong tunning, continue searching */
					break;
				}				
			}
		}		
	}	
	
	return true;
}

bool TEA5767_Mute(bool on)
{
	if(on)
	{
		radioTxBuffer[0] |= 0x80;
	}
	else
	{
		radioTxBuffer[0] &= ~0x80;
	}
	
	if(!TEA5767_WriteBuffer(radioTxBuffer)) return false;
	
	return true;
}

bool TEA5767_GetStatus()
{
	if(!TEA5767_ReadBuffer(radioRxBuffer)) return false;
	
	uint16_t PLL = ((radioRxBuffer[0] & 0x3F) << 8) | (radioRxBuffer[1]);
	
	radioStatus.readyFlag = (radioRxBuffer[0] & 0x80 ? true : false);
	radioStatus.bandLimitFlag = (radioRxBuffer[0] & 0x40 ? true : false);
	if(HLSI)
	{
		radioStatus.freqMHZ = roundf(((PLL * 32768 / 4 - 225000) / 1000000.0) * 10) / 10;
	}
	else
	{
		radioStatus.freqMHZ = roundf(((PLL * 32768 / 4 + 225000) / 1000000.0) * 10) / 10;
	}
	
	radioStatus.stereoFlag = (radioRxBuffer[2] & 0x80 ? true : false);
	radioStatus.signalLevel = (radioRxBuffer[3] >> 4) & 0x0F;
	
	return true;
}

static bool TEA5767_WriteBuffer(uint8_t *buffer)
{
	return TWI_TxBuffer(TEA5767_DEVICE_ADDR, buffer, 5);
}

static bool TEA5767_ReadBuffer(uint8_t *buffer)
{
	return TWI_RxBuffer(TEA5767_DEVICE_ADDR, buffer, 5);
}

static inline int ABS(int x)
{
	return  (((x) < 0) ? -(x) : (x) );
}