/*

	atmega328_pgm.cpp

*/
#include "atmega328_pgm.h"

#include <util/delay.h>



#define EXIT_ON_ERR 1


//================
//  private
//================

//
//	Atmega328_Programmer()
//
//	Constructor
Atmega328_Programmer::Atmega328_Programmer(mySPI* s_ptr, myUART* u_ptr) {
	spi_ptr = s_ptr;
	spi_ptr->set_highz();

	#ifdef PGMR_DEBUG
	if(u_ptr == nullptr) u_ptr = (myUART*)malloc(sizeof(myUART));
	else				 u_ptr = u_ptr;
	#endif
	
	//setInput(SLV_RESET);    // Set input to tri-state
	//setPin(SLV_RESET, 0);
	setOpenDrainPin(SLV_RESET, 0); // Set input to tri-state

	// Initialize data members
	pgmMode = false;
	err_flag = 0;
}


//
//	sendSPI()
//
//	Send data
void Atmega328_Programmer::sendSPI(pgm_byte_t* data, pgm_byte_t* resp, int len, int check_rbyte) {
  setPin(SPI_MISO, 1); // Set weak-pull up
  spi_ptr->send(data, resp, len);
	if( check_rbyte > 0 ) {
		if( pgmMode && err_flag == 0 && resp[check_rbyte] != data[check_rbyte-1] ){ 
			err_flag = data[check_rbyte-1]; // Record the attempted command that failed. Do not overwrite
			#ifdef PGMR_DEBUG
			uart_ptr->print("Looking for 0x"); uart_ptr->printnum(data[check_rbyte-1]);    uart_ptr->print("\n"); 
			uart_ptr->print("at index ");     uart_ptr->printnum(check_rbyte); 					 uart_ptr->print("\n");
			printResp(data,4); 
			uart_ptr->print("\nResponse:\n");
			printResp(resp,4); uart_ptr->print("\n");
			#endif
		}
	
	}
}


//
//	wrPgBuff()
//
// 
void Atmega328_Programmer::wrPgBuff(int addr, pgm_byte_t hbyte, pgm_byte_t lbyte){
	pgm_byte_t resp[4];
	pgm_byte_t ld_l[4] = LD_PGM_MEM_LBYTE((pgm_byte_t)((pgm_byte_t)((addr+1)&PG_MSK)), lbyte);
	pgm_byte_t ld_h[4] = LD_PGM_MEM_HBYTE((pgm_byte_t)((pgm_byte_t)(addr&PG_MSK)), hbyte); 

	sendSPI( ld_l, resp, 4 ); // Low Byte first, 6 LSB Addr
	sendSPI( ld_h, resp, 4 ); // High Byte second, 6 LSB Addr
}


//================
//  public
//================

//
//	startProgrammingMode()
//
//	Puts Slave MCU into programming mode
bool Atmega328_Programmer::startProgrammingMode(){;
  pgm_byte_t prgm_en[4] = SPI_PROGRAM_EN;	
  pgm_byte_t resp[4];
 
  setPin(SPI_SCK, 0);
  setOutput(SPI_SCK);
  //setPin(SLV_RESET,1);
  setOpenDrainPin(SLV_RESET, 1);
  _delay_ms(100);

  // Set Reset low
  //setPin(SLV_RESET, 0);
  //setOutput(SLV_RESET);
  setOpenDrainPin(SLV_RESET, 0);
  _delay_ms(5);

   // Pulse Reset at least 2 cc
  //setInput(SLV_RESET);
  //setPin(SLV_RESET, 1);
  setOpenDrainPin(SLV_RESET, 1);
  
  _delay_ms(1); 
  //setOutput(SLV_RESET);
  //setPin(SLV_RESET, 0);
  setOpenDrainPin(SLV_RESET, 0);

  // Delay 20 ms
  _delay_ms(20);

  // Send program enable byte sequence
  sendSPI(prgm_en,resp,4); 
  
  // Check response for 0x53 echo on 3rd byte
  if( resp[2] == 0x53 )	pgmMode = true;			// Mark programming mode success
  else					endProgrammingMode();	// If failed, de-assert slave reset
  
  //Debug
  #ifdef PGMR_DEBUG
	printResp(resp,4);
  #endif

	
  return pgmMode;
}

