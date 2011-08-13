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
		
		isDynamicallyAllocated = true;
		isAllocated = false;
		quickReallocation = true;
		
		allocatedSize = 0;
		writtenSize = 0;
	}
	
	void TalkyBuffer::allocate(BufferOffset size) {
		deAllocate();
		
		data = new char[size];
		isAllocated = true;
		isDynamicallyAllocated = false;
		allocatedSize = size;
	}
	
	void TalkyBuffer::deAllocate() {
		if (!isAllocated)
			return;
		
		delete[] data;
		isAllocated = false;
	}
	
	bool TalkyBuffer::reAllocate(BufferOffset s) {
		
		if (!isDynamicallyAllocated)
			return false;
		
		if (s == 0)
		{
			deAllocate();
			return false;
		}
		
		if (quickReallocation)
			s = BufferOffset(1) << BufferOffset(ceil(log(float(s))/log(2.0f)));
			// e.g. = 1 << ceil(log2(5));
			// 8 = 1 << 3
		
		char* newDataArea = new char[s];
		
		if (isAllocated)
		{
			memcpy(newDataArea, data, writtenSize);
			delete[] data;			
		}
		
		data = newDataArea;
		isAllocated = true;
		allocatedSize = s;
		
		return true;
	}

	void TalkyBuffer::clean() {
		readOffset = 0;
		writeOffset = 0;
		writtenSize = 0;
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
		if (size > getRemainingWriteSpace())
			return false;
		else
			return true;
	}
	
	bool TalkyBuffer::hasSpaceToRead(BufferOffset size) const {
		if (size > getRemainingReadSpace())
			return false;
		else
			return true;
	}
	
	BufferOffset TalkyBuffer::getRemainingWriteSpace() const {
		if (writeOffset >= allocatedSize)
			return 0;
		else
			return (allocatedSize - writeOffset);
	}
	
	BufferOffset TalkyBuffer::getRemainingReadSpace() const {
		if (readOffset >= allocatedSize)
			return 0;
		else
			return (allocatedSize - readOffset);
	}
	
	bool TalkyBuffer::write(const void *data, BufferOffset size) {
		if (!hasSpaceToWrite(size))
			if (!reAllocate(writeOffset + size))
				return false;
		
		memcpy((char*)data + writeOffset, data, size);
		
		advanceWritePointer(size);
		
		return true;
	}
	
	bool TalkyBuffer::read(void *data, BufferOffset size) {
		if (!hasSpaceToRead(size))
			return false;
		
		memcpy(data, (char*)data + readOffset, size);
		
		advanceReadPointer(size);
		
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
	
	
	//------
	
	char* TalkyBuffer::getWritePointer() {
		return data + writeOffset;
	}
	
	void TalkyBuffer::advanceWritePointer(BufferOffset s) {
		writeOffset += s;
		if (writeOffset > writtenSize)
			writtenSize = writeOffset;
		
		if (s > allocatedSize)
			throw ("TalkyBuffer::advanceWritePointer : Buffer overrun error");
	}
	
	char* TalkyBuffer::getReadPointer() {
		return data + readOffset;
	}
	
	void TalkyBuffer::advanceReadPointer(BufferOffset s) {
		readOffset += s;
		
		if (s > allocatedSize)
			throw ("TalkyBuffer::advanceReadPointer : Buffer overrun error");
	}
	
}