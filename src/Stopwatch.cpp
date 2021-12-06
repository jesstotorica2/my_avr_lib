/*

  Stopwatch.cpp

*/

#include "Stopwatch.h"

#include <avr/io.h>
//#include <avr/interrupt.h>
#include "myTimer.h"
#include "myInterrupt.h"

//****************************************************
//    Stopwatch
//****************************************************

unsigned long int   _my_stopwatch_ms;
unsigned int  			_my_stopwatch_us;
unsigned int        _my_stopwatch_target;

//===========
//  private
//===========

//
// clearPSR()
//
void Stopwatch::clearPSR(){
  GTCCR |= (1<<PSRSYNC);
}

//============
//  public
//============

// Timer()
//
// Constructor
Stopwatch::Stopwatch(uint8_t tmr) {   
  _my_stopwatch_ms = 0;
  _my_stopwatch_us = 0;
	_my_stopwatch_target = 0;
  tmrVal = ( tmr < 3 ) ? tmr : 2;
}

// ~Timer()
//
// Detructor
Stopwatch::~Stopwatch() {
	//Stopwatch::stop();					// Stop counting
}

//
// start()
//
// Start assumes that its timer is set in Normal mode (WGM=0) or in a
// 8-bit PWM mode. If WGM is changed to something else timer will not work accurately
void Stopwatch::start(unsigned int ms) {
  _my_stopwatch_ms = 0;
  _my_stopwatch_us = 0;
	_my_stopwatch_target = ms;
	
	if(tmrVal == 0) // Timer0
	{																		// WGM must equal 0 or 3 for this to work
		tmr_setTOIE0(1);           				// Enable overflow interrupt
		tmr_setCS0(0x3); 									// Set counter rate (62.5ns)*(64PR)*(256) = 1024us
		_my_stopwatch_us = 1024 - TCNT0; 	// Read remaining us for current count
	
	}
	else if(tmrVal == 1)
	{
		tmr_setTOIE1(1);           				// Enable overflow interrupt
		tmr_setCS1(0x3); 									// Set counter rate (62.5ns)*(64PR)*(256) = 1024us
		_my_stopwatch_us = 1024 - TCNT1; 	// Read remaining us for current count	
	}
	else if(tmrVal == 2)
	{
		tmr_setTOIE2(1);           				// Enable overflow interrupt
		tmr_setCS1(0x4); 									// Set counter rate (62.5ns)*(64PR)*(256) = 1024us
		_my_stopwatch_us = 1024 - TCNT2; 	// Read remaining us for current count	
	}	
	
	sei();
}

//
// reset()
//
// Simply resets running timer to 0. Will have no effect timer hasn't already been started
void Stopwatch::reset()
{
	_my_stopwatch_ms = 0;
}

//
//  done()
//
bool Stopwatch::done() { 
  return(_my_stopwatch_ms >= _my_stopwatch_target);
}

//
// stop()
//
//	Stops counter from running through increment calculations on each interrupt.
//	Does not disable interrupt!
void Stopwatch::stop(){
		_my_stopwatch_target = 1;
    _my_stopwatch_ms = 1;
}

//
// read()
//
unsigned long int Stopwatch::read(){
    return( _my_stopwatch_ms );
}

//
// MY_STOPWATCH_OVF_FUNC()
//
#define MY_STOPWATCH_OVF_ISR
void MY_STOPWATCH_OVF_FUNC()
{
	// Timer is past target, don't count. (target=0 is continuous mode)
	if( (_my_stopwatch_ms >= _my_stopwatch_target) && (_my_stopwatch_target > 0) );
	else
	{
		_my_stopwatch_us += 24;
		_my_stopwatch_ms += 1;
		if( _my_stopwatch_us >= 1000 )
		{
			_my_stopwatch_ms += 1;
			_my_stopwatch_us = _my_stopwatch_us%1000;	
		}
  }
}

//
// ISR Timer 0 overflow
//
/*
ISR (TIMER0_OVF_vect) {
  MY_STOPWATCH_OVF_ISR_FUNC();
}
*/



