#ifndef BUFFER_INFO_H
#define BUFFER_INFO_H

#include <Rendering\RenderTypeDefs.h>

struct BufferInfo
{
	static const rend_uint MAX_BUFFER_SIZE = 0x100000; //1 MB

	rend_uint glBufferID;
	rend_uint nextAvailableByte;
	bool hasBuffer;

	BufferInfo() : hasBuffer(false),
		glBufferID(-1),
		nextAvailableByte(0){}

	rend_uint getRemainingSize() const { return MAX_BUFFER_SIZE - nextAvailableByte; }
};

#endif