/*

  hc05.cpp

*/
#include "hc05.h"

//=====================
//	Private
//=====================


//
// p_send()
//
//  
bool hc05::p_send(const char* data, char* resp, int rb_len, unsigned int timeout, const char* tkn){
  int rlen = 0;
	bool tkn_found = false;
	int tkn_len	= strlen(tkn);
	uart_ptr->flush(); // Flush recieve buffer

	// Send data
	uart_ptr->tr_str(data);

	// If not getting response, return
	if( rb_len <= 0 ) return false;

	// Start timer
  tmr_ptr->start(timeout);
	while( !tmr_ptr->done() && !tkn_found ){
		tkn_found = recv_to_buf(resp, &rlen, rb_len, tkn, tkn_len );	
	}
	return tkn_found;	
}

//
//	recv_to_buf()
//
//	
bool hc05::recv_to_buf(char *d, int* idx, int len, const char* tkn, int tkn_len){
	bool tkn_found = false;
	bool len_exceeded = ((*idx) >= len);
	while( uart_ptr->available() && !len_exceeded ){
		if( (*idx) < len ) { 									// Gaurd from resp buffer overflow
			d[(*idx)]   = uart_ptr->read(); 		// Get uart character
			d[(*idx)+1] = '\0';						 			// Null terminate string 
			(*idx) = (*idx) + 1;														// Increment response length

			if( (*idx) >= tkn_len && (tkn_len > 0) ) { // Dont look for token if tkn_len is zero
				for( int i = 0; (tkn_found = (tkn[i] == d[(*idx)-tkn_len+i])) && (i < tkn_len); i++ );
			}

		}else
			len_exceeded = true;
	}
	return (tkn_found | len_exceeded);
}

//=====================
//	Public
//=====================

//
// hc05()
//
// Constructor
hc05::hc05(){
	//debug = false;	
	EN_pin = 7;
}

//
//	init()
//
// 	Initialize UART for HC-05 comms
bool hc05::init(myUART* u_ptr, Timer0* t_ptr, uint8_t enable_pin, unsigned long int br) {
	bool have_coms = false;
	EN_pin = enable_pin;
	setOutput(EN_pin);
	setPin(EN_pin, 1);
	if( br == 0 ) br = BT_DEFAULT_BAUD_RATE;
	
	if(u_ptr == nullptr){													 						// If no uart object passed	
		uart_ptr = (myUART*)malloc(sizeof(myUART));	 						// Create uart object 
		uart_ptr->init(br);  					 													// Initialize to Baud Rate
	}
	else									uart_ptr = u_ptr;

	if(t_ptr == nullptr)	tmr_ptr = (Timer0*)malloc(sizeof(Timer0));
	else									tmr_ptr = t_ptr;
	

	for( int i = 0; !(have_coms) && (i < 5); i++ ) // Attempt baud rate adjustment 5 times
	{
		uart_ptr->set_br(br); 			// Ensure baud rate is set
		poke(); // lead poke to clear error
		if( !(have_coms = poke()) ) // If passed baud rate does not work
		{
			uart_ptr->set_br(BT_DEFAULT_BAUD_RATE); // Try default 38400
			poke(); // lead poke to clear error
			if( poke() )	// If default works
			{
				set_uart(br, 0, 0);
				soft_reset();
			}
			uart_ptr->set_br(br);
		}
	}
	
	return have_coms;
	
}

//
// tr()
//
//
void hc05::tr(unsigned char x){
	uart_ptr->tr((unsigned int)x);
}

//
//	send()
//
//
bool hc05::send(const char* data, char* resp, int rb_len, unsigned int timeout, const char* tkn) {	
	if(tkn == nullptr) { 
		const char* x = DEFAULT_TOKEN;
		tkn = x;
	}
	if(resp == nullptr) {
		rb_len = 0;
	}
	
	return( p_send(data,resp,rb_len,timeout,tkn) );		
}

//
// sendnum()
//
// Converts integer to ascii chars and sends
bool hc05::sendnum(int num, int base)
{
	char num_str[16];
	itoa(num, num_str, base);
	return( send(num_str) );
}

//
//	poke()
//
//	Sends AT to bluetooth device, returns true if device responds
bool hc05::poke(unsigned int timeout)
{
	char s[] = "AT\r\n";
	char r[16];
	return(p_send(s,r,16, timeout, DEFAULT_TOKEN));
}

