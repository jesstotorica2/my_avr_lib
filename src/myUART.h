/*

  myUART.h

*/

#include <avr/io.h> 
#include <stdlib.h>

#ifndef MY_UART_H
#define MY_UART_H


#ifndef myUART_BUFF_SIZE
#define myUART_BUFF_SIZE 32
#endif

typedef unsigned long int baud_rate_t;

class myUART {
  private:
    bool buff_en;

    void _set_uart(baud_rate_t br, int dbits, int sbits, int pm);
    int  _dbits_encode(int no_dbits);
  	bool _is2x();
		void _set2x( uint8_t div_on );

  public:    
    myUART();
    void init(baud_rate_t br = 9600, int dbits = 8, int sbits = 1, int pm = 0);
    //void init(unsigned int br, int dbits, int sbits, int pm);
    void enable_buff();
		void disable_buff();
		void set_br(baud_rate_t br);

    // Transmit
    void tr(unsigned int d);
    void tr_str(const char* d);
    void tr_b(unsigned char* d, int blen);
    void tr_9b(unsigned int* d, int blen);
		void print(const char* d);
		void printnum(int num, int base = 10);

    // Receive
    bool available();
		char read();
		void flush();
};

void _myUART_wr_buff(unsigned char x);
unsigned char _myUART_rd_buff();
void MY_UART_RX_ISR();
#endif
