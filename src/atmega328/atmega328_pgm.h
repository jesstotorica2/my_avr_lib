/*

	atmega328_pgm.h

*/

#ifndef _ATMEGA328_PGM_H_
#define _ATMEGA328_PGM_H_

#include "mySPI.h"
#include "myUART.h"
#include "myIO.h"
#include <stdlib.h>
#include <util/delay.h>
#include "Stopwatch.h"

extern Stopwatch tmr0;

#ifndef PGM_BYTE_T
#define PGM_BYTE_T unsigned char
#endif 

#ifndef PGM_WORD_T
#define PGM_WORD_T unsigned int
#endif 

#define PGMR_DEBUG
//typedef unsigned char byte_t;
typedef PGM_BYTE_T pgm_byte_t;

class Atmega328_Programmer {
	public:
		// Public methods
		Atmega328_Programmer(mySPI* s_ptr = nullptr, myUART* u_ptr = nullptr);
		bool 				startProgrammingMode();
		void 				endProgrammingMode();
		bool 				inPgmMode();
	 	pgm_byte_t			errFlag();

		void 				ldPgBuff(int staddr, pgm_byte_t* data, int blen);
		pgm_byte_t 			rdPmemByte(int addr, bool high);
		void				cpPmemToBuff(int addr, int endaddr);
		void				wrPgBuff(int addr, pgm_byte_t hbyte, pgm_byte_t lbyte);
		void				wrPgToPmem(int addr, bool block = true);
		
		int  				atmegaIsBusy();
		void				atmegaChipErase(bool block = false);
		void				wrPmem(int staddr, pgm_byte_t* data, int blen);
		pgm_byte_t  		readFuseByte(pgm_byte_t idx);
		void 				setFuseByte(pgm_byte_t idx, pgm_byte_t fbyte);

	private:
		// Data members
		mySPI* 			spi_ptr;
		bool 				pgmMode;
		pgm_byte_t  err_flag;	

		// Private methods
		void sendSPI(pgm_byte_t* data, pgm_byte_t* resp, int len, int check_rbyte = 0);

		#ifdef PGMR_DEBUG
		myUART* uart_ptr;
		void printResp(pgm_byte_t* resp, int len);
		#endif
};

#ifndef SLV_RESET
#define SLV_RESET 6
#endif

#ifndef SPI_SS
#define SPI_SS 10
#endif

#ifndef SPI_MOSI
#define SPI_MOSI      11
#endif

#ifndef SPI_MISO
#define SPI_MISO      12
#endif

#ifndef SPI_SCK
#define SPI_SCK 13
#endif


#define PG_MSK 0x3F
#define PMEM_MSK 0x3FFF
#define WORD_BLEN 2
#define PG_SIZE 64
#define PG_BSIZE PG_SIZE*WORD_BLEN

//////////////////////////////////////////////////////////
// Atmega 328 SPI Instruction Set
// Instruction/Operation
#define SPI_PROGRAM_EN {0xAC, 0x53, 0x00, 0x00}
#define SPI_CHIP_ERASE {0xAC, 0x80, 0x00, 0x00}
#define SPI_ATMEGA_POLL {0xF0, 0x00, 0x00, 0x00}

// Load Instructions
#define LD_EXT_ADDR_BYTE(ext_addr) {0x4D, 0x00, ext_addr, 0x00}
#define LD_PGM_MEM_HBYTE(addr_lsb, hbyte_in) {0x48, 0x00, addr_lsb, hbyte_in}
#define LD_PGM_MEM_LBYTE(addr_lsb, lbyte_in) {0x40, 0x00, addr_lsb, lbyte_in}
#define LD_EEPROM_MEM_PG(addr, byte_in) {0xC1, 0x00, (0x03 & addr), byte_in} 

// Read Instructions
#define RD_PGM_MEM_HBYTE(addr_msb, addr_lsb) {0x28, addr_msb, addr_lsb, 0x00}
#define RD_PGM_MEM_LBYTE(addr_msb, addr_lsb) {0x20, addr_msb, addr_lsb, 0x00}
#define RD_EEPROM_MEM_PG(addr_h, addr_l) {0xA0, (0x3 & addr_h), addr_l, 0x00} 
#define RD_LOCK_BITS {0x58,0x00,0x00,0x00}
#define RD_SIGNATURE_BITS(addr) {0x30, 0x00, (0x3&addr),0x00}
#define RD_FUSE_BITS {0x50,0x00,0x00,0x00}
#define RD_FUSE_HBITS {0x58,0x08,0x00,0x00}
#define RD_EXT_FUSE_BITS {0x50,0x08,0x00,0x00}
#define RD_CAL_BYTE {0x38,0x00,0x00,0x00}

// Write Instructions
#define WR_PGM_MEM_PG(addr_msb,addr_lsb) {0x4C, addr_msb, addr_lsb, 0x00}
#define WR_EEPROM_MEM(addr_h,addr_l,byte_in) {0xC0, (0x03&addr_h), addr_l, byte_in}
#define WR_EEPROM_PG(addr) {0xC2, ((0xC0 & addr) >> 6), (addr << 2 ), 0x00}
#define WR_LOCK_BITS(byte_in) {0xAC, 0xE0, 0x00, byte_in}
#define WR_FUSE_BITS(byte_in) {0xAC, 0xA0, 0x00, byte_in}
#define WR_FUSE_HBITS(byte_in) {0xAC, 0xA8, 0x00, byte_in}
#define WR_EXT_FUSE_BITS(byte_in) {0xAC, 0xA4, 0x00, byte_in}


// Fuse byte indexes
#define FUSE_BYTE_LOW_IDX 0
#define FUSE_BYTE_HIGH_IDX 1
#define FUSE_BYTE_EXT_IDX 2 

#endif
