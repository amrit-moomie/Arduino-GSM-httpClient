
/*
	GSMInterface.cpp - Library for interfacing with the gsm module
	Created by Scott Williams, 05/07/2012
*/
#include "Arduino.h"
#include "string.h"
#include "SerialInterface.h"
#include "GSMInterface.h"

//Can't seem to call this serialInterface for some reason
SerialInterface *serialinterface;
byte gsmResetPin;
const char _command_SEPARATOR[] PROGMEM = "\",\"";

GSMInterface::GSMInterface()
{	
}

int GSMInterface::initialise(SerialInterface *_serialInterface, String apn, String user,String Password, byte _gsmResetPin)
{
	serialinterface = _serialInterface;
	gsmResetPin = _gsmResetPin;
	resetGSM(_gsmResetPin);
    delay(2000);
	if(!registerGPRS_GSM())
    {
		return 504;
    }
    else
    {
    serialinterface->printlnToDebug("[GPRS CONNECTED]");
    }
    
	if(!attachGPRSContinue(apn,user,Password))
    {
        return 503;
    }
	else
    {
        serialinterface->printlnToDebug("[GPRS BEARER SELECTED]");
		return 200;
    }
}

int GSMInterface::sendPacket(String server, int port, String packet)
{
	if(!openTCPSocket(server, port))
		return 500;
		
	int packetSize = packet.length()-1;
	serialinterface->printToGSM("AT+QISEND=" + String(packetSize));
    serialinterface->printToGSM('\r');
	delay(100);
	serialinterface->printToGSM(packet);
	
	if(serialinterface->pollForResponseFromCommand("", "OK", false))
	{
		delay(100);
		//closeTCPSocket();
		return 200;
	}
	else
	{
		closeTCPSocket();
		return 501;
	}
}

bool GSMInterface::sendRawPacket(String packet) 
{	
	int packetSize = packet.length()-1;
	serialinterface->printToGSM("AT+QISEND=" + String(packetSize));
    serialinterface->printToGSM('\r');
	delay(100);
	Serial.println(packet);
	serialinterface->printToGSM(packet);
	delay(100);	
	
	if(serialinterface->pollForResponseFromCommand("", "OK", false))
	{
		return true;
	}
	else
	{
		closeTCPSocket();
		return false;
	}
}

bool GSMInterface::registerGPRS_GSM()
{
    //revisted look good for Quectel M10
	serialinterface->printlnToDebug("[GPRS Attaching]");	
	return serialinterface->pollForResponseFromCommand("AT+CGATT?", "+CGATT: 1", true); // attach gprs
}

bool GSMInterface::attachGPRSContinue(String apn,String user, String Password)
{
    if(!serialinterface->pollForResponseFromCommand("AT+QIFGCNT=0","OK",false)) // Select a Context as Foreground Context
    {
         serialinterface->printlnToDebug("[QIFGCNT failed]");
         return false;
    }
    
    //if(!serialinterface->pollForResponseFromCommand("AT+QICSGP=1,\"" + apn+ _command_SEPARATOR + user + _command_SEPARATOR+ Password + "\"\r", "OK", false)) //Select  GPRS as the Bearer
    
    if(!serialinterface->pollForResponseFromCommand("AT+QICSGP=1,\"" + apn+ "\""+","+"\"" + user +  "\""+","+"\"" + Password + "\"\r", "OK", false)) //Select  GPRS as the Bearer
    {
            serialinterface->printlnToDebug("[QICSGP failed]");
    		return false;
    }
    if(!serialinterface->pollForResponseFromCommand("AT+QIMUX=0", "OK", false)) //single tcp connection
    {
        serialinterface->printlnToDebug("[QIMUX failed]");
        return false;
    }
    if(!serialinterface->pollForResponseFromCommand("AT+QIMODE=0", "OK", false)) //Normal mode, data will be sent through QISEND commands
    {
        serialinterface->printlnToDebug("[QIMODE failed]");
        return false;
    }
    if(!serialinterface->pollForResponseFromCommand("AT+QINDI=0", "OK", false)) //Method to Handle Received TCP/IP Data
    {
        serialinterface->printlnToDebug("[QINDI failed]");
        return false;
    }
   // serialinterface->printlnToGSM("AT+QIREGAPP?");
    if(!serialinterface->pollForResponseFromCommand("AT+QIREGAPP", "OK", false)) //Start TCP/IP Task
    {
        serialinterface->printlnToDebug("[QIREGAPP failed]");
        return false;
    }
    if(!serialinterface->pollForResponseFromCommand("AT+QIACT", "OK", false)) //Activate GPRS Context
    {
        serialinterface->printlnToDebug("[QIACT failed]");
        return false;
    }
    
    return true;
}

bool GSMInterface::openTCPSocket(String server, int port)
{
	delay(200);
    serialinterface->printlnToDebug("[Opening Socket]");
    
    if(!serialinterface->pollForResponseFromCommand("AT+QIDNSIP=0", "OK", false)) // TODO use IfIPAddress to determine right paramter
    {
        serialinterface->printlnToDebug("[QIDNSIP failed]");
        return false;
    }
	if(!serialinterface->pollForResponseFromCommand("AT+QIOPEN=\"TCP\",\"" + server + "\"," + port+ "\r", "OK", false))
    {
        serialinterface->printlnToDebug("[QIOPEN failed]");
        return false;
    }
    serialinterface->printlnToGSM("AT+QISTAT");
    return true;
	
}
bool GSMInterface::IfIPAddress(String server)
{
    //TODO need to check if server address is IP address or hostname
    return true;
}
void GSMInterface::closeTCPSocket()
{
	serialinterface->printlnToGSM("AT+QICLOSE");
	delay(200);
	serialinterface->printlnToDebug("[TCPSocketClosed]");	
}

void GSMInterface::resetGSM(byte _gsmResetPin)
{
    serialinterface->flushGSM();
    serialinterface->printlnToDebug("Starting Reset");
    pinMode(_gsmResetPin, OUTPUT);
    digitalWrite(_gsmResetPin, HIGH);
    delay(12000);
    digitalWrite(_gsmResetPin, LOW);
    delay(1000);
    Serial.println("End Reset");

}
