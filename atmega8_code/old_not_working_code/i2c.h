// Funkcje obsługujące komunikację i2c
#include <avr/io.h>
#include "pinDefines.h"

void initI2C(void);
                  /* Włącza oporniki podciągające i ustala szybkość magistrali 
	                                                na 100kHz (przy FCPU=8MHz) */

void i2cWaitForComplete(void);
                                      /* Czeka aż sprzęt włączy znacznik TWINT */

void i2cStart(void);
                                   /* Wysyła sygnał startu (ustawia bit TWSTA) */
void i2cStop(void);
                                    /* Wysyła sygnał stopu (ustawia bit TWSTO) */

void i2cSend(uint8_t data);
                   /* Ładuje dane, wysyła je i czeka na zakończenie transmisji */

uint8_t i2cReadAck(void);
                                     /* Odbiera dane od urządzenia podległego, 
			                         wysyłając na końcu ACK (ustawia bit TWEA) */
uint8_t i2cReadNoAck(void);
                                     /* Odbiera dane od urządzenia podległego, 
			                   wysyłając na końcu NACK (nie ustawia bitu TWEA) */
