/*

	uartxspi.h

*/

#ifndef _UART_X_SPI_H
#define _UART_X_SPI_H

#include <stdint.h>

#ifndef MY_SPI_ISR
#define MY_SPI_ISR spi_isr()
#endif

class uartxspi {

	public:
		uint8_t	 			cmd;
		uint8_t				rd_uart_ptr, rd_spi_ptr, req_rlen; // Buffer max size 256
		uint8_t 			wr_uart_ptr, wr_spi_ptr, req_wlen; // Buffer max size 256

		uartxspi();
		void reset();
};

#endif
