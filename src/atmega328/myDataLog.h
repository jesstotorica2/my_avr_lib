#ifndef _DATA_LOG_H_
#define _DATA_LOG_H_

#include <stdlib.h>
#include <avr/io.h>

#ifndef DATALOG_DTYPE 
#define DATALOG_DTYPE uint8_t
#endif

//
// DataLog
//
//	Simple circular buffer.
class DataLog
{
	public:
		int 		wr_ptr;
		int 		size;
		
		DataLog(void* log_buffer, int log_size);
		~DataLog();
		
		void* read(int idx, int* wr_ptr_ref = nullptr);
		void write(void* data);

	private:
		void* buffer;

}; // DataLog

#endif
