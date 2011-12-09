#pragma once
//
//  TalkyBuffer.h
//  Talky
//
//  Created by Elliot Woods on 11/08/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include <string.h>
#include <math.h>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>

#include "TalkyException.h"

using namespace std;

namespace Talky {

	/** BufferOffset is a Talky type.
	 We use it for defining indexes within the
	 TalkyBuffer.
	*/
	typedef unsigned long BufferOffset;

	class TalkyBuffer;

	/** TalkySerialisable is an interface class for when
	 you want to override standard serialisation methods.
	 */
	class TalkySerialisable {
	public:
		virtual void	serialiseToBuffer(TalkyBuffer &b) const = 0;
		virtual bool	deSerialiseFromBuffer(TalkyBuffer &b) = 0;
	};

	class TalkyBuffer : public TalkySerialisable {
	public:
		TalkyBuffer();
		TalkyBuffer(BufferOffset size);
		TalkyBuffer(const TalkyBuffer&  other);

		~TalkyBuffer();

		///copy operator
		TalkyBuffer& operator= (const TalkyBuffer & other);

		void	allocate(BufferOffset size);
		void	deAllocate();
		void	clean();

		BufferOffset size() const;
		const void * getData() const;
		void	setData(const void * d, BufferOffset size);

		bool	write(const void* d, BufferOffset size);
		bool	read(void* d, BufferOffset size);

		bool		hasSpaceToWrite(BufferOffset size) const;
		bool		hasSpaceToRead(BufferOffset size) const;
		BufferOffset	getRemainingWriteSpace() const;
		BufferOffset	getRemainingReadSpace() const;

		template<class T>
		TalkyBuffer& operator<<(T const &object) {
			if (!write(&object, sizeof(T)))
				throw("Buffer overrun - insufficient space to write");
			return *this;
		}

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

		bool getIsAllocated() const { return isAllocated; };
		bool getIsDynamicallyAllocated() const { return isDynamicallyAllocated; };
		bool getIsQuickReallocation() const { return quickReallocation; };
		BufferOffset getAllocatedSize() const { return allocatedSize; };
		BufferOffset getWrittenSize() const { return writtenSize; };
		BufferOffset getReadOffset() const { return readOffset; };
		BufferOffset getWriteOffset() const { return writeOffset; };


		//TalkyBuffer inherits TalkySerialisable
		void	serialiseToBuffer(TalkyBuffer &b) const;
		bool	deSerialiseFromBuffer(TalkyBuffer &b);


		//File access
		bool	loadFile(string filename);
		bool	saveFile(string filename) const;

	protected:
		void	init();

		///Used to perform dynamic reallocation. Returns false if we're dynamic allocation is turned off.
		bool	reAllocate(BufferOffset s);

		char*			_data;

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

	///Overload template when serialising a TalkySerialisable object
	TalkyBuffer& operator<<(TalkyBuffer& b, TalkySerialisable const &o);
	///Overload template when deserialising to a TalkySerialisable object
	bool operator>>(TalkyBuffer& b, TalkySerialisable &o);
}
