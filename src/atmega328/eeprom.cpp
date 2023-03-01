/*

	eeprom.cpp

*/

#include "eeprom.h"

//
// write_byte()
//
void eeprom::write_byte(uint8_t b, uint16_t offset)
{
	if( (EECR & ((1<<EEPM1) | (1<<EEPM0))) != 0 )
	{
		EECR = EECR & ( ~((1<<EEPM1) | (1<<EEPM0)) );
	}
	// Wait for completion of previous write 
	while(EECR & (1<<EEPE));
	// Set up address and Data Registers 
	EEAR = offset;
	EEDR = b;
	
	// Disable interrupts
	uint8_t sreg_prev = SREG;
	SREG = sreg_prev & 0x7F; // Clear global interrupts
	// Write logical one to EEMPE
	EECR |= (1<<EEMPE);
	// Start eeprom write by setting EEPE
	EECR |= (1<<EEPE);
	// re-enable interrupts (if they were already enabled)
	SREG |= sreg_prev;
}

//
// write()
//
void eeprom::write(uint8_t* data, uint16_t dlen, uint16_t offset)
{
	uint16_t idx;
	for( idx = 0; idx < dlen; idx++ )
	{
		write_byte(data[idx], offset+idx);
	}
}

//
// read_byte()
// 
uint8_t eeprom::read_byte(uint16_t offset)
{
	// Wait for completion of previous write
	while(EECR & (1<<EEPE));
	// Set up address register
	EEAR = offset;
	// Start eeprom read by writing EERE 
	EECR |= (1<<EERE);
	// Return data from Data Register 
	return EEDR;
}

//
// read()
//
void eeprom::read(uint8_t* data, uint16_t dlen, uint16_t offset, uint16_t delim)
{
	uint16_t idx;
	if( delim != 0x800 )
	{
		uint8_t c = (0xFF & delim);
		if( dlen > 0 ) data[0] = read_byte(offset);
		for( idx = 1; idx < dlen && data[idx] != c; idx++ )
		{
			data[idx] = read_byte(offset+idx);
		}
	}
	else
	{
		for( idx = 0; idx < dlen; idx++ )
		{
			data[idx] = read_byte(offset+idx);
		}
	}
}
