//
//  TalkyBase.cpp
//  TalkyShapes
//
//  Created by Elliot Woods on 05/02/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "TalkyBase.h"

namespace Talky {


	///////////////////////////////////////////////////////////////
	// PUBLIC
	//

	TalkyBase::TalkyBase() : 
	nodeType(0),
	lastConnectAttempt(0),
	isConnected(false),
	isServerBound(false),
	_bufferIn(TALKY_BUFFER_IN_SIZE),
	_bufferOut(TALKY_BUFFER_OUT_SIZE)
	{
		
	}

	TalkyBase::~TalkyBase()
	{
		if (nodeType != 0)
			throw("You need to implement a destructor in your Talky class. Check  TalkyBase::~TalkyBase() for notes");
	  /*
	   //implement the following in destructor of TalkyBase implementing class. Example code from ofxTalky:
	   
	   if (nodeType==0)
		   return;
	   
	   stopThread(true);
	   
	   if (nodeType == 1)
	   {
		   tcpClient->close();
		   delete tcpClient;
	   }
	   
	   if (nodeType == 2)
	   {
		   tcpServer->close();
		   delete tcpServer;
	   }
	   
	   */
	}


	//CLIENT SETUP
	void TalkyBase::setup(string remoteHost, int remotePort)
	{
		if (nodeType != 0)
		{
			throw(string("Already initialised as node type ") + (nodeType==1 ? string("server") : string("client")));
			return;
		}
		
		
		_remoteHost = remoteHost;
		_remotePort = remotePort;
		
		initClient();
		beginThread();
		
		nodeType = 1;
		
	}

	//SERVER SETUP
	void TalkyBase::setup(int localPort)
	{
		if (nodeType != 0)
		{
			throw("Already initialised as node type " + (nodeType==1 ? string("server") : string("client")));
			return;
		}
		
		_localPort = localPort;
		
		initServer();
		beginThread();
		
		nodeType = 2;
		
	}

	bool TalkyBase::getIsServerBound()
	{
		if (nodeType==2)
			return isServerBound;
		else
			throw("I'm not a server node, so can't possibly be bound! so why are you asking?");
		return false;
	}

	int TalkyBase::getNumClients()
	{
		if (nodeType==2)
			return getNumServerClients();
		else
			throw ("I'm not a server node, so can't possibly have clients! so why are you asking?");
		return -1;
	}

	float TalkyBase::getTimeUntilNextConnectNorm()
	{
		float frac =  float(clock() / CLOCKS_PER_MILLISEC - lastConnectAttempt) / float(TALKY_RECONNECT_TIME);
		
		return (frac < 1.0f ? frac : 1.0f);
	}
	
	string TalkyBase::getRemoteHost(){
		if (nodeType == 1)
			return _remoteHost;
		else
			return "";
	}
	
	//-----------------------------------------------------------
	
	TalkyBase& TalkyBase::operator<<(const TalkyMessage &m) {
		lockThread();
		sendQueue.push_back(m);
		unlockThread();
		
		return *this;
	}
	
	bool TalkyBase::operator>>(TalkyMessage& m) {
		if (!lockThread())
			return false;
		
		if (receiveQueue.size() > 0)
		{
			m = receiveQueue.front();
			receiveQueue.erase(receiveQueue.begin());
			
			unlockThread();
			return true;
		} else	{
			unlockThread();
			return false;
		}
	}

	void TalkyBase::clearMessages()
	{
		while (!lockThread())
		{
#ifdef _WIN32
			Sleep(1);
#else
			sleep(10);
#endif
		}
		
		receiveQueue.clear();
		
		unlockThread();
	}


	///////////////////////////////////////////////////////////////
	// PROTECTED
	//

