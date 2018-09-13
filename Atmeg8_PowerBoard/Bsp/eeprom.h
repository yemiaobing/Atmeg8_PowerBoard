#ifndef EEPROM_H_
#define EEPROM_H_

#include <avr/io.h>

#define EEPROM_START_ADDR	0x0000
#define EEPROM_END_ADDR		0x01FF
#define EEPROM_SIZE			512

extern void eeprom_write(unsigned int addr, unsigned char data);
extern unsigned char eeprom_read(unsigned int addr);
extern void eeprom_write_bytes(uint16_t addr, uint8_t* write_buffer, uint16_t length);
extern void eeprom_read_bytes(uint16_t addr, uint8_t* read_buffer, uint16_t length);
#endif /* EEPROM_H_ */