/*

	myDataLog.cpp

*/

#include "myDataLog.h"

//=============================================
//
//	Public
//
//=============================================

//
// Constructor
//
DataLog::DataLog(void* log_buffer, int size)
{
	buffer = log_buffer;
}

//
// Destructor
//
DataLog::~DataLog()
{
}

//
// write()
//
void DataLog::write(void* data_ptr)
{
	*((DATALOG_DTYPE*)buffer + wr_ptr) = *(DATALOG_DTYPE*)data_ptr; 
	wr_ptr = (wr_ptr+1)%size;
}

//
// read()
//
// Read data at index 'idx' back from last log
void* DataLog::read(int idx)
{
	if( idx >= 0 ) // positive index - move back from wr_ptr
	{
		idx = (wr_ptr-1) - (idx%size);  // Move 'idx' spaces back from wr_ptr
		if( idx < 0 ) idx = size - idx;	// If we crossed over 0, subtract difference from size to get idx
	}
	else // negative index - move forward from wr_ptr
	{
		idx = idx*(-1);
		idx = wr_ptr-1+idx;
	}
	
	return( ((DATALOG_DTYPE*)buffer)+idx );
}

