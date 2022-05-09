/*

	myInterrupt.h

*/

#include <avr/interrupt.h>
#include "myUART.h"
#include "Stopwatch.h"

void dummy();

//================== UART =================//

// UART RX - recieved byte interrupt
void MY_UART_RX_FUNC();

//================== Timers =================//

// - Timer0 ISR
void MY_TIMER0_OVF_FUNC();

// Timer1 Overflow ISR
void MY_TIMER1_OVF_FUNC();

//================== SPI =================//
// - Serial Transfer Complete ISR
void MY_SPI_STC_FUNC();
