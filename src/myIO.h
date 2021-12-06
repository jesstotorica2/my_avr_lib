/*

	myIO.h

*/

#include <avr/io.h>

// Digital - all GPIO
void setOutput(int pin);
void setInput(int pin);
void setPin(int pin, int val);
int  getPin(int pin);

#define _NUM_PINS_ 20
#define A0 14
#define A1 15
#define A2 16
#define A3 17 
#define A4 18
#define A5 19


