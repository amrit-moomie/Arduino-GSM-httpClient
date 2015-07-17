/*
	SerialInterface.cpp - Library for interfacing with serial ports
	Created by Scott Williams, 05/07/2012
*/
#include "Arduino.h"
#include "string.h"
#include "SerialInterface.h"

#define BUFFSIZE 200
#define DELAY 40

HardwareSerial *USBSerial;
SoftwareSerial *GSMSerial;

SerialInterface::SerialInterface()
{	
}

void SerialInterface::initialise(HardwareSerial *&_USBSerial, SoftwareSerial *&_GSMSerial)
{
	USBSerial = _USBSerial;
	GSMSerial = _GSMSerial;
	USBSerial->begin(9600);
	GSMSerial->begin(9600);
}

bool SerialInterface::pollForResponseFromCommand(String command, String expectedResponse, bool resendCommand)
{
	char c, at_buffer[BUFFSIZE], expected[expectedResponse.length()+1];
	expectedResponse.toCharArray(expected, sizeof(expected)+1);
	byte buffidx = 0;
	
    GSMSerial->flush();
	if(!command.equals(""))
		printlnToGSM(command);
	
	unsigned long startTime = micros();	
	while(1)
	{
		if(resendCommand)
		{
			printlnToGSM(command);
			delay(200);
		}
		buffidx = 0;
		while(1) 
		{
			if(micros() - startTime > 1000000*DELAY)
			{
				return false;
			}
			if(GSMAvailable())
			{
			    c = readGSM();
				printToDebug(c);
				if((buffidx == BUFFSIZE - 1) || (c == '\r'))
				{
		            at_buffer[buffidx] = '\0';
		            break;				
		        }
		        at_buffer[buffidx++]= c;				
			}
		}
		
		if(strstr(at_buffer, "+CME ERROR: 29"))
			return false;

		if(strstr(at_buffer, "+STCPC:1"))
			return false;
				
		if(strstr(at_buffer, expected) != 0)
		    return true;
	}	
}

void SerialInterface::printToDebug(String data)
{
	USBSerial->print(data);
}

void SerialInterface::printToDebug(char c)
{
	USBSerial->print(c);
}

void SerialInterface::printlnToDebug(String data)
{
	USBSerial->println(data);
}

void SerialInterface::printToGSM(String data)
{	
	GSMSerial->print(data);
}

void SerialInterface::printToGSM(char c)
{
	GSMSerial->print(c);
}

void SerialInterface::writeToGSM(int c)
{
	GSMSerial->write(c);
}

void SerialInterface::printlnToGSM(String data)
{
	GSMSerial->println(data);
}

char SerialInterface::readDebug()
{
	return USBSerial->read();
}

char SerialInterface::readGSM()
{
	return GSMSerial->read();
}

bool SerialInterface::GSMAvailable()
{
	return GSMSerial->available();
}
void SerialInterface::flushGSM()
{
    GSMSerial->flush();
}
