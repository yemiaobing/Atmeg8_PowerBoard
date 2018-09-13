#include "eeprom.h"
#include <avr/interrupt.h>

void eeprom_write(unsigned int addr, unsigned char data)
{
	if (addr > EEPROM_END_ADDR)
	{
		return;
	}
	
	/* Wait for completion of previous write */
	while(EECR & (1<<EEWE));
	/* Set up address and data registers */
	EEAR = addr;
	EEDR = data;
	/* Write logical one to EEMWE */
	EECR |= (1<<EEMWE);
	/* Start eeprom write by setting EEWE */
	EECR |= (1<<EEWE);
	/* Wait for completion of previous write */
	while(EECR & (1<<EEWE));
}

unsigned char eeprom_read(unsigned int addr)
{
	uint8_t data = 0;
	
	if (addr > EEPROM_END_ADDR)
	{
		return 0;
	}
	
	/* Wait for completion of previous write */
	while(EECR & (1<<EEWE));
	/* Set up address register */
	EEAR = addr;
	/* Start eeprom read by writing EERE */
	EECR |= (1<<EERE);
	/* Return data from data register */	
	data = EEDR;
	
	return data;
}

void eeprom_write_bytes(uint16_t addr, uint8_t* write_buffer, uint16_t length)
{	
	cli();
	while (length--)
	{
		eeprom_write(addr++, *write_buffer++);
	}
	sei();
}

void eeprom_read_bytes(uint16_t addr, uint8_t* read_buffer, uint16_t length)
{	
	cli();
	while (length--)
	{
		*read_buffer++ = eeprom_read(addr++);
	}
	sei();
}