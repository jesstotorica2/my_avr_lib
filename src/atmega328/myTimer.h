/*

	myTimer.h

*/

#ifndef _MY_TIMER_H_
#define _MY_TIMER_H_

#include "mystdlib.h"

// Waveform Generation Mode (WGM)
void tmr_setWGM0( uint8_t wgm );
void tmr_setWGM1( uint8_t wgm );
void tmr_setWGM2( uint8_t wgm );

// Clock Select (CS)
void tmr_setCS0( uint8_t cs );
void tmr_setCS1( uint8_t cs );
void tmr_setCS2( uint8_t cs );

// Timer Overflow Interrupt Enable
void tmr_setTOIE0( uint8_t toie );
void tmr_setTOIE1( uint8_t toie );
void tmr_setTOIE2( uint8_t toie );

// Output Compare Match
//
//  Timer 0:
//    - COM0A -> Pin 6  (PD6)
//    - COM0B -> Pin 5  (PD5)
//  
//  Timer 1:
//    - COM1A -> Pin 9  (PB1) 
//    - COM1B -> Pin 10 (PB2)
//
//  Timer 2:
//    - COM2A -> Pin 11 (PB3)
//    - COM2B -> Pin 3  (PD3)
//
//  COMx (Modes):
//    - 0x0: Output Compare not disabled
//    - 0x1: Toggle OCx on match
//    - 0x2: Clear OCx on match
//    - 0x3: Set OCx on match
//
void tmr_setCOM0A( uint8_t com );
void tmr_setCOM0B( uint8_t com );
void tmr_setCOM1A( uint8_t com );
void tmr_setCOM1B( uint8_t com );
void tmr_setCOM2A( uint8_t com );
void tmr_setCOM2B( uint8_t com );

#endif
