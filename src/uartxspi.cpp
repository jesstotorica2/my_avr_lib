/*

	uartxspi.cpp

*/

#include "uartxspi.h"

//=========================
//
//	public
//
//=========================

//
// uartxspi
//
// Constructor
uartxspi::uartxspi()
{
	reset();
}

//
// uartxspi
//
// reset()
// Resets all pointers to zero
void uartxspi::reset()
{
	rd_uart_ptr	= 0;
	rd_spi_ptr	= 0;
	req_rlen 		= 0;

	wr_spi_ptr 	= 0;
	wr_uart_ptr	= 0;
	req_wlen  	= 0;
}
