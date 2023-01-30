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
	_tdl_tmr1_ptr = this;
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
void TimedDataLog::start(uint32_t t)
{
	points_logged = 1;
	t = t%timestamp_rollover;
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
void TimedDataLog::time_align(unsigned long t)
{
		timestamp = t;
		tstep_count = 0;	// Reset time step count to zero
		TCNT1 = 0;			// Reset timer count to zero	

		#ifdef TIMEDDATALOG_TMR1_SAMPLE
		_tdl_sample_buf = tmr1_sample();
		write(&_tdl_sample_buf);
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
//#include "myUART.h"
//myUART* uart_dbg;
void timeddatalog_tmr1_ovf()
{
	_tdl_tmr1_ptr->tstep_count++;
	//uart_dbg->print("tstep_count: "); uart_dbg->printnum(_tdl_tmr1_ptr->tstep_count); uart_dbg->print("\r\n");
	if( _tdl_tmr1_ptr->tstep_count >= _tdl_tmr1_ptr->tsteps )
	{
		_tdl_tmr1_ptr->tstep_count = 0;
		#ifdef TIMEDDATALOG_TMR1_SAMPLE
		_tdl_sample_buf = _tdl_tmr1_ptr->tmr1_sample();
		//uart_dbg->print("Got sample: "); uart_dbg->printnum(_tdl_sample_buf); uart_dbg->print("\r\n");
		_tdl_tmr1_ptr->write(&_tdl_sample_buf);
		//uart_dbg->print("wr_ptr: "); uart_dbg->printnum(_tdl_tmr1_ptr->wr_ptr); uart_dbg->print("\r\n");
		#endif
		//uart_dbg->print("timestamp: "); uart_dbg->printnum(_tdl_tmr1_ptr->timestamp); uart_dbg->print("\r\n");
		//uart_dbg->print("timestamp_rollover: "); uart_dbg->printnum(_tdl_tmr1_ptr->timestamp_rollover); uart_dbg->print("\r\n");
		//uart_dbg->print("size: "); uart_dbg->printnum(_tdl_tmr1_ptr->size); uart_dbg->print("\r\n");
		_tdl_tmr1_ptr->timestamp = (_tdl_tmr1_ptr->timestamp + _tdl_tmr1_ptr->tsteps)%_tdl_tmr1_ptr->timestamp_rollover;
		if( _tdl_tmr1_ptr->points_logged < _tdl_tmr1_ptr->size ) _tdl_tmr1_ptr->points_logged++;
		//uart_dbg->print("points_logged: "); uart_dbg->printnum(_tdl_tmr1_ptr->points_logged); uart_dbg->print("\r\n");
		//uart_dbg->print("timestamp: "); uart_dbg->printnum(_tdl_tmr1_ptr->timestamp); uart_dbg->print("\r\n");
		
	}
}

