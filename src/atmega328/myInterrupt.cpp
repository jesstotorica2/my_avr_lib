/*

	myInterrupt.cpp

*/

#include "myInterrupt.h"

//
// UART
//
#ifndef MY_UART_RX_ISR
#define MY_UART_RX_ISR
void MY_UART_RX_FUNC(){}
#endif

ISR (USART_RX_vect) {
  _myUART_wr_buff((uint8_t)UDR0);
  MY_UART_RX_FUNC();
}

//
// Timer 0
//
#ifndef MY_TIMER0_OVF_ISR
#define MY_TIMER0_OVF_ISR
void MY_TIMER0_OVF_FUNC(){}
#endif

ISR( TIMER0_OVF_vect )
{
	_my_stopwatch_ovf_func();
	MY_TIMER0_OVF_FUNC();
}


//
// Timer 1
//
ISR( TIMER1_OVF_vect )
{
#ifdef TIMEDDATALOG_SAMPLE
	timeddatalog_tmr1_ovf();
#endif
}


//
// SPI
//
#ifndef MY_SPI_STC_ISR
#define MY_SPI_STC_ISR
void MY_SPI_STC_FUNC(){}
#endif

ISR (SPI_STC_vect) 
{
	MY_SPI_STC_FUNC();	
}
