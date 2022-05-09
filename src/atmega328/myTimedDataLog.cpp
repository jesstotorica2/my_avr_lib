/*

	myTimedDataLog.cpp

*/

#include "myTimer.h"
#include "myTimedDataLog.h"

// Global counter variables
TimedDataLog* _tdl_tmr1_ptr = nullptr;
DATALOG_DTYPE _tdl_sample_buf; 

//
// Constructor
//
TimedDataLog::TimedDataLog(uint16_t ts, void* log_buffer, int size) : DataLog(log_buffer, size)
{
	tstep_count = 0;
	points_logged = 0;
	tsteps = ts;
	timestamp_rollover = 0xFFFF;
}

//
// Destructor
//
TimedDataLog::~TimedDataLog()
{
	stop();
}

//
// start()
//
void TimedDataLog::start(uint16_t t)
{
	points_logged = 0;
	time_align(t);
	tmr_setTOIE1(0x1);
}

//
// resume()
//
// Pick up where it left off, same time samp etc
void TimedDataLog::resume()
{
	tmr_setTOIE1(0x1);
}

//
// stop()
//
void TimedDataLog::stop()
{
	tmr_setTOIE1(0x0);
}


//
// time_align()
//
void TimedDataLog::time_align(uint16_t t)
{
		tstep_count = 0;	// Reset time step count to zero
		TCNT1 = 0;			// Reset timer count to zero	

		#ifdef TIMEDDATALOG_TMR1_SAMPLE
		write(tmr1_sample());
		#endif
}

//
// set_tsteps()
//
void TimedDataLog::set_tsteps(uint16_t ts)
{
	points_logged = 0;
	tsteps = ts;
}

//
// get_time()
//
uint16_t TimedDataLog::get_time()
{
	return(timestamp+tstep_count);
}



/**** Interrupt Function  ****/

//
// timeddatalog_tmr1_ovf
//
void timeddatalog_tmr1_ovf()
{
	_tdl_tmr1_ptr->tstep_count++;
	
	if( _tdl_tmr1_ptr->tstep_count >= _tdl_tmr1_ptr->tsteps )
	{
		_tdl_tmr1_ptr->tstep_count = 0;
		#ifdef TIMEDDATALOG_TMR1_SAMPLE
		_tdl_sample_buf = tmr1_sample();
		_tdl_tmr1_ptr->write(&_tdl_sample_buf);
		#endif
		_tdl_tmr1_ptr->timestamp += (_tdl_tmr1_ptr->timestamp + _tdl_tmr1_ptr->tsteps)%_tdl_tmr1_ptr->timestamp_rollover;;
		if( _tdl_tmr1_ptr->points_logged < _tdl_tmr1_ptr->size ) _tdl_tmr1_ptr->points_logged++;
	}
}

