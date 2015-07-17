/*
	Sender.h - Library for interfacing with serial ports
	Created by Scott Williams, 06/07/2012
*/
#ifndef Sender_h
#define Sender_h

#include "string.h"

class Sender 
{
	public:
		virtual int sendPacket(String server, int port, String packet) = 0;
		virtual bool openTCPSocket(String server, int port) = 0;
		virtual bool sendRawPacket(String packet) = 0;
		virtual void closeTCPSocket() = 0;
};
#endif
