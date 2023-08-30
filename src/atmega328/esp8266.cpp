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
  	uint8_t recv_buf_ret = 0;
	uint16_t tkn_len = strlen(tkn);

	// flush buffer, wait till uart stops sending
	while( uart_ptr->available() ) uart_ptr->flush(); 
	// Send data
	if( data != nullptr ) uart_ptr->tr_str(data);

	// If not getting response, return
	if( rb_len == 0 ) return true;

	// Start timer
	tmr_ptr->start(timeout);
	while( !tmr_ptr->done() && (recv_buf_ret == 0) )
	{
	  recv_buf_ret = _recv_to_buf(resp, &rlen, rb_len, tkn, tkn_len);
	}
	tmr_ptr->stop();
	
	return( recv_buf_ret == 0x1 );
} 

//
// _recv_to_buf()
//
// Does the heavy lifting for send function, places recieved data into buffer. Checks for token.
// Returns
//  1. 0x0 - finished recieving data, token not found, length not exceeded
//	1. 0x1 - token found
//  2. 0x2 - 'len' exceeded
uint8_t esp8266::_recv_to_buf(char *d, uint16_t* idx, uint16_t len, const char* tkn, uint16_t tkn_len)
{
  bool tkn_found = false;
  bool len_exceeded = ((*idx) >= len);
  while( uart_ptr->available() && !len_exceeded )
  {
    if( (*idx) < len ) 
	{                  							// Guard from resp buffer overflow
      d[(*idx)] = uart_ptr->read();     		// Get uart character
      if( (*idx)+1 < len ) d[(*idx)+1] = '\0';  // Null terminate string (if theres room) 
      (*idx) = (*idx) + 1;                  	// Increment response length

	  // Dont look for token if tkn_len is zero
      if( (*idx) >= tkn_len && (tkn_len > 0) )
      { 
        for( uint16_t i = 0; (tkn_found = (tkn[i] == d[(*idx)-tkn_len+i])) && (i < tkn_len); i++ );
      }

    }
    else
	{
		// DEBUG!!!!!
		//uart_ptr->print("Got a length exceeded!!!\r\n");
		//uart_ptr->print("(*idx): "); uart_ptr->printnum((*idx));
		//uart_ptr->print("\r\n");
		//_delay_ms(3000);
		// DEBUG!!!!!
      len_exceeded = true;
  	}
  }

  if( tkn_found )
  {
	return 0x1;
  }
  else if( len_exceeded )
  {
  	return 0x2;
  }
  else
  {
	return 0x0;
  }
}


//=====================
//  Public
//=====================

//
// esp8266()
//
// Constructor
esp8266::esp8266()
{
	esp8266_rst = 2;
}

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
	//char setbr_resp[48];
	uart_ptr = u_ptr;
	tmr_ptr = t_ptr;

	esp8266_rst = rst_pin;
	
	return( resetBaudRate(br) );
}

