/*

	esp8266.cpp

*/

#include "esp8266.h"


//=====================
//  Private
//=====================
BTpgmrEcho* bte_ptr; //DEBUG

//
// _send()
//
// Runs a transaction with the ESP8266 
bool esp8266::_send(const char* data, char* resp, uint16_t rb_len, unsigned int timeout, const char* tkn)
{
	uint16_t rlen = 0;
  bool tkn_found = false;
  uint16_t tkn_len = strlen(tkn);

  // Send data
  if( data != nullptr ) uart_ptr->tr_str(data);

  // If not getting response, return
  if( rb_len == 0 ) return true;

  // Start timer
  tmr_ptr->start(timeout);
  while( !tmr_ptr->done() && !tkn_found ){
    tkn_found = _recv_to_buf(resp, &rlen, rb_len, tkn, tkn_len );
  }
	tmr_ptr->stop();
  return tkn_found;
} 

//
// _recv_to_buf()
//
// Does the heavy lifting for send function, places recieved data into buffer. Checks for token.
bool esp8266::_recv_to_buf(char *d, uint16_t* idx, uint16_t len, const char* tkn, uint16_t tkn_len)
{
  bool tkn_found = false;
  bool len_exceeded = ((*idx) >= len);
  while( uart_ptr->available() && !len_exceeded )
  {
    if( (*idx) < len ) {                  						// Gaurd from resp buffer overflow
      d[(*idx)]   = uart_ptr->read();     						// Get uart character
      if( (*idx)+1 < len ) d[(*idx)+1] = '\0';        // Null terminate string (if theres room) 
      (*idx) = (*idx) + 1;                            // Increment response length

      if( (*idx) >= tkn_len && (tkn_len > 0) )
      { // Dont look for token if tkn_len is zero
        for( uint16_t i = 0; (tkn_found = (tkn[i] == d[(*idx)-tkn_len+i])) && (i < tkn_len); i++ );
      }

    }
    else
      len_exceeded = true;
  }
  return (tkn_found | len_exceeded);
}


//=====================
//  Public
//=====================

//
// esp8266()
//
// Constructor
/*esp8266::esp8266()
{
	esp8266_rst = 2;
}*/

//DEBUG!!!!!!!!!!!!!!
esp8266::esp8266(BTpgmrEcho* b_p)
{
	bte_ptr = b_p;	
	esp8266_rst = 2;
}

//
// ~esp8266()
//
// destructor
esp8266::~esp8266()
{}

//
// init()
//
// Initialize ESP8266 module
bool esp8266::init(myUART* u_ptr, Stopwatch* t_ptr, uint8_t rst_pin, unsigned long int br)
{
	char setbr_resp[48];
	uart_ptr = u_ptr;
	tmr_ptr = t_ptr;

	esp8266_rst = rst_pin;
	setOutput(esp8266_rst);
	
	esp8266::hw_rst();	

	uart_ptr->set_br(9600); // Assume 9600 default for all ESP8266 (NOTE: dont change default)
	
	if( setBaudRate(br, setbr_resp, 48) )
		return true;
	else 
		return false;
}

//
// hw_rst()
//
// Hardware reset pulse to ESP8266
void esp8266::hw_rst()
{
	setPin(esp8266_rst, 0);
	_delay_ms(1000);
	setPin(esp8266_rst, 1);
	_delay_ms(1000);
	return;
}

//
// tr()
//
//	Transmits certain number of bytes instead of string
bool esp8266::tr(const uint8_t* data, uint16_t dlen, char* resp, uint16_t rb_len, unsigned int timeout, const char* tkn)
{
	// Send byte array
	uart_ptr->tr_b(data, dlen);
	// Get response  (nullptr will not send anything but will wait for specified response)
	return( send(nullptr, resp, rb_len, timeout, tkn) );
}

//
// send()
//
// Send data to ESP8266, recieve data to given response buffer pointer
bool esp8266::send(const char* data, char* resp, uint16_t rb_len, uint16_t timeout, const char* tkn) 
{
  if(resp == nullptr) 
	{
    rb_len = 0;
  }

  if(tkn == nullptr) 
	{
  	return( _send(data, resp, rb_len, timeout, ESP8266_SUCCESS_RESP) );
	}
	else
 		return( _send(data,resp,rb_len,timeout,tkn) );
}

