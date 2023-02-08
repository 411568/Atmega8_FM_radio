#ifndef TEA5767_H_
#define TEA5767_H_

#ifndef F_CPU
#define F_CPU	8000000
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#define TEA5767_DEVICE_ADDR		0x60
#define TEA5767_SCAN_UP			1		
#define TEA5767_SCAN_DOWN		0
#define TEA5767_MIN_FREQ		76.0
#define TEA5767_MAX_FREQ		108.0

typedef struct
{
	float	freqMHZ;
	bool	muteFlag;
	bool	readyFlag;
	bool	bandLimitFlag;
	bool	stereoFlag;
	uint8_t signalLevel;	
} _radioStatus;

extern _radioStatus radioStatus;

bool TEA5767_Init(float freqMHz);
bool TEA5767_SetFreq(float freqMHz);
bool TEA5767_ScanFreq(uint8_t updown);
bool TEA5767_Mute(bool on);
bool TEA5767_GetStatus();

#endif /* TEA5767_H_ */
