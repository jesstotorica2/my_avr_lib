/*

  mySPI.h

*/

#ifndef __MY_SPI_H__
#define __MY_SPI_H__

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "mystdlib.h"
#include "myIO.h"

#ifndef SPI_BYTE_T 
#define SPI_BYTE_T unsigned char
#endif

#ifndef _SPI_ISR_ENABLE_
#define _SPI_ISR_ENABLE_ 1
#endif

typedef SPI_BYTE_T spi_byte_t;

class mySPI {
	private:
		int _tr(int d);
	
	public:
		mySPI();
		// Set register bits
		void init(int mst = 1, int clk_div = 0x5, int mode = 0);
		void enable(int mst);
		void disable();
		void set_mst();
		void set_slv();
		void set_cpha(int cpha);
		void set_cpol(int cpol);
		void set_clk_div(int div);
		void enable_interrupt();
		void disable_interrupt();

		// Send/recieve data
		int tr(int d);
		void send(spi_byte_t* d, spi_byte_t* r, int blen);
		

};

//#ifndef MY_SPI_ISR
//#define MY_SPI_ISR	_VOID_SPI_ISR_()
//#endif
//void _VOID_SPI_ISR_();
void _MY_SPI_ISR_();


#define SPI_MST			0x1
#define SPI_SLV			0x0

#define SPI_DIV2  	0x4
#define SPI_DIV4 		0x0
#define SPI_DIV8 		0x5
#define SPI_DIV16 	0x1
#define SPI_DIV32 	0x6
#define SPI_DIV64 	0x2
#define SPI_DIV128 	0x3

#ifndef SPI_SS
#define SPI_SS 	 10
#endif

#ifndef SPI_MOSI
#define SPI_MOSI 11
#endif

#ifndef SPI_MISO
#define SPI_MISO 12
#endif

#ifndef SPI_SCK
#define SPI_SCK  13
#endif

#endif
