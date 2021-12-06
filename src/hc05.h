/*
    hc05.h 

*/

#ifndef HC05_H
#define HC05_H

#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include "myUART.h"
#include "Stopwatch.h"
#include "myIO.h"

#define BT_DEFAULT_BAUD_RATE 		 38400
#define DEFAULT_TOKEN 				"OK\r\n"

class hc05 {
  private:
    myUART* uart_ptr;
    Stopwatch* tmr_ptr;
		uint8_t	EN_pin;

    bool p_send(const char* data, char* resp, int rb_len, unsigned int timeout, const char* tkn );
		bool recv_to_buf(char *d, int *idx, int len, const char* tkn, int tkn_len);


  public:
    hc05();
    bool init(myUART* u_ptr, Stopwatch* t_ptr, uint8_t enable_pin,  unsigned long int br);
		void tr(unsigned char x);
		bool send(const char* data, char* resp = nullptr, int rb_len = 0, unsigned int timeout = 5000, const char* tkn = nullptr);    
		bool sendnum(int num, int base = 10);
		bool poke(unsigned int timeout = 1000);
		bool set_uart( unsigned long int br, uint8_t stop = 0, uint8_t parity = 0);

		bool soft_reset();
		bool inquire();
		int  get_state();
		bool listen(char* d, int dlen, const char* delimiter = nullptr, int timeout_s = 0);

		//bool debug;
};

// HC-05 States
#define HC05_INITIALIZED 		0
#define HC05_READY					1
#define HC05_PAIRABLE				2
#define	HC05_PAIRED					3
#define HC05_INQUIRING 			4
#define HC05_CONNECTED 			5
#define HC05_DISCONNECTED 	6
#define HC05_NUKNOW 				7
#define HC05_UNKNOWN 			 -1





#endif // HC05_H
