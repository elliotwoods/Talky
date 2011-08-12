#pragma once
/*
 *  TalkyMessage.h
 *  network protocol test
 *
 *  Created by Elliot Woods on 01/02/2011.
 *  Copyright 2011 Kimchi and Chips. All rights reserved.
 *
 */

#include <ctime>
#include <string>
#include <sstream>

using namespace std;

#include "TalkyBuffer.h"

#define CLOCKS_PER_MILLISEC (CLOCKS_PER_SEC / 1000)
#define TALKY_START_TOKEN (unsigned char)0
#define TALKY_END_TOKEN '\n'

namespace Talky {
	
	class TalkyMessageHeader {
	public:
		TalkyMessageHeader();
		TalkyMessageHeader(const char * Company, const char * Protocol, unsigned short Version, unsigned short ContentsType);
		
		unsigned short	getContentsType() const;
		unsigned long	getTimestamp() const;
		
		void	setCompany(const char * s);
		void	setProtocol(const char * s);
		void	setVersion(unsigned short v);
		void	setContentsType(unsigned short t);
		void	setTimestamp();
		
		string	toString();
		
	protected:
		char			company[2];
		
		char			protocol[2];
		unsigned short	version;
		unsigned long	timestamp;
		
		unsigned short	contentsType;
	};
	
	class TalkyMessage {
	public:
		TalkyMessage();
		
		bool	serialise(char* &message, int &remainingAvailableBytes);
		bool	serialise(TalkyBuffer &buf);
		
		bool	deSerialise(char* &message, int &remainingBytesReceived);
		bool	deSerialise(TalkyBuffer &buf);
		
		template <class T>
		TalkyMessage& operator<<(T &object)
		{
			setPayload(&object, sizeof(T));
		}
		
		template <class T>
		bool operator>>(T &object)
		{
			if (getPayloadLength() == sizeof(object))
				return (payload >> object);
			else
				return false;
		}
		
		const TalkyBuffer&			getPayload() const;
		const TalkyMessageHeader&	getHeader() const;
		
		void	setPayload(void* const message, unsigned short length);
		void	setHeader(TalkyMessageHeader const &h);
		
		int		getTotalLength();
		unsigned short getPayloadLength();
		void	initPayload(unsigned short length);
		
		string	toString();
		
		//for sorting by timestamp
		bool operator<(const TalkyMessage& other) const;

	protected:	
		TalkyMessageHeader	header;
		TalkyBuffer			payload;		
	};
}