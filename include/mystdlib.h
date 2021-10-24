/*

	mystdlib.h

*/
#include <stdlib.h>
#include <string.h>

#define BITSET(reg, bit)	( (reg) | (1<<(bit))	)
#define BITCLR(reg, bit)	( (reg) & (~(1<<(bit))) )
#define CLRSET(reg, bit) 	( ((reg) & (~(1<<(bit)))) | ((reg) | (1<<(bit))) )
