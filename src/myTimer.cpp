/*

  myTimer.cpp

*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "myTimer.h"

//****************************************************
//    Timer0
//****************************************************

unsigned long int        _my_timer0_ms;
unsigned int  			_my_timer0_us;
unsigned int        _my_timer0_target;

//===========
//  private
//===========

//
// clearPSR()
//
void Timer0::clearPSR(){
  GTCCR |= (1<<PSRSYNC);
}

//============
//  public
//============

// Timer()
//
// Constructor
Timer0::Timer0() {   
  _my_timer0_ms = 0;
  _my_timer0_us = 0;
	_my_timer0_target = 0;
  

	TCCR0A = 0; // OC0A, OC0B disconnected. WGM01:WGM00 = 0
	TCCR0B = 0; // FOC0A, FOC0B = 0, WGM00 = 0, Timer stopped

}

// ~Timer()
//
// Detructor
Timer0::~Timer0() {
	TIMSK0 &=(~(1<<TOIE0)); // Disable interrupt
	Timer0::stop();					// Stop counting
}

// start()
//
//
void Timer0::start(unsigned int ms) {
  _my_timer0_ms = 0;
  _my_timer0_us = 0;
	_my_timer0_target = ms;
  Timer0::stop();
	TIMSK0 |= (1<<TOIE0);           // Enable overflow interrupt
  sei();
  TCNT0 = 0; 											// Set counter to 0
  TCCR0B = (1<<CS01) | (1<<CS00); // (62.5ns)*(64PR)*(256) = 1024us
}

//
//  done()
//
bool Timer0::done() { 
  return(_my_timer0_ms >= _my_timer0_target);
}

//
// stop()
//
void Timer0::stop(){
    TCCR0B &= 0xF8;                 // Clear CS2-CS0
}

//
// read()
//
unsigned long int Timer0::read(){
    return( _my_timer0_ms );
}

//
// MY_TIMER0_ISR()
//
#ifndef MY_TIMER0_OVF_ISR_FUNC
#define MY_TIMER0_OVF_ISR_FUNC
void MY_TIMER0_OVF_ISR(){
  _my_timer0_us += 24;
  _my_timer0_ms += (1+ int(_my_timer0_us/1000));
  _my_timer0_us = _my_timer0_us%1000;
  if(_my_timer0_ms >= _my_timer0_target && _my_timer0_target > 0)
    TCCR0B &= 0xF8;                 // Clear CS2-CS0
	
}
#endif

//
// ISR Timer 0
//

#ifndef TIMER0_OVF_ISR
#define TIMER0_OVF_ISR
ISR (TIMER0_OVF_vect) {
  MY_TIMER0_OVF_ISR();
}

#endif


