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
	
	TalkyBuffer::TalkyBuffer(const TalkyBuffer& other) {
		operator=(other);
	}
	
	TalkyBuffer::~TalkyBuffer() {
		deAllocate();
	}
	
	//------
	
	TalkyBuffer& TalkyBuffer::operator= (const TalkyBuffer & other) {
	
		isAllocated = other.getIsAllocated();
		isDynamicallyAllocated = other.getIsDynamicallyAllocated();
		quickReallocation = other.getIsQuickReallocation();
		allocatedSize = other.getAllocatedSize();
		writtenSize = other.getWrittenSize();
		readOffset = other.getReadOffset();
		writeOffset = other.getWriteOffset();
		
		
		_data = new char[allocatedSize];
		memcpy(_data, other.getData(), allocatedSize);
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
		
		_data = new char[size];
		isAllocated = true;
		isDynamicallyAllocated = false;
		allocatedSize = size;
	}
	
	void TalkyBuffer::deAllocate() {
		if (!isAllocated)
			return;
		
		delete[] _data;
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
			memcpy(newDataArea, _data, writtenSize);
			delete[] _data;			
		}
		
		_data = newDataArea;
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
		return _data;
	}
	
	void TalkyBuffer::setData(const void * d, BufferOffset size) {
		allocate(size);
		memcpy(_data, d, size);
		writtenSize = size;
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
			return (writtenSize - readOffset);
	}
	
	bool TalkyBuffer::write(const void *d, BufferOffset size) {
		if (!hasSpaceToWrite(size))
			if (!reAllocate(writeOffset + size))
				return false;
		
		memcpy(this->getWritePointer(), d, size);
		
		advanceWritePointer(size);
		
		return true;
	}
	
	bool TalkyBuffer::read(void *d, BufferOffset size) {
		if (!hasSpaceToRead(size))
			return false;
		
		memcpy(d, this->getReadPointer(), size);
		
		advanceReadPointer(size);
		
		return true;
	}
	
	//------
	
	void TalkyBuffer::serialiseToBuffer(TalkyBuffer &other) const {
		const BufferOffset s = this->size();
		other << s;
		
		if (!other.write(getData(), s))
			throw("Buffer overrun - insufficient space to write");
	}
	
	bool TalkyBuffer::deSerialiseFromBuffer(TalkyBuffer &other) {
		BufferOffset s;
		if (!(other >> s))
			return false;
		
		if (!other.hasSpaceToRead(s))
			return false;
		
		//clear our local data and replace
		//with section of other buffer's data
		setData(other.getReadPointer(), s);
		other.advanceReadPointer(s);
		
		return true;
	}
	
	//------
	bool TalkyBuffer::loadFile(string filename) {
		
		
		// this is untested
		
		ifstream inFile;
		
		if (inFile.is_open()) {
			try {
				
				inFile.open(filename.c_str(), ios::binary);

				//find filesize
				long begin, end;
				begin = inFile.tellg();
				inFile.seekg (0, ios::end);
				end = inFile.tellg();
				
				//allocate to this size
				allocate(end - begin);
				
				inFile.read(_data, end-begin);
			
				if (inFile.fail())
					throw;
			} catch (...) {
				if (inFile.is_open())
					inFile.close();
				return false;
			}
			
			inFile.close();
			return true;
		} else {
			return false;
		}
	}
	
	bool TalkyBuffer::saveFile(string filename) const {
		ofstream outFile;
		try {
			outFile.open(filename.c_str(), ios::binary | ios::out | ios::trunc);
			outFile.write(_data, size());
		} catch (...) {
			if (outFile.is_open())
				outFile.close();
			return false;
		}
		outFile.close();
		return true;
	}
	//------
	
	string TalkyBuffer::toString(unsigned short maxLength) const {
		
		stringstream out;
		unsigned short i;
		
		if (maxLength > writtenSize)
			maxLength = writtenSize;
		
		for (i=0; i<maxLength; i++)
		{
			if (_data[i] > 32)
				out << _data[i];
			else
				out << ".";
		}
		
		return out.str();
	}
	
	
	//------
	
	char* TalkyBuffer::getWritePointer() {
		return _data + writeOffset;
	}
	
	void TalkyBuffer::advanceWritePointer(BufferOffset s) {
		writeOffset += s;
		if (writeOffset > writtenSize)
			writtenSize = writeOffset;
		
		if (s > allocatedSize)
			throw ("TalkyBuffer::advanceWritePointer : Buffer overrun error");
	}
	
	char* TalkyBuffer::getReadPointer() {
		return _data + readOffset;
	}
	
	void TalkyBuffer::advanceReadPointer(BufferOffset s) {
		readOffset += s;
		
		if (s > allocatedSize)
			throw ("TalkyBuffer::advanceReadPointer : Buffer overrun error");
	}
	
	//------
	
	TalkyBuffer& operator<<(TalkyBuffer &b, TalkySerialisable const &o) {
		o.serialiseToBuffer(b);
		return b;
	}
	
	bool operator>>(TalkyBuffer &b, TalkySerialisable& o) {
		return o.deSerialiseFromBuffer(b);
	}	
	
}