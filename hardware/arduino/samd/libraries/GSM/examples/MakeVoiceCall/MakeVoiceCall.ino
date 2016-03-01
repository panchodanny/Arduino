/*
 Make Voice Call

 This sketch, for the Arduino GSM shield, puts a voice call to
 a remote phone number that you enter through the serial monitor.
 To make it work, open the serial monitor, and when you see the
 READY message, type a phone number. Make sure the serial monitor
 is set to send a just newline when you press return.

 Circuit:
 * GSM shield
 * Voice circuit.
 With no voice circuit the call will send nor receive any sound
 
 ATTENTION: to work correctly with M0/M0 pro you have to connect pin 2 to pin 4 of the shield


 created Mar 2012
 by Javier Zorzano

 This example is in the public domain.
 */

// libraries
#include <GSM.h>

// PIN Number
#define PINNUMBER ""

// initialize the library instance
GSM gsmAccess; // include a 'true' parameter for debug enabled
GSMVoiceCall vcs;

String remoteNumber = "";  // the number you will call
char charbuffer[20];

void setup()
{

  // initialize serial communications and wait for port to open:
  SerialUSB.begin(9600);
  while (!SerialUSB) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  SerialUSB.println("Make Voice Call");

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

  SerialUSB.println("GSM initialized.");
  SerialUSB.println("Enter phone number to call.");

}

void loop()
{

  // add any incoming characters to the String:
  while (SerialUSB.available() > 0)
  {
    char inChar = SerialUSB.read();
    // if it's a newline, that means you should make the call:
    if (inChar == '\n')
    {
      // make sure the phone number is not too long:
      if (remoteNumber.length() < 20)
      {
        // let the user know you're calling:
        SerialUSB.print("Calling to : ");
        SerialUSB.println(remoteNumber);
        SerialUSB.println();

        // Call the remote number
        remoteNumber.toCharArray(charbuffer, 20);


        // Check if the receiving end has picked up the call
        if (vcs.voiceCall(charbuffer))
        {
          SerialUSB.println("Call Established. Enter line to end");
          // Wait for some input from the line
          while (SerialUSB.read() != '\n' && (vcs.getvoiceCallStatus() == TALKING));
          // And hang up
          vcs.hangCall();
        }
        SerialUSB.println("Call Finished");
        remoteNumber = "";
        SerialUSB.println("Enter phone number to call.");
      }
      else
      {
        SerialUSB.println("That's too long for a phone number. I'm forgetting it");
        remoteNumber = "";
      }
    }
    else
    {
      // add the latest character to the message to send:
      if (inChar != '\r')
        remoteNumber += inChar;
    }
  }
}

