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
		
		DataLog(void* log_buffer, int size);
		~DataLog();
		

		void* read(int idx);
		void write(void* data);


	private:
		void* buffer;

}; // DataLog

#endif
