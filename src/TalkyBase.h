#pragma once
//
//  TalkyBase.h
//  TalkyShapes
//
//  Created by Elliot Woods on 05/02/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//


#define TALKY_SLEEP_TIME 10
#define TALKY_RECONNECT_TIME 200

#define TALKY_BUFFER_IN_SIZE 100000
#define TALKY_BUFFER_OUT_SIZE 100000

#include "TalkyMessage.h"

#include <vector>
#include <ctime>
#include <iostream>

using namespace std;

namespace Talky {

	class TalkyBase
	{
	public:
		TalkyBase();
		virtual ~TalkyBase();
		
		////////////////////////////////
		// MAY NEED SOME OVERRIDING
		// FOR ERROR HANDLING
		////////////////////////////////
		//
		///setup as client on (default port=5001)
		virtual void    setup(string remoteHost, int remotePort=5001);
		
		///setup as server (default port=5001)
		virtual void    setup(int localPort=5001);

		virtual bool	getIsServerBound();
		virtual int		getNumClients();
		//
		////////////////////////////////
		
		bool	getIsConnected() { return isConnected; }
		float	getTimeUntilNextConnectNorm();
		
		string  getRemoteHost(); ///< Get the hostname of remote machine. Perhaps split this into 2 functions? or warn if we use this for client?
				
		TalkyBase&	operator<<(TalkyMessage& msg);
		bool		operator>>(TalkyMessage& msg);
		
		void    send(TalkyMessage &msg);
		bool	popMessage(TalkyMessage &msg);
		void	clearMessages();
		
		
		/** Direct access to the receiveQueue.
		 Ideally use the stream operators to get messages off the receiveQueue.
		 */
		vector<TalkyMessage> const	&getReceiveQueue() {return receiveQueue; };
		vector<TalkyMessage> const	&getSendQueue() { return sendQueue;};
		
		
	protected:
		void update();
		
		///Pull from Server's rx buffer onto _bufferIn
		bool rxServer(int iClient);
		///Pull from Client's rx buffer onto _bufferIn
		bool rxClient();
		
		///Push _bufferOut onto TCP node's tx buffer
		void tx();
		///Push from _bufferOut onto Server's tx buffer
		void txServer(int iClient, bool clean=true);
		///Push from _bufferOut onto Client's tx buffer
		void txClient(bool clean=true);
		
		////////////////////////////////
		// INTERFACE
		////////////////////////////////
		//
		virtual void beginThread() = 0;
		
		virtual bool lockThread() = 0;
		virtual void unlockThread() = 0;
		virtual bool lockServer() { return true; };
		virtual void unlockServer() { };
		
		virtual void initClient() = 0;
		virtual void initServer() = 0;
		
		virtual void startClient() = 0;
		virtual void startServer() = 0;
		
		virtual bool isClientConnected() = 0;
		virtual bool isServerConnected() = 0;
		virtual bool isServersClientConnected(int iClient) = 0;
		
		virtual int getNumServerClients() = 0;
		
		virtual int rxServer(int iClient, char *buffer, int bufferSize) = 0;
		virtual int rxClient(char *buffer, int bufferSize) = 0;
				
		virtual void txServer(int iClient, char *buffer, int messageSize) = 0;
		virtual void txClient(char *buffer, int messageSize) = 0;
		
		virtual void notifyClientIsNowConnected() = 0;
		virtual void notifyReceiveEvent(int msgCount) = 0;
		
		virtual void throwWarning(string s) = 0;
		virtual void throwError(string s) = 0;
		//
		////////////////////////////////
		
		vector<TalkyMessage>		sendQueue;
		vector<TalkyMessage>		receiveQueue;
		
		void    processIncomingBuffer();
		
		TalkyBuffer		_bufferIn;
		TalkyBuffer		_bufferOut;
		
		int				_localPort;
		
		string			_remoteHost;
		int				_remotePort;
		
		int				nodeType; ///< 0=none, 1=client, 2=server
		
		bool			isServerBound; ///< have we got the port?
		bool			isConnected;
		long			lastConnectAttempt;
	};
}