/*

	esp8266.h

*/

#include "mystdlib.h"
#include "myUART.h"
#include "Stopwatch.h"
#include "myIO.h"

//DEBUG!!!
#include "btpgmrecho.h"

#ifndef _ESP8266_H_
#define _ESP8266_H_

#define ESP8266_SUCCESS_RESP "OK\r\n"

// Commands
#define ESP8266_CIPSERVER_CMD(port) ("AT+CIPSERVER=1," #port "\r\n")
#define ESP8266_HOSTNAME_CMD(hostname) ("AT+CWHOSTNAME=\"" #hostname "\"\r\n")


class esp8266 {
	private:
		myUART* 		uart_ptr;
		Stopwatch* 		tmr_ptr;
		uint8_t			esp8266_rst;
		
		bool _send(const char* data, char* resp, uint16_t rb_len, uint16_t timeout, const char* tkn);
		bool _recv_to_buf(char *d, uint16_t* idx, uint16_t len, const char* tkn, uint16_t tkn_len);


	public:
		//esp8266(); DEBUG!!!!!!!
		esp8266(BTpgmrEcho* b_p);
		~esp8266();
		bool 			init(myUART* u_ptr, Stopwatch* t_ptr, uint8_t rst_pin, unsigned long int br);
		void 			hw_rst();
		bool 			tr(const uint8_t* data, uint16_t dlen, char* resp = nullptr, uint16_t rb_len = 0, uint16_t timeout = 5000, const char* tkn = nullptr);
		bool 			send(const char* data, char* resp = nullptr, uint16_t rb_len = 0, uint16_t timeout = 5000, const char* tkn = nullptr);
		bool 			setBaudRate(unsigned long int br, char* rbuf, uint16_t rlen, uint16_t timeout = 5000);
		bool 			connectWifi(const char* ssid, const char* pswd, char* rbuf, uint16_t rblen);
		bool 			listen(char* d, uint16_t dlen, const char* delimiter = nullptr, uint16_t timeout_ms = 0);
		uint8_t			getConnectionIDs(char*d, uint16_t dlen);
		uint8_t			getStatus(char *d, uint16_t dlen);
		bool			connected(char *d, uint16_t dlen);
		uint8_t 		waitConnect(char* d, uint16_t dlen, uint16_t timeout_ms = 0);
		uint8_t 		waitIPD(char* d, uint16_t *dlen, uint16_t timeout_ms = 0);
		bool 			CIPsend(uint8_t link_id, const char* data, uint16_t dataLen = 0, char* resp = nullptr, uint16_t rlen = 0, uint16_t timeout_ms = 5000 );		
	
}; // esp8266

#endif
