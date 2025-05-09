/*

	myIO.cpp

*/

#include "myIO.h"

//
// setPUD()
//
void setPUD(int val)
{
	if( val ) val = 1;
	MCUCR &= ( ~(1 << PUD) );
	MCUCR |= ( val << PUD );
}

//
//	setOutput()
//
//	set pin as output
void setOutput(int pin) {
	if 	  	( pin >= 0  && pin < 8  ) DDRD |= (1<<pin);
  	else if ( pin >= 8  && pin < 14 ) DDRB |= (1<<(pin%8)); 
	else if ( pin >= 14 && pin < 20 ) DDRC |= (1<<(pin%14));
}


//
//	setInput()
//
//	set pin as input
void setInput(int pin) {
	if 	  	( pin >= 0  && pin < 8  ) DDRD &= ~(1<<pin);
  	else if ( pin >= 8  && pin < 14 ) DDRB &= ~(1<<(pin%8)); 
	else if ( pin >= 14 && pin < 20 ) DDRC &= ~(1<<(pin%14));
}


//
//	setPin()
//
//	set pin digital value (0 or 1)
void setPin(int pin, int val) {
	if( val != 0 ) val = 1;

	if 	 	( pin >= 0  && pin < 8  ) PORTD = (PORTD & (~(1<<pin))) 		| (val<<pin);
  	else if ( pin >= 8  && pin < 14 ) PORTB = (PORTB & (~(1<<(pin%8)))) 	| (val<<(pin%8)); 
	else if ( pin >= 14 && pin < 20 ) PORTC = (PORTC & (~(1<<(pin%14)))) 	| (val<<(pin%14));
}


//
//	getPin()
//
//	get pin digital value (0 or 1)
int getPin(int pin) {
	if 		( pin >= 0  && pin < 8  ) return( (PIND>>pin)&0x1 );
  	else if ( pin >= 8  && pin < 14 ) return( (PINB>>(pin%8))&0x1 ); 
	else if ( pin >= 14 && pin < 20 ) return( (PINC>>(pin%14))&0x1 );
	else							  return -1;
}


//
// setOpenDrainPin()
//
void setOpenDrainPin(int pin, int val, bool pullup)
{
	if( val == 0 ) 
	{
		setPin(pin, 0); 
		setOutput(pin);
	}
	else
	{
		setInput(pin);
		setPin(pin, 1);
	}
}

//
// getPinDir()
//
//
int getPinDir(int pin)
{
	if 		( pin >= 0  && pin < 8  ) return( (DDRD>>pin)&0x1 );
  	else if ( pin >= 8  && pin < 14 ) return( (DDRB>>(pin%8))&0x1  ); 
	else if ( pin >= 14 && pin < 20 ) return( (DDRC>>(pin%14))&0x1 );
	else	return -1;
}

