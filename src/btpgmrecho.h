/*

	btpgmrecho.h

*/

#ifndef __BTPGMRECHO_H__
#define __BTPGMRECHO_H__

#include "mystdlib.h"
#include "mySPI.h"

#define ECHO_WRITE 	0x40
#define ECHO_READ 	0x80

#define ECHO_TX_DELAY_US 10000

// DEBUG takeout
#include "myUART.h"
extern myUART uart;

class BTpgmrEcho {

	private:
		mySPI* spi_ptr;

		uint16_t _run_header(uint8_t cmd);

	public:
		uint8_t cs_n;	

		BTpgmrEcho(mySPI* s, uint8_t chipSelectPin);
		uint8_t tr_stream( uint8_t* w, uint8_t wlen, uint8_t* r, uint8_t rlen );
		void 		wr_stream( uint8_t* w, uint8_t wlen );
		uint8_t rd_stream( uint8_t* r, uint8_t rlen );
		uint8_t rd_available();

		void 		print(	const char* d	);
		void 		printnum( int num, uint8_t base = 10 );
};



#endif /*__BTPGMRECHO_H__*/
