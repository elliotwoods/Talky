#pragma once
//
//  TalkyBuffer.h
//  Talky
//
//  Created by Elliot Woods on 11/08/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include <sstream>
#include <string>

using namespace std;

namespace Talky {
	
	/** BufferOffset is a Talky type.
	 We use it for defining indexes within the 
	 TalkyBuffer.
	*/
	typedef unsigned short BufferOffset;
	
	class TalkyBuffer {
	public:
		TalkyBuffer();
		TalkyBuffer(BufferOffset size);
		
		~TalkyBuffer();
		
		void	allocate(BufferOffset size);
		void	deAllocate();
		
		BufferOffset size() const;
		const void * getData() const;
		void setData(const void * d, BufferOffset size);
		
		bool		hasSpaceToWrite(BufferOffset size) const;
		bool		hasSpaceToRead(BufferOffset size) const;
		
		TalkyBuffer& operator<<(TalkyBuffer const &other);
		bool operator>>(TalkyBuffer &other) const;
		
		template<class T>
		TalkyBuffer& operator<<(T const &object) {
			if (!write(&object, sizeof(T)))
				throw("Buffer overrun - insufficient space to write");
			return *this;
		}
		
/*		template<class T>
		TalkyBuffer& operator<<(T const object) {
			if (!write(&object, sizeof(T)))
				throw("Buffer overrun - insufficient space to write");
			return *this
		}
*/		
		bool operator>>(TalkyBuffer &other);
		
		template<class T>
		bool operator>>(T& object) {
			return read(&object, sizeof(T));
			readOffset += sizeof(T);
		}
		
		string toString(unsigned short maxLength=10) const;
		
	protected:
		void	init();
		
		bool	write(const void* data, BufferOffset size);
		bool	read(void* data, BufferOffset size);
		
		char*			data;
		bool			isAllocated;
		BufferOffset	allocatedSize;
		BufferOffset	writtenSize;
		
		BufferOffset	readOffset;
		BufferOffset	writeOffset;
		
	};
}