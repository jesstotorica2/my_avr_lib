/*

  myTimer.h

*/

#ifndef MY_SYSTIMER_H
#define MY_SYSTIMER_H


class Timer0 {
  private:
    void clearPSR();

  public:
    void TIMER0_ISR();
    
    Timer0();
    ~Timer0();
		void start(unsigned int ms = 0);
		void stop();
    bool done();

		unsigned long int read();
};

void MY_TIMER0_ISR();


#endif

