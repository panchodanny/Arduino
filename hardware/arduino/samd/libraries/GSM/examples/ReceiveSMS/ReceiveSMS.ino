/*
 SMS receiver

 This sketch, for the Arduino GSM shield, waits for a SMS message
 and displays it through the Serial port.

 Circuit:
 * GSM shield attached to and Arduino
 * SIM card that can receive SMS messages
 
 ATTENTION: to work correctly with M0/M0 pro you have to connect pin 2 to pin 4 of the shield

 created 25 Feb 2012
 by Javier Zorzano / TD

 This example is in the public domain.

 http://arduino.cc/en/Tutorial/GSMExamplesReceiveSMS

*/

// include the GSM library
#include <GSM.h>

// PIN Number for the SIM
#define PINNUMBER ""

// initialize the library instances
GSM gsmAccess;
GSM_SMS sms;

// Array to hold the number a SMS is retreived from
char senderNumber[20];

void setup()
{
  // initialize serial communications and wait for port to open:
  SerialUSB.begin(9600);
  while (!SerialUSB) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  SerialUSB.println("SMS Messages Receiver");

  // connection state
  boolean notConnected = true;

  // Start GSM connection
  while (notConnected)
  {
    if (gsmAccess.begin(PINNUMBER) == GSM_READY)
      notConnected = false;
    else
    {
      SerialUSB.println("Not connected");
      delay(1000);
    }
  }

  SerialUSB.println("GSM initialized");
  SerialUSB.println("Waiting for messages");
}

void loop()
{
  char c;

  // If there are any SMSs available()
  if (sms.available())
  {
    SerialUSB.println("Message received from:");

    // Get remote number
    sms.remoteNumber(senderNumber, 20);
    SerialUSB.println(senderNumber);

    // An example of message disposal
    // Any messages starting with # should be discarded
    if (sms.peek() == '#')
    {
      SerialUSB.println("Discarded SMS");
      sms.flush();
    }

    // Read message bytes and print them
    while (c = sms.read())
      SerialUSB.print(c);

    SerialUSB.println("\nEND OF MESSAGE");

    // Delete message from modem memory
    sms.flush();
    SerialUSB.println("MESSAGE DELETED");
  }

  delay(1000);

}


