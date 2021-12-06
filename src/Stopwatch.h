/*

  Stopwatch.h

*/

#ifndef MY_STOPWATCH_H
#define MY_STOPWATCH_H

#include "mystdlib.h"
#include "myInterrupt.h"

class Stopwatch {
  private:
		uint8_t tmrVal;    
		void 		clearPSR();
		
  public:
    Stopwatch(uint8_t tmr = 0);
    ~Stopwatch();
		void start(unsigned int ms = 0);
		void reset();
		void stop();
    bool done();

		unsigned long int read();
};


#endif

