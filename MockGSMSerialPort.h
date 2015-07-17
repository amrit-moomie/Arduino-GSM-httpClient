/*
	MockGSMSerialPort.h - Mock gsm module for testing purposes
	Created by Scott Williams, 05/07/2012
*/
#ifndef MockGSMSerialPort_h
#define MockGSMSerialPort_h

#include "Arduino.h"
#include "string.h"

class MockGSMSerialPort 
{
	public:
		MockGSMSerialPort();	
		void print(String data);
		void print(char c);
		void println(String data);
		void write(int c);
		char read();
		bool available();
	private:			
};
#endif
