/*

 This example tests to see if the modem of the
 GSM shield is working correctly. You do not need
 a SIM card for this example.

 Circuit:
 * GSM shield attached
 
 ATTENTION: to work correctly with M0/M0 pro you have to connect pin 2 to pin 4 of the shield

 Created 12 Jun 2012
 by David del Peral
 modified 21 Nov 2012
 by Tom Igoe

 http://arduino.cc/en/Tutorial/GSMToolsTestModem

 This sample code is part of the public domain

 */

// libraries
#include <GSM.h>

// modem verification object
GSMModem modem;

// IMEI variable
String IMEI = "";

void setup()
{
  // initialize serial communications and wait for port to open:
  SerialUSB.begin(9600);
  while (!SerialUSB) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // start modem test (reset and check response)
  SerialUSB.print("Starting modem test...");
  if (modem.begin())
    SerialUSB.println("modem.begin() succeeded");
  else
    SerialUSB.println("ERROR, no modem answer.");
}

void loop()
{
  // get modem IMEI
  SerialUSB.print("Checking IMEI...");
  IMEI = modem.getIMEI();

  // check IMEI response
  if (IMEI != NULL)
  {
    // show IMEI in serial monitor
    SerialUSB.println("Modem's IMEI: " + IMEI);
    // reset modem to check booting:
    SerialUSB.print("Resetting modem...");
    modem.begin();
    // get and check IMEI one more time
    if (modem.getIMEI() != NULL)
    {
      SerialUSB.println("Modem is functoning properly");
    }
    else
    {
      SerialUSB.println("Error: getIMEI() failed after modem.begin()");
    }
  }
  else
  {
    SerialUSB.println("Error: Could not get IMEI");
  }
  // do nothing:
  while (true);
}

