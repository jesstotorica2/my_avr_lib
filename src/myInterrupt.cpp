/*

	myInterrupt.cpp

*/

#include "myInterrupt.h"


#ifndef MY_STOPWATCH_OVF_ISR
#define MY_STOPWATCH_OVF_ISR
//void MY_STOPWATCH_OVF_FUNC(){}
#endif

ISR( TIMER0_OVF_vect )
{
	MY_STOPWATCH_OVF_FUNC();
	//MY_TIMER0_USR_ISR();
}


#ifndef MY_SPI_STC_USR_ISR
#define MY_SPI_STC_USR_ISR
//void MY_SPI_STC_USR_FUNC(){}
#endif

ISR (SPI_STC_vect) 
{
	MY_SPI_STC_USR_FUNC();	
}
void dummy(){}
