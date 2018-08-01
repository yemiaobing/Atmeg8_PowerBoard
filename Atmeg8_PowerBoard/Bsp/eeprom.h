#ifndef EEPROM_H_
#define EEPROM_H_

#include <avr/io.h>

extern void eeprom_write(unsigned int addr, unsigned char data);
extern unsigned char eeprom_read(unsigned int addr);
#endif /* EEPROM_H_ */