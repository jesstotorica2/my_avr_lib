/*

 eeprom.h

*/

#ifndef _AVR_EEPROM_H_
#define _AVR_EEPROM_H_

#include "mystdlib.h"

namespace eeprom
{
	void		write_byte(uint8_t b, uint16_t offset);
	void 		write(uint8_t* data, uint16_t dlen, uint16_t offset);
	uint8_t		read_byte(uint16_t offset);
	void 		read(uint8_t* data, uint16_t dlen, uint16_t offset, uint16_t delim=0x800);

}

#endif
