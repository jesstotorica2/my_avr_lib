/*

	myADC.cpp

*/

#include "myADC.h"
#include <util/delay.h> //DEBUG

//=================================================
//
//	Public
//
//=================================================

//
// Constructor
//
myADC::myADC()
{
	set_ref(MYADC_DEFAULT_REF);
	set_clkdiv(MYADC_DEFAULT_CLK);
}

//
// Destructor
//
myADC::~myADC()
{

}

//
// set_sigmux()
//
void myADC::set_sigmux(int sig_in)
{
	ADMUX = (ADMUX & ~0xF) | (sig_in & 0xF);
}

//
// set_ref()
//
void myADC::set_ref(int ref)
{
	ADMUX = (ADMUX & ~(0x3<<6)) | ((ref & 0x3) << 6);
}

//
// set_lar()
//
void myADC::set_lar(bool set)
{
	ADMUX = (set) ? (ADMUX | (0x1<<5)) : (ADMUX & ~(0x1<<5));
}

//
// enable()
//
void myADC::enable(bool en)
{
	ADCSRA = (en) ? (ADCSRA | (0x1<<7)) : (ADCSRA & ~(0x1<<7));
}

//
// start_conversion()
//
void myADC::start_conversion()
{
	if( ADCSRA & (1<<7) )
	ADCSRA = (ADCSRA | (0x1<<6));
}

//
// conversion_wait()
//
void myADC::conversion_wait()
{
	if( ADCSRA & (1<<7) ) // Ensure ADC is enabled so we dont get stuck
	{
		while( (ADCSRA & (0x1 << 4)) == 0 ); // While ADIF == 0
	}
}

//
// set_clkdiv()
//
void myADC::set_clkdiv(int clkdiv)
{
	ADCSRA = (ADCSRA & ~0x7) | (clkdiv & 0x7);
}

//
// set_auto_trigger()
//
void myADC::set_auto_trigger(int ats, bool at_enable)
{
	ADCSRA = (at_enable) ? (ADCSRA | (0x1<<5)) : (ADCSRA & ~(0x1<<5));
	
	ADCSRB = (ADCSRB & ~0x7) | (ats & 0x7);
}

//
// disable_din()
//
void myADC::disable_din(uint8_t mask)
{
	DIDR0 = (0x1F & mask);
}

//
// read()
//
uint16_t myADC::read()
{
	uint16_t result = (ADCL & 0xFF);
	result |= ((ADCH & 0xFF) << 8);
	return result;
}

//
// sample()
//
uint16_t myADC::sample()
{
	start_conversion();
	conversion_wait();
	return( read() );
}

