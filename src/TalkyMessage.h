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


#define CLOCKS_PER_MILLISEC (CLOCKS_PER_SEC / 1000)
#define TALKY_ENDCHAR '\n'

namespace Talky {
	class TalkyMessage
	{
	public:
		TalkyMessage();
		
		bool	serialise(char* &message, int &remainingAvailableBytes);
		bool	deSerialise(char* &message, int &remainingBytesReceived);
		
		template <class T>
		TalkyMessage& operator<<(T &object)
		{
			setPayload(&object, sizeof(T));
		}
		
		template <class T>
		bool operator>>(T &object)
		{
			if (getPayloadLength() == sizeof(object))
			{
				object = *(T*)getPayload();
				return true;
			} else
				return false;
		}
		
		char	*getPayload(int &length) const;
		char	*getPayload() const;
		
		void	setPayload(void* const message, unsigned short length);
		int		getTotalLength();
		unsigned short getPayloadLength();
		void	initPayload(unsigned short length);
		
		string	toString();
		void	setCompany(const char * s);
		void	setProtocol(const char * s);
		void	setTimestamp();
		
		//for sorting by timestamp
		bool operator<(const TalkyMessage& other) const;
		
		//contents
		char			Company[2];
		
		char			Protocol[2];
		unsigned short	Version;
		unsigned long	Timestamp;
		
		unsigned short	ContentsType;
		
	protected:	
		unsigned short	PayloadLength;
		char			*Payload;
		
		bool			hasPayload;
		
	};
}