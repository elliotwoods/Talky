//
//  TalkyBuffer.cpp
//  Talky
//
//  Created by Elliot Woods on 11/08/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "TalkyBuffer.h"

namespace Talky {

	//------
	
	TalkyBuffer::TalkyBuffer() {
		init();
	}
	
	TalkyBuffer::TalkyBuffer(BufferOffset size) {
		init();
		allocate(size);
	}
	
	TalkyBuffer::~TalkyBuffer() {
		deAllocate();
	}
	
	//------
	
	void TalkyBuffer::init() {
		readOffset = 0;
		writeOffset = 0;
		isAllocated = false;
		allocatedSize = 0;
		writtenSize = 0;
	}
	
	void TalkyBuffer::allocate(BufferOffset size) {
		deAllocate();
		
		data = new char[size];
		isAllocated = true;
		allocatedSize = size;
	}
	
	void TalkyBuffer::deAllocate() {
		if (!isAllocated)
			return;
		
		delete[] data;
		isAllocated = false;
	}
	
	//------
	
	BufferOffset TalkyBuffer::size() const {
		return writtenSize;
	}
	
	const void * TalkyBuffer::getData() const {
		return data;
	}
	
	void TalkyBuffer::setData(const void * d, BufferOffset size) {
		allocate(size);
		memcpy(data, d, size);
	}
	
	bool TalkyBuffer::hasSpaceToWrite(BufferOffset size) const {
		if (size > allocatedSize - writeOffset)
			return false;
		else
			return true;
	}
	
	bool TalkyBuffer::hasSpaceToRead(BufferOffset size) const {
		if (size > writtenSize - readOffset)
			return false;
		else
			return true;
	}
	
	bool TalkyBuffer::write(const void *data, BufferOffset size) {
		if (!hasSpaceToWrite(size))
			return false;
		
		memcpy((char*)data + writeOffset, data, size);
		writeOffset += size;
		if (writeOffset > writtenSize)
			writtenSize = writeOffset;
		
		return true;
	}
	
	bool TalkyBuffer::read(void *data, BufferOffset size) {
		if (!hasSpaceToRead(size))
			return false;
		
		memcpy(data, (char*)data + readOffset, size);
		readOffset += size;
		
		return true;
	}
	
	//------
	
	TalkyBuffer& TalkyBuffer::operator<<(TalkyBuffer const &other) {

		const BufferOffset size = other.size();
		
		*this << size;
		
		if (write(other.getData(), other.size()))
			return *this;
		else
			throw("Buffer overrun - insufficient space to write");
	}
	
	bool TalkyBuffer::operator>>(TalkyBuffer &other) {		
		
		if (!hasSpaceToRead(sizeof(BufferOffset)))
			return false;
			
		BufferOffset size;
			
		*this >> size;
		
		if (!hasSpaceToRead(size))
			return false;
		
		other.allocate(size);
		other.setData(data + readOffset, size);
		readOffset += size;
	}
	
	//------
	
	string TalkyBuffer::toString(unsigned short maxLength) const {
		
		stringstream out;
		unsigned short i;
		
		if (maxLength > writtenSize)
			maxLength = writtenSize;
		
		for (i=0; i<maxLength; i++)
		{
			if (data[i] > 32)
				out << data[i];
			else
				out << ".";
		}
		
		return out.str();
	}
	
}