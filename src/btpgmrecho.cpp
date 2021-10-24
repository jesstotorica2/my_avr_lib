/*

  btpgmrecho.cpp

*/
#include "btpgmrecho.h"


/*

	Tx:	[ CMD | WLEN | RLEN | WDATA[0] | ... | ]
	Rx: [ xxx | RCNT | WCNT | RDATA[0] | ... | ]

*/

//=======================================
//
//	Private
//
//=======================================

//=======================================
//
//	Public
//
//=======================================

//
//	BTpgmrEcho
//
// 	Constructor
BTpgmrEcho::BTpgmrEcho(mySPI* s, uint8_t chipSelectPin)
{
	spi_ptr = s;
	cs_n = chipSelectPin;
	setOutput(cs_n);
	setPin(cs_n, 1);
}

//
//	tr_stream()
//
uint8_t BTpgmrEcho::tr_stream( uint8_t* w, uint8_t wlen, uint8_t* r, uint8_t rlen )
{
	uint8_t bytes_read;
	setPin(cs_n, 0);
	spi_ptr->tr( ECHO_WRITE | ECHO_READ ); 			// send cmd
	_delay_us(ECHO_TX_DELAY_US);
	bytes_read = spi_ptr->tr(wlen);							// send wlen, get rcnt
	if( bytes_read > rlen )	bytes_read = rlen;
	_delay_us(ECHO_TX_DELAY_US);								
	spi_ptr->tr(bytes_read);										// send rlen (ignore wcnt)
	_delay_us(ECHO_TX_DELAY_US);

	uint8_t ceiling = (bytes_read > wlen) ? bytes_read : wlen;
	uint8_t wbyte, rbyte;
	
	for( uint8_t i = 0; i < ceiling; i++ )
	{
		if( i < wlen ) wbyte = w[i];	// Still writing?
		else					 wbyte = 		0;
			
		rbyte = spi_ptr->tr(wbyte);		// Transfer data

		if( i < bytes_read ) r[i] = rbyte; // Still reading?
		
		_delay_us(ECHO_TX_DELAY_US); // Delay
	}
	
	setPin(cs_n, 1);

	return bytes_read;
}


//
//	wr_stream()
//
void BTpgmrEcho::wr_stream( uint8_t* w, uint8_t wlen )
{
	tr_stream( w, wlen, nullptr, 0 );
}

//
//	rd_stream()
//
uint8_t BTpgmrEcho::rd_stream( uint8_t* r, uint8_t rlen )
{
	return( tr_stream( nullptr, 0, r, rlen ) );
}


//
//	rd_available()
//
uint8_t BTpgmrEcho::rd_available()
{
	uint8_t bytes_available;
	setPin(cs_n, 0);
	spi_ptr->tr( ECHO_WRITE | ECHO_READ ); 			// send cmd
	_delay_us(ECHO_TX_DELAY_US);
	bytes_available = spi_ptr->tr(0);						// send wlen, get rcnt
	_delay_us(ECHO_TX_DELAY_US);								
	spi_ptr->tr(0);															// send rlen (ignore wcnt)
	_delay_us(ECHO_TX_DELAY_US);
	return bytes_available;
}

//
//	print()
//
//	String print easy use
void BTpgmrEcho::print(const char* d)
{
	uint16_t wlen = strlen(d);
	wr_stream((uint8_t*)d, wlen);
}


