/*

	myTimedDataLog.h

*/

#ifndef _MY_TIMEDDATALOG_H_
#define _MY_TIMEDDATALOG_H_

#include "myDataLog.h"

#ifndef DATALOG_DTYPE 
#define DATALOG_DTYPE uint8_t
#endif

#ifndef TIMEDDATALOG_TIMER
#define TIMEDDATALOG_TIMER
#endif

//
// TimedDataLog
//
//
class TimedDataLog : public DataLog
{
	public: 
		TimedDataLog(uint16_t ts, void* log_buffer, int size);
		~TimedDataLog();

		uint32_t timestamp;				// Timestamp of latest data log
		uint32_t timestamp_rollover;	// Rollover limit of timestamp
		uint16_t tsteps;				// Timestep roll over limit	
		uint16_t tstep_count;			// Current timestep count
		int 	 points_logged;			// Number of data points logged. This is reset on start and when tsteps changes

		void start(uint32_t t = 0);			// Start log at time t
		void resume();						// Pick where we left off
		void stop();						// Stop logging
		
		void time_align(uint32_t t);	// Align logging to time t. Takes a sample immediately and sets timestamp=t 
		void set_tsteps(uint16_t ts);
		uint16_t get_time();		  	// Returns timestamp+tstep_count

		// This function should be compiled by user in main .cpp file. This function will be called
		// to retrieve data that needs recorded. Should return 'DATALOG_DTYPE' as defined by user
		// #define TIMEDDATALOG_TMR1_SAMPLE
		DATALOG_DTYPE tmr1_sample();
		
}; // TimedDataLog

//
// Global declarations (Allows for use in interupt)
//
void timeddatalog_tmr1_ovf();

#endif
