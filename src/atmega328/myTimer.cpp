/*

	myTimer.cpp

*/

#include "myTimer.h"

//***********	WGM ****************
//
// set Waveform Generation Mode
//
void tmr_setWGM0( uint8_t wgm )
{	
	// TCCR0A[1:0] = WGM0[1:0]
	TCCR0A &= ~0x3;
	TCCR0A |= (0x3 & wgm);
	// TCCR0B[3] = WGM0[2]
	TCCR0B &= ~(1<<WGM02);
	TCCR0B |= (((wgm>>2)&0x1)<<WGM02);
}

void tmr_setWGM1( uint8_t wgm )
{
	// TCCR1A[1:0] = WGM1[1:0]
	TCCR1A &= ~0x3;
	TCCR1A |= (0x3 & wgm);
	// TCCR1B[4:3] = WGM0[3:2]
	TCCR1B &= ~((1<<WGM13) | (1<<WGM12));
	TCCR1B |= (((wgm>>2)&0x3)<<WGM12);
	
}

void tmr_setWGM2( uint8_t wgm )
{
	// TCCR2A[1:0] = WGM2[1:0]
	TCCR2A &= ~0x3;
	TCCR2A |= (0x3 & wgm);
	// TCCR2B[3] = WGM2[2]
	TCCR2B &= ~(1<<WGM22);
	TCCR2B |= (((wgm>>2)&0x1)<<WGM22);

}

//***********	CS ****************
//
// set clock select
//
void tmr_setCS0( uint8_t cs )
{
	// TCCR0B[2:0] = CS0[2:0]
	TCCR0B &= ~0x7;
	TCCR0B |= (cs & 0x7);
}

void tmr_setCS1( uint8_t cs )
{
	// TCCR1B[2:0] = CS1[2:0]
	TCCR1B &= ~0x7;
	TCCR1B |= (cs & 0x7);
}

void tmr_setCS2( uint8_t cs )
{
	// TCCR2B[2:0] = CS2[2:0]
	TCCR2B &= ~0x7;
	TCCR2B |= (cs & 0x7);
}


//*********** TOIE ****************
//
// set timer interrupt overflow enable
//
void tmr_setTOIE0( uint8_t toie )
{
	//TIMSK0 = TOIE
	TIMSK0 = (TIMSK0 & ~0x1) | (toie & 0x1);
}

void tmr_setTOIE1( uint8_t toie )
{
	//TIMSK1 = TOIE
	TIMSK1 = (TIMSK1 & ~0x1) | (toie & 0x1);
}

void tmr_setTOIE2( uint8_t toie )
{
	//TIMSK2 = TOIE
	TIMSK2 = (TIMSK2 & ~0x1) | (toie & 0x1);
}



//*********** COM0x ****************
//
// Set Compare Output Match mode
//

// Timer 0
void tmr_setCOM0A( uint8_t com )
{
	TCCR0A = (TCCR0A & ~0xC0) | ((0x3&com)<<COM0A0);
}

void tmr_setCOM0B( uint8_t com )
{
	TCCR0A = (TCCR0A & ~0x30) | ((0x3&com)<<COM0B0);
}

// Timer 1
void tmr_setCOM1A( uint8_t com )
{
	TCCR1A = (TCCR1A & ~0xC0) | ((0x3&com)<<COM1A0);
}

void tmr_setCOM1B( uint8_t com )
{
	TCCR1A = (TCCR1A & ~0x30) | ((0x3&com)<<COM1B0);
}

// Timer 2
void tmr_setCOM2A( uint8_t com )
{
	TCCR2A = (TCCR2A & ~0xC0) | ((0x3&com)<<COM2A0);
}

void tmr_setCOM2B( uint8_t com )
{
	TCCR2A = (TCCR2A & ~0x30) | ((0x3&com)<<COM2B0);
}

