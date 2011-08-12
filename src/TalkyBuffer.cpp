//
//  TalkyBuffer.cpp
//  Talky
//
//  Created by Elliot Woods on 11/08/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "TalkyBuffer.h"

namespace Talky {
	
	TalkyBuffer::TalkyBuffer() {
		isAllocated = false;
	}
	
	TalkyBuffer::~TalkyBuffer() {
		delete[] data;
	}
	
	TalkyBuffer::TalkyBuffer(BufferOffset size) {
		data = new char[size];
	}
	
}