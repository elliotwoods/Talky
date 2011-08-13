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
	typedef unsigned long BufferOffset;
	
	class TalkyBuffer {
	public:
		TalkyBuffer();
		TalkyBuffer(BufferOffset size);
		
		~TalkyBuffer();
		
		void	allocate(BufferOffset size);
		void	deAllocate();
		void	clean();
		
		BufferOffset size() const;
		const void * getData() const;
		void setData(const void * d, BufferOffset size);
		
		bool		hasSpaceToWrite(BufferOffset size) const;
		bool		hasSpaceToRead(BufferOffset size) const;
		BufferOffset	getRemainingWriteSpace() const;
		BufferOffset	getRemainingReadSpace() const;
		
		///Used when putting a payload onto main buffers
		TalkyBuffer& operator<<(TalkyBuffer const &other);
		///Used when pulling a payload off main buffers
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
		
		
		
		/**
		 Hard ass access. You sure you really want to do this??
		 We need this in TalkyBase, consider moving to 'friend' class?
		 */
		char*	getWritePointer();
		/**
		 Hard ass access. You sure you really want to do this??
		 We need this in TalkyBase, consider moving to 'friend' class?
		 */
		void	advanceWritePointer(BufferOffset);
		
		/**
		 Hard ass access. You sure you really want to do this??
		 We need this in TalkyBase, consider moving to 'friend' class?
		 */
		char*	getReadPointer();
		/**
		 Hard ass access. You sure you really want to do this??
		 We need this in TalkyBase, consider moving to 'friend' class?
		 */
		void	advanceReadPointer(BufferOffset);
		
	protected:
		void	init();
		
		bool	write(const void* data, BufferOffset size);
		bool	read(void* data, BufferOffset size);
		
		///Used to perform dynamic reallocation. Returns false if we're dynamic allocation is turned off.
		bool	reAllocate(BufferOffset s);
		
		char*			data;
		
		///Does this buffer have space allocated?
		bool			isAllocated;
		
		///Does this buffer allow for space to be dynamically allocated for storage? If false, then we presume static allocation
		bool			isDynamicallyAllocated;
		
		///If this flag is true, we always reAllocate by a factor of 2, so that multiple reallocations are not necessary. We may wish to turn this flag off if we encounter large files.
		bool			quickReallocation;
		
		
		BufferOffset	allocatedSize;
		BufferOffset	writtenSize;
		
		BufferOffset	readOffset;
		BufferOffset	writeOffset;
		
	};
}