//
// setBaudRate()
//
// Sets ESP8266 buad rate and local UART baud rate
bool esp8266::setBaudRate(long unsigned int br, char* rbuf, uint16_t rlen, uint16_t timeout)
{
	char set_br_cmd[31] = "AT+UART_CUR=";
	ultoa(br, (set_br_cmd+12), 10);
	strcat((set_br_cmd+12), ",8,1,0,0\r\n"); 
	if( send(set_br_cmd, rbuf, rlen, timeout) )
	{
		uart_ptr->set_br(br);
		return true;
	}
	else
		return false;
}

//
// connectWifi()
//
// Connect to a wifi network
bool esp8266::connectWifi(const char* ssid, const char* pswd, char* rbuf, uint16_t rblen )
{
	char jap_cmd[64];
	char* cmd_ptr;
	jap_cmd[0] = '\0';
	cmd_ptr = strcat(jap_cmd, "AT+CWJAP_CUR=\"");
	cmd_ptr = strcat(jap_cmd, ssid);
	cmd_ptr = strcat(jap_cmd, "\",\"");
	cmd_ptr = strcat(jap_cmd, pswd);
	cmd_ptr = strcat(jap_cmd, "\"\r\n");
	
	return( send(cmd_ptr, rbuf, rblen, 15000) );
}

//
//	listen()
//
//
bool esp8266::listen(char* d, uint16_t dlen, const char* delimiter, uint16_t timeout_ms)
{
  uint16_t  idx = 0, idx_prev = 0;
  uint16_t  tkn_len;
  bool 			data_recv_finished = false;
  
  if(delimiter != nullptr)  tkn_len = strlen(delimiter);
  else                      tkn_len = 0;

  if( timeout_ms > 0 ) tmr_ptr->start(timeout_ms); // Start timer (if a timeout value was given)
  
  while( !data_recv_finished )
  {
    if( timeout_ms > 0 )
		{
			if( tmr_ptr->done() )
			{
				return false;
			}
		} 
  
	//if( uart_ptr->available() && debug ) {
  //   uart_ptr->printnum(tmr_ptr->read()); uart_ptr->print("\r\n"); //DEBUG
  //  }
    data_recv_finished = _recv_to_buf(d, &idx, dlen, delimiter, tkn_len);
    
    if( idx > idx_prev ) // Reset timeout timer if data was received
    {
      tmr_ptr->reset();
			idx_prev = idx;
    }
  }

  // Debug
  //uart_ptr->printnum(time_s); uart_ptr->print(" s "); uart_ptr->printnum(tmr_ptr->read()); uart_ptr->print("ms listen     time\r\n");

  return true; // Return success if not timed out
}


//
//	getConnectionIDs
//
//	Polls esp8266 to get current connection status. Returns byte
//  encoded value where byte[4:0] represent connection on respective channel 
uint8_t esp8266::getConnectionIDs(char*d, uint16_t dlen)
{
	uint8_t cnxn_byte = 0;
	uint8_t link_id = 0;
	char* cip_sts_ptr = nullptr;
	char* str_ptr;
	
	if( send("AT+CIPSTATUS\r\n", d, dlen) )
	{
		str_ptr=d;
		while( (cip_sts_ptr=strstr(str_ptr,"+CIPSTATUS:")) != nullptr )
		{
			link_id = cip_sts_ptr[11] - 48;
			cnxn_byte |= (1<<link_id);
			str_ptr = (cip_sts_ptr+12);
		}
	}

	return cnxn_byte;
}

//
//	waitConnect
//
//	Waits for a connection to be made to the esp8266 (acting as server). Returns
// 	connection byte (see getConnectionIDs)
uint8_t esp8266::waitConnect(char* d, uint16_t dlen, uint16_t timeout_ms)
{
	uint8_t cnxn_byte = getConnectionIDs(d,dlen);
	uint8_t link_id = 0;
	if( cnxn_byte != 0 )
		return cnxn_byte;
	else
	{
		if( listen(d, dlen, ",CONNECT\r\n", timeout_ms) ) // Successful listen
		{
			// If second char is ',' assume char[0] is the connection port. (Should be quicker than poll cipstatus again)
			if(	d[1] == ',' )
			{
				link_id = (d[0] - 48);
				cnxn_byte = (1<<link_id);
			}
			else
				cnxn_byte = getConnectionIDs(d,dlen);
		}

	}
	
	return cnxn_byte;	
}

