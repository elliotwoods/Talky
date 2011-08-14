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
		
		unsigned short	contentsType;
		
		/** Timestamp for the message. 
		 This is set automatically when the header
		 is assigned to the message
		 */
		unsigned long	timestamp;
	};
	
	//------
	
	class TalkyMessage {
	public:
		friend class TalkyBuffer;
		
		TalkyMessage();
		
		template <class T>
		TalkyMessage& operator<<(T &object)
		{
			payload << object;
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
		
		void					setHeader(TalkyMessageHeader const &h);
		
		int						getTotalLength() const;
		unsigned short			getPayloadLength() const;
		void					initPayload(unsigned short length);
		string					toString();
		
		///for sorting by timestamp
		bool operator<(const TalkyMessage& other) const;
		
		/** Serialise this onto main buffer.
		 Consider instead using 
		 buf << msg;
		 syntax. Serialise may be depreciated / made private
		 */		
		bool	serialise(TalkyBuffer &buf) const;
		
		/** Deserialise this off of main buffer.
		 Consider instead using 
		 buf >> msg;
		 syntax. Deserialise may be depreciated / made private
		 */		
		bool	deSerialise(TalkyBuffer &buf);

	protected:
		TalkyMessageHeader	header;
		TalkyBuffer			payload;		
	};
	
	//-----
	
	///Used when putting a message onto main buffer
	TalkyBuffer& operator<<(TalkyBuffer& b, TalkyMessage const &m);
	///Used when pulling a message off main buffer
	bool operator>>(TalkyBuffer& b, TalkyMessage &m);

}