//
//	endProgrammingMode()
//
//	De-assert slave reset, end programming mode
void Atmega328_Programmer::endProgrammingMode(){
	if( pgmMode )
		while( atmegaIsBusy() ) _delay_us(5);
	
	//setInput(SLV_RESET);	// Set input to tri-state
	//setPin(SLV_RESET, 0);
	setOpenDrainPin(SLV_RESET, 0); // Set input to tri-state
	pgmMode = false;
}

//
//	inPgmMode()
//
//	Returns true if target is in programming mode
bool Atmega328_Programmer::inPgmMode() {
	return pgmMode;
}

//
//	errFlag()
//
//	Returns value of error flag. (0 if no errors have occured)
pgm_byte_t Atmega328_Programmer::errFlag(){
	return err_flag;
}

//
//	ldPgBuff()
//
//	Writes 'blen' number of bytes to Page Buffer from starting buffer address
void Atmega328_Programmer::ldPgBuff( int staddr, pgm_byte_t* data, int blen){
	bool page_end = false;
	blen -= (blen%WORD_BLEN);

	for( int byte_idx = 0; (byte_idx < blen) && !page_end; byte_idx+=2, staddr++ ){
		wrPgBuff(staddr, data[byte_idx+1], data[byte_idx]); // Data byte high low (assuming L H L H)		

		page_end = (((staddr+1)&PG_MSK) == 0); // If next address is new page start
	} 
}

//
//	rdPmemByte()
//
// 	Read a byte from program memory. Uses word address plus high/low byte designation
pgm_byte_t Atmega328_Programmer::rdPmemByte(int addr, bool high){
	pgm_byte_t  rd[] = RD_PGM_MEM_LBYTE((pgm_byte_t)((addr&0xFF00)>>8),(pgm_byte_t)(addr&0xFF));
	pgm_byte_t  resp[4];

	if( high ) rd[0] = 0x28; 	
	sendSPI(rd, resp, 4);

	return resp[3];
}




//
//	cpPmemToBuff()
//
//	Copies Program Memory address range to mem buffer. Stops at page border if 
//	it comes before the end address.
void Atmega328_Programmer::cpPmemToBuff(int addr, int wlen){
	bool pg_end = false;

	for( int i = 0; (!pg_end && (i < wlen)); i++,addr++ )
	{
		wrPgBuff( (addr&PG_MSK), rdPmemByte((addr&PMEM_MSK), true),  rdPmemByte((addr&PMEM_MSK), false) );
		pg_end = (((addr+1)&PG_MSK) == 0); // If next address is new page start
	} 
}

//
// wrPgToPmem()
//
// Writes contents of page buffer to program memory. Rounds address down 
// to nearest page address 
void Atmega328_Programmer::wrPgToPmem(int addr, bool block) {
	addr = (addr&(~PG_MSK))&PMEM_MSK;
	pgm_byte_t resp[4];
	pgm_byte_t cmd[4] = WR_PGM_MEM_PG((pgm_byte_t)((addr>>8)&0xff),(pgm_byte_t)(addr&0xff));
	

	sendSPI(cmd,resp, 4);
	if( block )
	{
		while( atmegaIsBusy() ) _delay_us(5);
	}
}

//
//	atmegaIsBusy()
//
//	Polls Atmega with SPI cmd, returns true if busy
int Atmega328_Programmer::atmegaIsBusy(){
	int isBusy = 0;
	
	pgm_byte_t poll[4] = SPI_ATMEGA_POLL;
	pgm_byte_t resp[4];
	sendSPI(poll,resp,4);

	isBusy = (0x01 & resp[3]);
	return isBusy;
}


//
//	atmegaChipErase()
//
//	Erase Program Flash memory
void Atmega328_Programmer::atmegaChipErase(bool block){
	pgm_byte_t cmd[4] = SPI_CHIP_ERASE;
	pgm_byte_t resp[4];
	sendSPI(cmd,resp,4);

	if( block )
		while( atmegaIsBusy() ) _delay_us(5);
}

