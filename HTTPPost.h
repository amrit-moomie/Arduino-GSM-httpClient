/*
	HTTPPost.h - Library for sending HTTP post over GPRS
	Created by Sam Hughes-Gage, Adrian Wragg and Scott Williams 27/06/2012
*/
#ifndef HTTPPost_h
#define HTTPPost_h

#include "Arduino.h"
#include "string.h"
#include "SoftwareSerial.h"
class HTTPPost 
{
	public:
		HTTPPost();
		int initialise(HardwareSerial *_usbSerial, SoftwareSerial *_GSMSerial, String apn, String user,String Password, byte gsmResetPin);
		bool openTCPSocket(String server, int port);
		void closeTCPSocket();
		bool sendRawPostPacket(String host, int port, String path, String data, String contentType);
		int post(String server, int port, String path, String content, String contentType);
		int get(String server, int port, String path, String content);
};
#endif
