#include "eeprom.h"
#include <avr/interrupt.h>

void eeprom_write(unsigned int addr, unsigned char data)
{
	cli();
	
	/* Wait for completion of previous write */
	while(EECR & (1<<EEWE))
	;
	/* Set up address and data registers */
	EEAR = addr;
	EEDR = data;
	/* Write logical one to EEMWE */
	EECR |= (1<<EEMWE);
	/* Start eeprom write by setting EEWE */
	EECR |= (1<<EEWE);
	
	sei();
}

unsigned char eeprom_read(unsigned int addr)
{
	uint8_t data = 0;
	
	cli();
	
	/* Wait for completion of previous write */
	while(EECR & (1<<EEWE));
	/* Set up address register */
	EEAR = addr;
	/* Start eeprom read by writing EERE */
	EECR |= (1<<EERE);
	/* Return data from data register */	
	data = EEDR;
	
	sei();
	
	return data;
}