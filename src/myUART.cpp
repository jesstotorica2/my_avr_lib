/*

  myUART.cpp

*/
#include <avr/io.h>
#include <avr/interrupt.h>

#include "myUART.h"

#ifndef FCLK
#define FCLK 16000000
#endif

#define UART_BR_SCALE16 FCLK/16UL
#define UART_BR_SCALE8  FCLK/8UL

unsigned char _myUART_CBUFF[myUART_BUFF_SIZE];
int  _myUART_rd_ptr;
int  _myUART_wr_ptr;
bool _myUART_buff_ovrflw;

//================
//  private
//================

//
//  _set_uart()
//
// Set uart settings
//
void myUART::_set_uart(baud_rate_t br, int dbits, int sbits, int pm ){
  // Baud Rate 
  set_br(br);

  // Set data bit size
  dbits = _dbits_encode(dbits);
  UCSR0B = (((dbits&0x8)>>2) << UCSZ02); 
  UCSR0C = ((dbits&0x3)<<1);

  // Set stop bits (1 or 2)
  sbits    = (sbits == 2) ? 1 : 0;
  UCSR0C |= (sbits<<USBS0);

  // Set parity mode
  UCSR0C |= ((pm&0x3)<<UPM00);

  // Enable Tx,Rx
  UCSR0B |= (1<<RXEN0)|(1<<TXEN0);
}

//
//  _dbits_encode()
//
//  Returns encoded 3-bit value for given no. of data bits
int myUART::_dbits_encode(int no_dbits){
  switch(no_dbits){
    case 5:   return 0;
    case 6:   return 1;
    case 7:   return 2;
    case 9:   return 7;
    default:  return 3;
  }
}

//
//  _is2x()
//
//  Checks U2X bit, returns value
bool myUART::_is2x(){
  return( (UCSR0A & (1<<U2X0)) ? true : false );
}

//
// _set2x()
//
//
void myUART::_set2x( uint8_t div_on )
{
	if(div_on) UCSR0A |= (1<<U2X0);
	else			 UCSR0A &= ~(1<<U2X0);				
}
//================
//  public
//================
myUART::myUART() {
  _myUART_rd_ptr = 0;
  _myUART_wr_ptr = 0;
  buff_en = true;
  _myUART_buff_ovrflw = false;
} 
 
void myUART::init(baud_rate_t br, int dbits, int sbits, int pm){
  _set_uart(br,dbits,sbits,pm);
  enable_buff();
}

//
//  set_br()
//
// Set the baud rate
void myUART::set_br(baud_rate_t br){
	baud_rate_t br_div;
	
	if( br > 38400 )
	{
		_set2x(1);
		br_div = (UART_BR_SCALE8/br - 1);
	}
	else
	{
		_set2x(0);
		br_div = (UART_BR_SCALE16/br - 1);
	}
  
  UBRR0H = (unsigned char)(br_div>>8); 
  UBRR0L = (unsigned char) br_div; 
}

//
//  enable_buff()
//
//  Enable UART buffer via ISR
void myUART::enable_buff() {
  buff_en = true;
  // Enable interupt  
  UCSR0B |= (1<<RXCIE0);
  sei();
}

//
// disable_buff()
//
// Disable UART buffer (i.e. interrupt)
void myUART::disable_buff() {
  buff_en = false;
  // Disable receive interrupt
  UCSR0B &= ~(1<<RXCIE0);
}

///////// Transmit ////////////////////

//
// tr()
//
// Transmit single word (5 - 9 bits long)
//
void myUART::tr(unsigned int d){
  /* Wait for empty transmit buffer */ 
  while (!(UCSR0A & (1<<UDRE0)));

  /* Copy 9th bit to TXB8 */
  UCSR0B &= ~(1<<TXB80);

  if (d & 0x0100)
    UCSR0B |= (1<<TXB80);
    /* Put data into buffer, sends the data */ 
  UDR0 = d;
}

//
//  tr_str() 
//
//  Transmit character string
//  
void myUART::tr_str(const char* d) {
  for( int i = 0; d[i] != '\0'; i++ ) {
    tr((unsigned int)(d[i]));
  }
}

//
//  tr_b() 
//
//  Transmit byte array
//  
void myUART::tr_b(unsigned char* d, int blen) {
  for( int i = 0; i < blen; i++ ) {
    tr((unsigned int)(d[i]));
  }
}

//
//  tr_9b()
//   
//  Transmit array of 9 bit values
//
void myUART::tr_9b(unsigned int* d, int blen = 0) {
  for( int i = 0; i < blen; i++ ) {
    tr(d[i]);
  }
}

//
// print()
//
// Prints const char string (i.e. print("my data") )
void myUART::print(const char* d) {
  for( int i = 0; d[i] != '\0'; i++ ) {
    tr((unsigned int)(d[i]));
  } 
}

//
// printnum()
//
// Converts integer to character array and sends data
void myUART::printnum(int num, int base) {
	char num_str[16];
	itoa(num, num_str, base);
	print(num_str);
}

///////// Ready ////////////////////


//
//  available()
//  
//  Data available?
//  
bool myUART::available() {
  if( !buff_en )
    return( !(UCSR0A & (1<<RXC0)) );
  else
    return( !( _myUART_wr_ptr == _myUART_rd_ptr ) );
}

//
//  read()
//
//  Read one character from the data buffer
//
char myUART::read() {
  return((char)_myUART_rd_buff());
}

//
//  flush()
//
//  Read one character from the data buffer
//
void myUART::flush() {
  _myUART_wr_ptr = _myUART_rd_ptr;
}



void _myUART_wr_buff(unsigned char x) {
  int wrptr_nxt = ((_myUART_wr_ptr+1)%myUART_BUFF_SIZE);  
  if(wrptr_nxt == _myUART_rd_ptr)   _myUART_buff_ovrflw = true;
  else {
    _myUART_CBUFF[_myUART_wr_ptr] = x;
    _myUART_wr_ptr = wrptr_nxt;
  }
}

unsigned char _myUART_rd_buff() {
  unsigned char x = _myUART_CBUFF[_myUART_rd_ptr];
  if( _myUART_rd_ptr == _myUART_wr_ptr ) {
    return 0;
  }else{
    _myUART_rd_ptr = ((_myUART_rd_ptr+1)%myUART_BUFF_SIZE);
    return x;
  } 
}


//
//  RX DATA ISR FUNCTION
//
//
//
#ifndef MY_UART_ISR_FUNC
#define MY_UART_ISR_FUNC
void MY_UART_RX_ISR() {}
#endif

//
// RX DATA ISR 
//
// 
//
#ifndef MY_UART_ISR
#define MY_UART_ISR
ISR (USART_RX_vect) {
  unsigned char x = UDR0;
  _myUART_wr_buff(x);
  MY_UART_RX_ISR();
}
#endif