	void TalkyBase::update()
	{
		if (nodeType == 0)
			return;
		
		/////////////////////////////////////
		// CHECK RECONNECTS
		/////////////////////////////////////
		//
		int currentTime = clock() / CLOCKS_PER_MILLISEC;
		if (currentTime - lastConnectAttempt > TALKY_RECONNECT_TIME)
		{
			
			if (nodeType == 1) {
				//client
				if (!isConnected)
				{
					if (isClientConnected())
					{
						isConnected = true;
						notifyClientIsNowConnected();
					}
				} else
					if (!isClientConnected())
						isConnected = false;
			} else {
				//server
				isConnected = isServerConnected();
			}
			
			
			if (!isConnected && nodeType==1) {
				//client
				startClient();
				isConnected = isClientConnected();
				
				if (isConnected)
					notifyClientIsNowConnected();
			}
			
			
			if (!isServerBound && nodeType==2) {
				//server
				startServer();
				isConnected = isServerConnected();
			}
			
			lastConnectAttempt = currentTime;
		}
		//
		/////////////////////////////////////
		
		
		
		lockThread();
		
		/////////////////////////////////////
		// RECEIVE DATA
		/////////////////////////////////////
		//		
		//
		//
		if (nodeType == 1)
		{	
			if (isClientConnected())
			{				
				//client
				_bufferIn.clean();
				rxClient();
				processIncomingBuffer();
			}
			
		} else {
			
			//server
			if (getNumServerClients() > 0) {
				
				if (lockServer())
				{
					for (int iRemote=0; iRemote<getNumServerClients() ; iRemote++)
					{
						if (!isServersClientConnected(iRemote))
							continue; 
						
						_bufferIn.clean();
						rxServer(iRemote);				
						processIncomingBuffer();
						
					}
					unlockServer();
				}
				
			}
		}
			
		//
		/////////////////////////////////////
		
		
		
		/////////////////////////////////////
		// SEND DATA
		/////////////////////////////////////
		//
		if (isConnected)
		{
			bool hasDataToSend = false;
			
			try {
				while (sendQueue.size() > 0)
				{
					_bufferOut << sendQueue.front();
					sendQueue.erase(sendQueue.begin());
					hasDataToSend = true;
				}
			} catch (char* e) {
				//buffer overrun
				throwWarning("Buffer overrun, waiting until next frame to send remaining messages");
			}
			
			if (hasDataToSend)
				tx();
		}
		//
		/////////////////////////////////////
		
		unlockThread();
	}

	bool TalkyBase::rxServer(int iClient) {
		const int nBytesReceived = rxServer(iClient, _bufferIn.getWritePointer(), _bufferIn.getRemainingWriteSpace());
		
		try
		{
			_bufferIn.advanceWritePointer(nBytesReceived);
		} catch 
	}
	
	bool TalkyBase::rxClient() {
		const int nBytesReceived = rxClient(_bufferIn.getWritePointer(), _bufferIn.getRemainingWriteSpace());
		
		_bufferIn.advanceWritePointer(nBytesReceived);		
	}
	
	void TalkyBase::tx() {
		if (nodeType == 0) {
			throwWarning("TalkyBase::tx : can't send, we're not initialsed as either a client or server");
			return;
		}
		if (nodeType == 1) {
			//client
			txClient();
			
		} else if (nodeType == 2) {
			//server
			for (int iClient=0; iClient < getNumServerClients(); iClient++)
				txServer(iClient, false);
			
			//only clean the buffer once
			_bufferOut.clean();
		}
	}
	
	void TalkyBase::txServer(int iClient, bool clean) {
		int nBytesSending = _bufferOut.getRemainingReadSpace();
		txServer(iClient, _bufferOut.getReadPointer(), nBytesSending);
		
		if (clean)
			_bufferOut.clean();
	}
	
	void TalkyBase::txClient(bool clean) {
		int nBytesSending = _bufferOut.getRemainingReadSpace();
		txClient(_bufferOut.getReadPointer(), nBytesSending);
		
		if (clean)
			_bufferOut.clean();
	}
	
	void TalkyBase::processIncomingBuffer() {	
		//perhaps recode this so we dont copy?
		TalkyMessage msg;
		while (_bufferIn >> msg)
			receiveQueue.push_back(msg);
		
		//sort by timestamp
		sort(receiveQueue.begin(), receiveQueue.end());
		
		//trigger message available event.
		//processing will be performed in this thread
		int msgCount = receiveQueue.size();
		notifyReceiveEvent(msgCount);
	}
	
	string TalkyBase::toString() {
		stringstream out;
		
		out  << "Talky node is ";
		switch (nodeType) {
			case 0:
				out << "uninitialised." << endl;
				break;
				
			case 1:
				out << "a client, connecting to " << _remoteHost << " on port " << _remotePort << "." << endl;			
				break;
				
			case 2:
				out << "a server on local port " << _localPort << ", with " << getNumClients() << " clients" << endl;
		}
		
		return out.str();
	}
}