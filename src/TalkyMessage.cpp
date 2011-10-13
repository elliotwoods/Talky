/*
 *  TalkyMessage.cpp
 *  network protocol test
 *
 *  Created by Elliot Woods on 01/02/2011.
 *  Copyright 2011 Kimchi and Chips. All rights reserved.
 *
 */

#include "TalkyMessage.h"

namespace Talky {
	
	//------------------------------------------------------
	// TalkyMessageHeader
	//------------------------------------------------------
	//
	TalkyMessageHeader::TalkyMessageHeader() :
	version(0),
	contentsType(0) {
	}
	
	TalkyMessageHeader::TalkyMessageHeader(TalkyMessageHeader& other, ContentsType t)
	{
		setCompany(other.getCompany());
		setProtocol(other.getProtocol());
		setVersion(other.getVersion());
		setContentsType(t);
	}
	
	TalkyMessageHeader::TalkyMessageHeader(const char * Company, const char * Protocol, ProtocolVersion v, ContentsType t)
	{
		setCompany(Company);
		setProtocol(Protocol);
		setVersion(v);
		setContentsType(t);
	}
	
	const char* TalkyMessageHeader::getCompany() const {
		return company;
	}
	
	const char* TalkyMessageHeader::getProtocol() const {
		return protocol;
	}
	
	ProtocolVersion TalkyMessageHeader::getVersion() const {
		return version;
	}
	
	ContentsType TalkyMessageHeader::getContentsType() const {
		return contentsType;
	}
	
	unsigned long TalkyMessageHeader::getTimestamp() const {
		return timestamp;
	}
	
	void TalkyMessageHeader::setCompany(const char * s)	{
		memcpy(company, s, 2);
	}
	
	void TalkyMessageHeader::setProtocol(const char * s) {
		memcpy(protocol, s, 2);
	}
	
	void TalkyMessageHeader::setVersion(unsigned short v) {
		version = v;
	}
	
	void TalkyMessageHeader::setContentsType(unsigned short t) {
		contentsType = t;
	}
	
	void TalkyMessageHeader::setTimestamp()	{
		timestamp = clock() / CLOCKS_PER_MILLISEC;
	}
	
	string TalkyMessageHeader::toString() {

		stringstream out;
		
		out << "Company:\t" << string(company, 2) << "\n";
		out << "Protocol:\t" << string(protocol, 2) << "\n";
		out << "Version:\t" << version << "\n";
		out << "Timestamp:\t" << timestamp << "\n";
		out << "ContentsType:\t" << contentsType << "\n";
		
		return out.str();

	}
	
	bool TalkyMessageHeader::operator==(const TalkyMessageHeader &other) const {
		return
		company == other.getCompany() &&
		protocol == other.getProtocol() &&
		version == other.getVersion() &&
		contentsType == other.getContentsType();
	}
	//
	//------------------------------------------------------
	
	
	//------------------------------------------------------
	// TalkyMessage
	//------------------------------------------------------
	//
	TalkyMessage::TalkyMessage() {
		header = defaultHeader;
	}
	
	TalkyMessage::TalkyMessage(TalkyMessageHeader const &h) {
		header = h;
	}

	bool TalkyMessage::serialise(TalkyBuffer &buf) const {
		
		if (!buf.hasSpaceToWrite(getTotalLength()))
			return false;
		
		buf << TALKY_START_TOKEN;
		buf << header;
		buf << (TalkySerialisable&) payload;
		buf << TALKY_END_TOKEN;		
		
		return true;		
	}
	
	bool TalkyMessage::deSerialise(TalkyBuffer &buf) {
		
		unsigned char token;
		
		//token + payload header + message header
		if (!buf.hasSpaceToRead(sizeof(TalkyMessageHeader) + 1 + sizeof(BufferOffset)))
			return false;
		
		buf >> token;
		if (token != TALKY_START_TOKEN)
			 throw("Message corrupt!");
		
		buf >> header;
		buf >> (TalkySerialisable&) payload;
		
		buf >> token;
		if (token != TALKY_END_TOKEN)
			throw("Message corrupt!");
	}
	
	const TalkyBuffer& TalkyMessage::getPayload() const {
		return payload;
	}
	
	const TalkyMessageHeader& TalkyMessage::getHeader() const {
		return header;
	}

	void TalkyMessage::setHeader(Talky::TalkyMessageHeader const &h) {
		header = h;
		header.setTimestamp();
	}
	
	int TalkyMessage::getTotalLength() const {
		return	sizeof(unsigned char) +
				sizeof(TalkyMessageHeader) +
				sizeof(BufferOffset) +
				payload.size() +
				sizeof(unsigned char);
	}

	unsigned short TalkyMessage::getPayloadLength() const {
		return payload.size();
	}

	string TalkyMessage::toString()
	{
		
		stringstream out;

		out << header.toString();
		
		out << "Payload:\n";
		
		out << payload.toString();
		
		out << "\n\n";
		
		return out.str();
	}

	bool TalkyMessage::operator<(const TalkyMessage& other) const
	{
		return header.getTimestamp() < other.getHeader().getTimestamp();
	}

	void TalkyMessage::initPayload(unsigned short length)
	{
		payload.allocate(length);
	}
	
	//------
	
	TalkyMessageHeader TalkyMessage::defaultHeader = TalkyMessageHeader("KC", "Ta", 0, 0);
	
	void TalkyMessage::setDefaultHeader(Talky::TalkyMessageHeader &h) {
		defaultHeader = h;
	}
	//
	//------------------------------------------------------
	
	//------------------------
	// buffer stream operators
	//
	TalkyBuffer& operator<<(TalkyBuffer &b, TalkyMessage const &m) {
		if (m.serialise(b))
			return b;
		else
			throw("Buffer overrun - insufficient space to write");
	}
	
	bool operator>>(TalkyBuffer &b, TalkyMessage &m) {
		return m.deSerialise(b);
	}
	//
	//------------------------
}