//
//	waitIPD()
//
//	Waits for network data from esp8266 server connection. Places data in buffer and
//	returns link ID. *dlen should be set to max buffer len, value will be changed to
//  data length received before function returns. If data received is not '+IPD' prefix,
//  function returns invalid 0xFF, but dlen value is still valid
uint8_t esp8266::waitIPD(char* d, uint16_t *dlen, uint16_t timeout_ms)
{
	const char* ipd_tkn = "\r\n+IPD,";
  uint16_t  idx = 0;
	uint16_t  max_dlen = *dlen;
	uint8_t		link_id = 0xFF;
 	uint16_t 	numStrLen = 0, num = 0;
  bool 			is_ipd = false; // Assume true until proven false
	char 			c = 0;

  if( timeout_ms > 0 ) tmr_ptr->start(timeout_ms); // Start timer (if timeout provided)
  
  //while( !data_recv_finished )
  while(1)
  {
		// Monitor timeout
		if( timeout_ms > 0 )
		{
			if( tmr_ptr->done() )
			{
				bte_ptr->print(d); bte_ptr->print(" d\n");
				return 0xFF;
			}
		} 
		
		// Get ipd header data
		if( !is_ipd )
		{
			if( uart_ptr->available() ) // data available
			{
				if( idx >= (max_dlen-1) ) // Guard from buffer overflow (-1 for the null char)
				{
					return 0xFF;
				}
				
				// Read char
				if( idx == 0 )
				{
					d[idx] = uart_ptr->read(); 	// First char, remove from buff
					c = d[idx];
					d[idx+1] = '\0'; 							// Null terminate string
					tmr_ptr->reset();							// reset stopwatch back to 0
				}
				else
					c = uart_ptr->peek();		// All next chars, only remove if is correct data

				*dlen = idx+1;///////

				if( idx < 7 ) // Looking for "+IPD," str
				{
					if( c != ipd_tkn[idx] )	return 0xFF; // Check recieved char is correct 
				}
				else if( idx == 7 ) // 8th char has to be link id
				{
					link_id = c - 48;
					if( link_id > 4 ) return 0xFF; // check its in valid range 0-4
				}
				else if( idx > 8 )
				{
					if( c >= '0' && c <= '9' )
						numStrLen++;
					else if( c == ':' ) // Confirms IPD data
					{
						d[idx] = '\0';
						num = atoi( (d+9) ); // pass pointer to start of data length number				
						is_ipd = true;
						idx = 0; // Reset index to 0
						uart_ptr->read(); 
					}
					else
						return 0xFF;
				}
				
				if( idx != 0 )	
				{
					d[idx] = uart_ptr->read(); 		// Discard char (must have been correct since we haven't returned)
					d[idx+1] = '\0'; 							// Null terminate string
					tmr_ptr->reset();							// reset stopwatch back to 0
				}
				if( !is_ipd ) 	idx++;
			}
		}
		else // confirmed IPD and we have data length. Now we just need to read 'num' bytes into buffer as they come in
		{
			if( _recv_to_buf(d, &idx, num, nullptr, 0) ) // Will return true once 'num' is hit
			{	
				*dlen = idx;
				return link_id;
			}
			*dlen = idx;
		}

	}// end while

}

//
// CIPsend()
//
// Sends data to a given link id. If data length is not provided (is zero), length will be calculated with strlen() (wont 
// work for non-string data). If a response buffer is not provided data will be sent without confirming response from ESP8266
// and function will always return true.
bool esp8266::CIPsend(uint8_t link_id, const char* data, uint16_t dataLen, char* resp, uint16_t rlen, uint16_t timeout_ms )
{
	char cip_cmd[20] = "AT+CIPSEND=0,";  // 20 bytes leaves room for values up to 9999 (+\r\n\0) - max ESP pkt is 2048	
	cip_cmd[11] = (link_id+48);
	if( dataLen == 0 ) dataLen = strlen(data); // NOTE: this will cause issues if a string is not whats provided
	itoa(dataLen, (cip_cmd+13), 10); // Put data length string into CIP cmd
	strcat((cip_cmd+13), "\r\n"); // Slap on the \r\n chars
	
	// Set up data send
	if( send( cip_cmd, resp, rlen, timeout_ms, "OK\r\n>" ) )
	{
		if( rlen == 0 ) _delay_ms(3);
		// Ready to send (send it!)
		//bte_ptr->print(data); bte_ptr->print(" data\n");
		if( esp8266::tr( (uint8_t*)data, dataLen, resp, rlen, timeout_ms, "SEND OK" ) ) return true;
		else 																																						return false;
			
	}
	else
		return false;

}

