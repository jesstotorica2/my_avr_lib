/*

	myADC.h

*/

#include <stdlib.h>
#include <avr/io.h>

// Signal Select
#define MYADC_ADC0 0
#define MYADC_ADC1 1
#define MYADC_ADC2 2
#define MYADC_ADC3 3
#define MYADC_ADC4 4
#define MYADC_ADC5 5
#define MYADC_ADC6 6
#define MYADC_ADC7 7
#define MYADC_ADC8 8
#define MYADC_TEMP MYADC_ADC8

#define MYADC_1_1V 14
#define MYADC_GND  15

// References
#define MYADC_AREF 		0 // AREF, Internal Vref turned off
#define MYADC_AVCC 		1 // AVCC with external capacitor at AREF pin
#define MYADC_1_1VREF 	3 // Internal 1.1V Voltage Reference with external capacitor at AREF pin

// Clock Div
#define MYADC_CLKDIV2	0
#define MYADC_CLKDIV4	2
#define MYADC_CLKDIV8	3
#define MYADC_CLKDIV16	4
#define MYADC_CLKDIV32	5
#define MYADC_CLKDIV64	6
#define MYADC_CLKDIV128	7

// ADC Auto trigger
#define MYADC_FREE_RUN		0 // Free Running mode 
#define MYADC_COMPARATOR	1 // Analog Comparator
#define MYADC_EXT_INTR0		2 // External Interrupt Request 0
#define MYADC_TMR0_COMPA	3 // Timer/Counter0 Compare Match A
#define MYADC_TMR0_OVERFLOW	4 // Timer/Counter0 Overflow 
#define MYADC_TMR1_COMPB	5 // Timer/Counter1 Compare Match B
#define MYADC_TMR1_OVERFLOW	6 // Timer/Counter1 Overflow 
#define MYADC_TMR1_CAPTURE	7 // Timer/Counter1 Capture Event

// Defaults
#define MYADC_DEFAULT_REF MYADC_AVCC
#define MYADC_DEFAULT_CLK MYADC_CLKDIV16

class myADC
{
	public:
		//myADC(int clkdiv = MYADC_CLKDIV16, int sig_in = );
		myADC();
		~myADC();
		
		// ADMUX
		void set_sigmux(int sig_in);
		void set_ref(int ref);
		void set_lar(bool set); // Left Adjust Register
		
		// ADCSRA
		void enable(bool en = true);
		void start_conversion();
		void conversion_wait();
		void set_clkdiv(int clkdiv);

		// ADCSRB
		void set_auto_trigger(int ats, bool at_enable = true);		
		
		// DIDR0
		void disable_din(uint8_t mask);

		// Data
		uint16_t read();
		uint16_t sample();


}; // myADC

