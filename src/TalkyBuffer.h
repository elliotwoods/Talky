#pragma once
//
//  TalkyBuffer.h
//  Talky
//
//  Created by Elliot Woods on 11/08/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

namespace Talky {
	
	/** BufferOffset is a Talky type.
	 We use it for defining indexes within the 
	 TalkyBuffer.
	*/
	typedef unsigned short BufferOffset;
	
	class TalkyBuffer {
	public:
		TalkyBuffer();
		~TalkyBuffer();
		TalkyBuffer(BufferOffset size);
		
	protected:
		char*	data;
		bool	isAllocated;
	};
}