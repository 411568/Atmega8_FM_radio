/*
 * FMRadio.c
 *
 * Created: 2018-05-15 오후 3:33:13
 * Author : kiki
 */ 

#ifndef F_CPU
#define F_CPU	8000000L
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#include "TEA5767.h"
#include "128A_USART.h"

#define PRESET1	89.9
#define PRESET2	95.1
#define PRESET3	95.7
#define PRESET4	97.5
#define PRESET5	101.5

double radioFreq = PRESET1;
uint8_t rcvCmd = 0;
char msg[150];

int main(void)
{
	USART0_Init();
	if(!TEA5767_Init(radioFreq))
	{
		memset(msg, 0, sizeof(msg));
		sprintf(msg, "TEA5767_Init failed.\r\n");
		USART0_TxBuffer((uint8_t *)msg, strlen(msg));
		return -1;
	}
	
    /* Replace with your application code */
    while (1) 
    {	
		if(USART0_RxByte(&rcvCmd))
		{
			if(rcvCmd == '+')
			{
				if(!TEA5767_ScanFreq(TEA5767_SCAN_UP))
				{
					memset(msg, 0, sizeof(msg));
					sprintf(msg, "TEA5767_ScanFreq(UP) failed.\r\n");
					USART0_TxBuffer((uint8_t *)msg, strlen(msg));
				}
			}
			else if(rcvCmd == '-')
			{
				if(!TEA5767_ScanFreq(TEA5767_SCAN_DOWN))
				{
					memset(msg, 0, sizeof(msg));
					sprintf(msg, "TEA5767_ScanFreq(DOWN) failed.\r\n");
					USART0_TxBuffer((uint8_t *)msg, strlen(msg));
				}
			}
			else if(rcvCmd == '=')
			{
				if(TEA5767_GetStatus())
				{
					memset(msg, 0, sizeof(msg));
					sprintf(msg, "Freq: [%05.1f], Ready Flag: [%d], Signal Level: [%d], Stereo Flag: [%d], Mute Flag: [%d], Bandlimit Flag: [%d]\r\n", 
					radioStatus.freqMHZ, radioStatus.readyFlag, radioStatus.signalLevel, radioStatus.stereoFlag, radioStatus.muteFlag, radioStatus.bandLimitFlag);
					USART0_TxBuffer((uint8_t *)msg, strlen(msg));
				}
			}
			else if(rcvCmd == '1')
			{
				if(!TEA5767_SetFreq(PRESET1))
				{
					memset(msg, 0, sizeof(msg));
					sprintf(msg, "TEA5767_SetFreq(PRESET1) failed.\r\n");
					USART0_TxBuffer((uint8_t *)msg, strlen(msg));
				}
			}
			else if(rcvCmd == '2')
			{
				if(!TEA5767_SetFreq(PRESET2))
				{
					memset(msg, 0, sizeof(msg));
					sprintf(msg, "TEA5767_SetFreq(PRESET2) failed.\r\n");
					USART0_TxBuffer((uint8_t *)msg, strlen(msg));
				}
			}
			else if(rcvCmd == '3')
			{
				if(!TEA5767_SetFreq(PRESET3))
				{
					memset(msg, 0, sizeof(msg));
					sprintf(msg, "TEA5767_SetFreq(PRESET3) failed.\r\n");
					USART0_TxBuffer((uint8_t *)msg, strlen(msg));
				}
			}
			else if(rcvCmd == '4')
			{
				if(!TEA5767_SetFreq(PRESET4))
				{
					memset(msg, 0, sizeof(msg));
					sprintf(msg, "TEA5767_SetFreq(PRESET4) failed.\r\n");
					USART0_TxBuffer((uint8_t *)msg, strlen(msg));
				}
			}
			else if(rcvCmd == '5')
			{
				if(!TEA5767_SetFreq(PRESET5))
				{
					memset(msg, 0, sizeof(msg));
					sprintf(msg, "TEA5767_SetFreq(PRESET5) failed.\r\n");
					USART0_TxBuffer((uint8_t *)msg, strlen(msg));
				}
			}
		}
    }
}

