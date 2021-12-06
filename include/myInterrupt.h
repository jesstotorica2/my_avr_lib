/*

	myInterrupt.h

*/

#include <avr/interrupt.h>

void dummy();


void MY_STOPWATCH_OVF_FUNC();


//================== SPI =================//
// - Serial Transfer Complete ISR
void MY_SPI_STC_USR_FUNC();
