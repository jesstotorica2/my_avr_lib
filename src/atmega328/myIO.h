/*

	myIO.h

*/

#ifndef __MY_AVR_IO_h__
#define __MY_AVR_IO_h__

#include <avr/io.h>

// Global pull-up disable bit (set to 1 to disable 10K pull-up)
void setPUD(int val);

// Digital - all GPIO
void setOutput(int pin);
void setInput(int pin);
void setPin(int pin, int val);
void setOpenDrainPin(int pin, int val, bool pullup = true);

int  getPin(int pin);
int  getPinDir(int pin);

#define _NUM_PINS_ 20
#define A0 14
#define A1 15
#define A2 16
#define A3 17 
#define A4 18
#define A5 19

#endif
