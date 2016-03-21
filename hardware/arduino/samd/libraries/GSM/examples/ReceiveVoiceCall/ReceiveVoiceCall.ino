/*
 Receive Voice Call

 This sketch, for the Arduino GSM shield, receives voice calls,
 displays the calling number, waits a few seconds then hangs up.

 Circuit:
 * GSM shield
 * Voice circuit. Refer to to the GSM shield getting started guide
   at http://www.arduino.org/products/shields/5-arduino-shields/arduino-gsm-shield-2
 * SIM card that can accept voice calls
 
 ATTENTION: to work correctly with M0/M0 pro you have to connect pin 2 to pin 4 of the shield

 With no voice circuit the call will connect, but will not send or receive sound

 created Mar 2012
 by Javier Zorzano

 This example is in the public domain.

 http://arduino.cc/en/Tutorial/GSMExamplesReceiveVoiceCall

 */

// Include the GSM library
#include <GSM.h>

// PIN Number
#define PINNUMBER ""

// initialize the library instance
GSM gsmAccess;
GSMVoiceCall vcs;

// Array to hold the number for the incoming call
char numtel[20];

void setup()
{
  // initialize serial communications and wait for port to open:
  SerialUSB.begin(9600);
  while (!SerialUSB) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  SerialUSB.println("Receive Voice Call");

  // connection state
  boolean notConnected = true;

  // Start GSM shield
  // If your SIM has PIN, pass it as a parameter of begin() in quotes
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

  // This makes sure the modem correctly reports incoming events
  vcs.hangCall();

  SerialUSB.println("Waiting for a call");
}

void loop()
{
  // Check the status of the voice call
  switch (vcs.getvoiceCallStatus())
  {
    case IDLE_CALL: // Nothing is happening

      break;

    case RECEIVINGCALL: // Yes! Someone is calling us

      SerialUSB.println("RECEIVING CALL");

      // Retrieve the calling number
      vcs.retrieveCallingNumber(numtel, 20);

      // Print the calling number
      SerialUSB.print("Number:");
      SerialUSB.println(numtel);

      // Answer the call, establish the call
      vcs.answerCall();
      break;

    case TALKING:  // In this case the call would be established

      SerialUSB.println("TALKING. Press enter to hang up.");
      while (SerialUSB.read() != '\n')
        delay(100);
      vcs.hangCall();
      SerialUSB.println("Hanging up and waiting for the next call.");
      break;
  }
  delay(1000);
}