//
//	set_uart()
//
bool hc05::set_uart( unsigned long int br, uint8_t stop, uint8_t parity )
{
	char br_str[16];
	char stop_str[4];
	char parity_str[4];
	char cmd[32];
	char resp[16];
	strcpy(cmd, "AT+UART=");

	ultoa(br,br_str, 10);
	itoa(stop, stop_str,10);
	itoa(parity, parity_str, 10);
	
	strcat(cmd,br_str);
	strcat(cmd,",");
	strcat(cmd,stop_str);
	strcat(cmd,",");
	strcat(cmd,parity_str);
	strcat(cmd,"\r\n");

	return( p_send(cmd, resp, 16, 1000, DEFAULT_TOKEN) );
}

//
//	soft_reset()
//
//	Sends reset command to HC-05, then init command. Returns true if "OK" is recieved back
bool hc05::soft_reset()
{
	char r[16];
	bool success = false;

	setPin(EN_pin, 0);
	success = p_send("AT+RESET\r\n", r, 16, 1000, DEFAULT_TOKEN);
	_delay_ms(1000);
	setPin(EN_pin, 1);
	return success;
}


//
//	inquire()
//
//	Commands device to inquire, returns true if cmd issued successfully		
bool hc05::inquire(){	
	char r[16];
	return( send("AT+INQ\r\n", r, 16) );
}

//
//	get_state()
//
//	Returns enumerated value representing the HC05 state
int hc05::get_state()
{	
	char resp[32];
	bool send_fail = true;
	char *start = nullptr;
	char *end = nullptr;
	for( int i = 0; (send_fail = !send("AT+STATE?\r\n", resp, 32)) ; i++ ) _delay_ms(500);
	
	start = strstr(resp,"+STATE:");
	end 	= strstr(resp,"\r");

	if( start == nullptr || end == nullptr )
		return HC05_UNKNOWN;
	else{
		start += 7;
		*end = '\0';
	}
	

	if( send_fail )																	 return HC05_UNKNOWN;
	else if( strcmp(start, "INITIALIZED"		) == 0 ) return HC05_INITIALIZED;
	else if( strcmp(start, "READY"			 		) == 0 ) return HC05_READY;
	else if( strcmp(start, "PAIRABLE"	 			) == 0 ) return HC05_PAIRABLE;
	else if( strcmp(start, "PAIRED"		 			) == 0 ) return HC05_PAIRED;
	else if( strcmp(start, "INQUIRING"	 		) == 0 ) return HC05_INQUIRING;
	else if( strcmp(start, "CONNECTED"	 		) == 0 ) return HC05_CONNECTED;
	else if( strcmp(start, "DISCONNECTED"		) == 0 ) return HC05_DISCONNECTED;
	else if( strcmp(start, "NUKNOW"					) == 0 ) return HC05_NUKNOW;
	else 																						 return HC05_UNKNOWN;

}


//
//	listen()
//
//	Listen for data froma bluetooth connection
bool hc05::listen(char* d, int dlen, const char* delimiter, int timeout_s)
{
	int  idx = 0, idx_prev = 0;
	int  tkn_len;
	int  time_s = 0;
	bool data_recv_finished = false;
	if(timeout_s <= 0)
	{
		time_s = -1;
		timeout_s = 0;
	}
	

	if(delimiter != nullptr)	tkn_len = strlen(delimiter);
	else											tkn_len = 0;

	if( timeout_s > 0 ) tmr_ptr->start(1000); // Start 1 second timer (if a timeout value was given)
	uart_ptr->flush();
	
	while( !data_recv_finished && (time_s < timeout_s) )
	{
		if(tmr_ptr->done() && timeout_s > 0)
		{
			time_s += 1;
			tmr_ptr->start(1000);
		}
		 
	//if( uart_ptr->available() && debug ) {
	//	 uart_ptr->printnum(tmr_ptr->read()); uart_ptr->print("\r\n"); //DEBUG
	//	}
		data_recv_finished = recv_to_buf(d, &idx, dlen, delimiter, tkn_len);
		/*
		if( idx > idx_prev ) // Reset timeout timer if data was received
		{
			time_s = 0;
			idx_prev = idx;
		}*/
	}	

	// Debug
	//uart_ptr->printnum(time_s); uart_ptr->print(" s "); uart_ptr->printnum(tmr_ptr->read()); uart_ptr->print("ms listen time\r\n");

	tmr_ptr->stop();	
	return(time_s < timeout_s); // Return success if not timed out
}