//
// hw_rst()
//
// Hardware reset pulse to ESP8266.
// NOTE: If a pin is set as an input prior to calling hw_rst, function assumes
//       it is being used as an open-drain with external pull-up.
void esp8266::hw_rst()
{
	int pinDir = getPinDir(esp8266_rst); // Record the pin direction
	setPin(esp8266_rst, 0);
	if( pinDir == 0 ) // Previously set as an input
	{
		setOutput(esp8266_rst); // Set as output to pull low
	}
	_delay_ms(500);

	if( pinDir == 0 ) // Previously set as an input
	{
		setInput(esp8266_rst); // Set as input to tri-state
	}
	else
	{
		setPin(esp8266_rst, 1);
	}
	_delay_ms(500);

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
// resetBaudRate()
//
// This function first runs a hardware reset to set the es8266 back to default rate. Then resets
// baud rate to desired value 'br'. If the 'def_br' is not correct the function will fail. (Assumes 9600)
// Returns true on success
bool esp8266::resetBaudRate(long unsigned int br, long unsigned int def_br)
{
	char setbr_resp[48];
	this->hw_rst();
	uart_ptr->set_br(def_br); // Assume 9600 default for all ESP8266 (NOTE: dont change default)
	
	if( setBaudRate(br, setbr_resp, 48) )
		return true;
	else 
		return false;	
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
// poke()
//
bool esp8266::poke(uint16_t timeout)
{
	char ack_resp[16];
	return( send("AT\r\n", ack_resp, 16, timeout) );
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
	
	return( send(cmd_ptr, rbuf, rblen, 20000) );
}

//
//	listen()
//
//
bool esp8266::listen(char* d, uint16_t dlen, const char* delimiter, uint16_t timeout_ms)
{
  uint16_t  idx = 0, idx_prev = 0;
  uint16_t  tkn_len;
  bool 		data_recv_finished = false;
  
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
  
	// either tkn found or limit exceed
    data_recv_finished = ( 0 != _recv_to_buf(d, &idx, dlen, delimiter, tkn_len) ); 
    
    if( idx > idx_prev ) // Reset timeout timer if data was received
    {
    	tmr_ptr->reset();
		idx_prev = idx;
    }
  }

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
	
	if( send("AT+CIPSTATUS\r\n", d, dlen) )
	{
		if( (cip_sts_ptr=strstr(d,"+CIPSTATUS:")) != nullptr )
		{
			link_id = cip_sts_ptr[11] - 48;
			cnxn_byte |= (1<<link_id);
		}
	}

	return cnxn_byte;
}

//
//	getStatus
//
//	Polls esp8266 to get current status 'CIPSTATUS'. Returns integer value of status:
//		0 - Failed to talk to ESP
//		1 - Failed to find 'STATUS:' in ESP response
//		2 - Connected to AP
//		3 - TCP or UDP transmission has been created
//		4 - TCP or UDP transmission of station is disconnected (?)
//		5 - Station is NOT connected to an AP
uint8_t esp8266::getStatus(char*d, uint16_t dlen)
{
	uint8_t statusVal = 0;
	char* cip_sts_ptr = nullptr;
	
	if( send("AT+CIPSTATUS\r\n", d, dlen) )
	{
		if( (cip_sts_ptr = strstr(d,"STATUS:")) != nullptr )
			statusVal = cip_sts_ptr[7] - 48;
		else
			statusVal = 1;
	}
	else
		statusVal = 0;

	return statusVal;
}

//
// getIP
//	
// Returns the IP of ESP8266 in the for of a 32-bit value. Returns 0 if fails
// +CIFSR:STAIP,"192.168.1.200"{h0D}
uint32_t esp8266::getIP(char* d, uint16_t dlen)
{
	if( send("AT+CIFSR\r\n", d, dlen) )
	{
		char* ip_str;
		ip_str = strstr(d, ":STAIP,\"");
		ip_str = (ip_str+sizeof(":STAIP,\"")-1);
		return( IPstrtou(ip_str, dlen-(sizeof(":STAIP,\"")+1), '"') );
	}
	return 0;
}

//
// getDomainIP()
//
// Get IP for domain name
uint32_t esp8266::getDomainIP(const char* dname, char* d, uint16_t dlen)
{
	uart_ptr->tr_str("AT+CIPDOMAIN=\"");
	uart_ptr->tr_str(dname);
	if( send("\"\r\n", d, dlen, 10000) )
	{
		char* ip_str;
		ip_str = strstr(d, "+CIPDOMAIN:");
		ip_str = (ip_str+sizeof("+CIPDOMAIN:")-1);
		return( IPstrtou(ip_str, dlen-sizeof("+CIPDOMAIN:")+1, '\r') );
	}
	return 0;
	
}

//
//	connected
//
//	Polls esp8266 to get current status. Returns bool value
//  signifying whether the station is connected to an AP
bool esp8266::connected(char* d, uint16_t dlen)
{
	uint8_t sts = getStatus(d, dlen);
	return(	sts > 1 && sts < 5 );
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
				cnxn_byte |= (1<<link_id);
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
  	uint16_t  	idx = 0;
	uint16_t  	max_dlen = *dlen;
	uint8_t		link_id = 0xFF;
 	uint16_t 	numStrLen = 0, num = 0;
  	bool 		is_ipd = false; // Assume true until proven false
	char 		c = 0;

  if( timeout_ms > 0 ) tmr_ptr->start(timeout_ms); // Start timer (if timeout provided)
  
  //while( !data_recv_finished )
  while(1)
  {
		// Monitor timeout
		if( timeout_ms > 0 )
		{
			if( tmr_ptr->done() )
			{
				//bte_ptr->print(d); bte_ptr->print(" d\n");
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
					return 0xFE;
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
					if( c != ipd_tkn[idx] )	return 0xFD; // Check recieved char is correct 
				}
				else if( idx == 7 ) // 8th char has to be link id
				{
					link_id = c - 48;
					if( link_id > 4 ) return 0xFC; // check its in valid range 0-4
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
						return 0xFB;
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
			if( ( 0 != _recv_to_buf(d, &idx, num, nullptr, 0) ) ) // Will return true once 'num' is hit
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
bool esp8266::CIPsend(uint8_t link_id, const char* data, uint16_t dataLen, char* resp, uint16_t rlen, uint16_t timeout_ms)
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
		if( esp8266::tr( (uint8_t*)data, dataLen, resp, rlen, timeout_ms, "SEND OK" ) ) return true;
		else																			return false;
			
	}
	else
		return false;

}

//
// CIPstart()
//
// Starts a TCP connection on the specified link id with a given server IP address on a given port.
bool esp8266::CIPstart(uint8_t link_id, const char* serverIP, uint16_t port, char* resp, uint16_t rlen, uint16_t timeout_ms)
{
	char port_str[6];
	itoa(port, port_str, 10);  // Translate port into string
	
	uart_ptr->tr_str("AT+CIPSTART=");
	uart_ptr->tr(link_id+48);
	uart_ptr->tr_str(",\"TCP\",\"");
	uart_ptr->tr_str(serverIP);
	uart_ptr->tr_str("\",");
	uart_ptr->tr_str(port_str);

	return( send( "\r\n", resp, rlen, timeout_ms, "OK\r\n" ) );
}

//
// CIPclose()
//
// Close connection on a given link ID
bool esp8266::CIPclose(uint8_t link_id, char* resp, uint16_t rlen, uint16_t timeout_ms)
{
	uart_ptr->tr_str("AT+CIPCLOSE=");
	uart_ptr->tr(link_id+48);
	
	return( send( "\r\n", resp, rlen, timeout_ms, "OK\r\n>" ) );
}


// Helper
uint32_t esp8266::IPstrtou(char* ip_str, uint16_t dlen, char delim)
{
	if( ip_str != nullptr )
	{	
		char ip_byte_str[4];
		uint16_t byte_cnt = 0;
		uint32_t ip = 0;
		
		for( uint32_t i = 0, j = 0; ip_str[i] != '\0' && byte_cnt < 4 && i < dlen; i++, j++ )
		{
			if( j > 3 ) // Went more than 3 spaces without finding '.' or delim
			{
				return 0;
			}
			else if( ip_str[i] == '.' || ip_str[i] == delim )
			{
				ip_byte_str[j] = '\0'; // Cap off the ip byte string
				ip = (ip << 8) | atoi(ip_byte_str); // Convert string to integer and shift into IP 
				byte_cnt += 1;
				j = -1;
				if( byte_cnt >= 4 )
				{
					if( ip_str[i] != delim ) return 0;
					else 				     return ip;
				}
			}
			else
			{
				ip_byte_str[j] = ip_str[i];
			}
		}
	}

	return 0;

}

