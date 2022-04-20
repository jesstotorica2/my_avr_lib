/*

  mySPI.h

*/

#include "mySPI.h"

//================
//  private
//================

//
// _tr()
//
// Transmit one byte of data
int mySPI::_tr(int d) {
	SPDR = d;
	while( (SPSR>>SPIF) != 0x1 );
	return( (int) SPDR );
}


//================
//  public
//================
//
//	mySPI()
//
// Constructor
mySPI::mySPI() 
{
}



//
//	init()
//
//
void mySPI::init(int mst, int clk_div, int mode) {
	SPCR = 0x0;
	// Set CPOL, CPHA
	SPCR |= ((mode&0xfc)<<CPHA);
	
	// Set Clk Divider
	SPCR |= (clk_div&0x3);
	SPSR |= ((clk_div>>2)&0x1);
	
	if( mst ) set_mst();
	else			set_slv();

}

//
// enable()	
//
//
void mySPI::enable(int mst){
	if( mst ) set_mst();
	else			set_slv();
	SPCR |= (1<<SPE);
}

//
// disable()	
//
//
void mySPI::disable(){
	SPCR = BITCLR(SPCR, SPE);
}


//
// set_mst()
//
//
void mySPI::set_mst(){
	SPCR = BITSET(SPCR, MSTR);
  
	// Set I/O 
	setOutput(SPI_SCK);  	// SCK
  setInput (SPI_MISO);  // MISO
  setOutput(SPI_MOSI);  // MOSI
	setOutput(SPI_SS);		// SS

	setPin(SPI_SS, 1);
}	

//
// set_slv()
//
//
void mySPI::set_slv(){
	BITCLR(SPCR, MSTR);
	
	// Set I/O 
  setPin(SPI_SCK, 0);
	setInput (SPI_SCK);  	// SCK
	setOutput(SPI_MISO);  // MISO
  setPin(SPI_MOSI, 0);
	setInput (SPI_MOSI);  // MOSI
  setPin(SPI_SS, 0);
	setInput (SPI_SS);		// SS

}

//
// set_highz()
//
// Set SPI I/O to high-impedance (all inputs)
void mySPI::set_highz(){
	
	// Set I/O 
  	setPin(SPI_SCK, 0);
	setInput (SPI_SCK);  	// SCK
  	setPin(SPI_MISO, 0);
	setInput(SPI_MISO);  // MISO
  	setPin(SPI_MOSI, 0);
	setInput (SPI_MOSI);  // MOSI
  	setPin(SPI_SS, 0);
	setInput (SPI_SS);		// SS

}

//
// set_cpha
//
//
void mySPI::set_cpha(int cpha) {
	if( cpha == 0 ) SPCR &= (~(1<<CPHA));
	else						SPCR |= (1<<CPHA);
}

//
// set_cpol
//
//
void mySPI::set_cpol(int cpol) {
	if( cpol == 0 ) SPCR &= (~(1<<CPOL));
	else						SPCR |= (1<<CPOL);
}


//
// set_clk_div()
//
//
void mySPI::set_clk_div(int div) {
	SPCR = ((SPCR & (0xfc)) | (div & 0x3));
	SPSR = ( (SPSR & (0xfe))  | ((div>>2)&0x1) );
}

//
// enable_interrupt()
//
//
void mySPI::enable_interrupt(){
	SPCR = BITSET(SPCR, SPIE);
	sei();
}

//
// disable_interrupt()
//
//
void mySPI::disable_interrupt(){
	SPCR = BITCLR(SPCR, SPIE);
}

//
// tr()
//
// Transmit one byte of data
int mySPI::tr(int d) {
	unsigned char B_dir = DDRB;
 	int ret = SPDR; // Clear SPDR (and SPIF bit)
	d &= 0xff;
	// Set pins 13(SCK), 11(MOSI), 10(SS) as ouputs
	DDRB |= 0x2C;
	// Enable SPI as master
	SPCR |= ( (1<<MSTR) | (1<<SPE) );
	
	
	// Write data to data reg.
	ret = _tr(d);
	
	// Clear En and Mstr
	SPCR &= ~( (1<<MSTR) | (1<<SPE) );
	// Return Data Direction Register back to former state
	DDRB = B_dir;
	return ret;
}


//
// send()
//
// Send array of bytes (char)
void mySPI::send(spi_byte_t* d, spi_byte_t* r, int blen) {
	unsigned char B_dir = DDRB;
 	SPDR; // Clear SPDR (and SPIF bit)
	// Set pins 13(SCK), 11(MOSI), 10(SS) as ouputs
	DDRB |= 0x2C;
	// Enable SPI as master
	SPCR |= ( (1<<MSTR) | (1<<SPE) );
	
	for( int i = 0; i < blen; i++ ) {
		r[i] = (spi_byte_t)_tr((int)d[i]);
	}

	// Clear En and Mstr
	SPCR &= ~( (1<<MSTR) | (1<<SPE) );
	// Return Data Direction Register back to former state
	DDRB = B_dir;

}

/*#if _SPI_ISR_ENABLE_ == 0
void _MY_SPI_ISR(){}
#endif*/
//erroror
//void _MY_SPI_ISR_() {}

//
/*ISR (SPI_STC_vect){
	//_MY_SPI_ISR_();
}*/