//
// wrPmem()
//
// Writes program memory from start address to end of data lepage
// sections are read back from Pmem, loaded to page buffer and re-written
void Atmega328_Programmer::wrPmem(int addr, pgm_byte_t* data, int blen) {
	bool last_page = false;
	int  end_addr = addr+(blen/WORD_BLEN);

	// Ensure Atmega is ready
	while( atmegaIsBusy() ) _delay_us(5);

	// Get leading words in start page from Pmem, load to buffer
	if( (addr&PG_MSK) != 0 ) cpPmemToBuff( (addr&(~PG_MSK)), addr&PG_MSK );

	// Cut of incomplete word
	blen -= (blen%WORD_BLEN);

	// Write pages
	for( int byte_idx = 0; !last_page; (byte_idx+=PG_BSIZE),(addr+=PG_SIZE) ) 
	{
		last_page = ((byte_idx+PG_BSIZE) >= blen); // Check if last page

		// If incomplete last page
		if( last_page && (byte_idx+PG_BSIZE != blen) ) { 										
			ldPgBuff( (addr&PG_MSK), (data+byte_idx), blen-byte_idx ); // Load page buffer with remaining dbytes
			cpPmemToBuff( end_addr, PG_SIZE );	// Read from pmem to page buffer till end of page
		}
		else{
			ldPgBuff( (addr&PG_MSK), (data+byte_idx), PG_BSIZE ); // Otherwise load page buffer
		}
		
		// Subtract start page offset from byte index ( byte_idx =   -offset_bytes + PG_BSIZE (after loop) )
		if( byte_idx == 0 )
			byte_idx -= ( (addr&PG_MSK)*WORD_BLEN );
		
		addr &= (~PG_MSK);			  // Address masked to lower page border
		wrPgToPmem(addr, !last_page); // Write page to pmem, dont block on last page
	} 

}


//
// readFuseByte()
// 
// 0 - Low byte
// 1 - High byte
// 2 - Extended byte
pgm_byte_t  Atmega328_Programmer::readFuseByte(pgm_byte_t idx)
{
	pgm_byte_t resp[4];
	
	while( atmegaIsBusy() ) _delay_ms(5);
	
	if( idx == FUSE_BYTE_LOW_IDX )
	{
		pgm_byte_t cmd[4] = RD_FUSE_BITS;
		sendSPI(cmd,resp, 4);
	}
	else if( idx == FUSE_BYTE_HIGH_IDX )
	{
		pgm_byte_t cmd[4] = RD_FUSE_HBITS;
		sendSPI(cmd,resp, 4);
	}
	else if( idx == FUSE_BYTE_EXT_IDX )
	{
		pgm_byte_t cmd[4] = RD_EXT_FUSE_BITS;
		sendSPI(cmd,resp, 4);
	}
	else
	{
		return 0xFF;
	}
	return resp[3];
}

//
// setFuseByte()
//
void Atmega328_Programmer::setFuseByte(pgm_byte_t idx, pgm_byte_t fbyte)
{
	pgm_byte_t resp[4];
	
	while( atmegaIsBusy() ) _delay_ms(5);
	
	if( idx == FUSE_BYTE_LOW_IDX )
	{
		pgm_byte_t cmd[4] = WR_FUSE_BITS(fbyte);
		sendSPI(cmd,resp, 4);
	}
	else if( idx == FUSE_BYTE_HIGH_IDX )
	{
		pgm_byte_t cmd[4] = WR_FUSE_HBITS(fbyte);
		sendSPI(cmd,resp, 4);
	}
	else if( idx == FUSE_BYTE_EXT_IDX )
	{
		pgm_byte_t cmd[4] = WR_EXT_FUSE_BITS(fbyte);
		sendSPI(cmd,resp, 4);
	}
}


#ifdef PGMR_DEBUG
// Helper
void Atmega328_Programmer::printResp(pgm_byte_t* resp, int len){
    uart_ptr->print("{");
    for(int i = 0; i < len; i++ ){
      uart_ptr->print(" 0x");
      uart_ptr->printnum((int)resp[i], 16);
      if( i == len-1 ) uart_ptr->print(" }");
      else uart_ptr->print(",");
    }
	uart_ptr->print("\r\n");
    
}
#endif
