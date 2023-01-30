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
DataLog::DataLog(void* log_buffer, int log_size)
{
	buffer = log_buffer;
	size = log_size;
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
//#include "myUART.h"
//myUART* uart_dbg;
void* DataLog::read(int idx, int* wr_ptr_ref)
{
	if( wr_ptr_ref == nullptr ) wr_ptr_ref = &wr_ptr;
	//uart_dbg->print("idx start: "); uart_dbg->printnum(idx); uart_dbg->print("\r\n");
	//uart_dbg->print("wr_ptr_ref: "); uart_dbg->printnum(*wr_ptr_ref); uart_dbg->print("\r\n");
	if( idx >= 0 ) // positive index - move back from wr_ptr
	{
		idx = ((*wr_ptr_ref)-1) - (idx%size);  // Move 'idx' spaces back from wr_ptr
		if( idx < 0 ) idx = size + idx;	// If we crossed over 0, subtract difference from size to get idx
	}
	else // negative index - move forward from wr_ptr
	{
		idx = idx*(-1);
		idx = ((*wr_ptr_ref)-1+idx)%size;
	}
	//uart_dbg->print("idx end: "); uart_dbg->printnum(idx); uart_dbg->print("\r\n");
	return( ((DATALOG_DTYPE*)buffer)+idx );
}

