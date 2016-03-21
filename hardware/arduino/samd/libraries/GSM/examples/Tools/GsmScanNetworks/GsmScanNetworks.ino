/*

 GSM Scan Networks

 This example prints out the IMEI number of the modem,
 then checks to see if it's connected to a carrier. If so,
 it prints the phone number associated with the card.
 Then it scans for nearby networks and prints out their signal strengths.

 Circuit:
 * GSM shield
 * SIM card

 ATTENTION: to work correctly with M0/M0 pro you have to connect pin 2 to pin 4 of the shield
 
 Created 8 Mar 2012
 by Tom Igoe, implemented by Javier Carazo
 Modified 4 Feb 2013
 by Scott Fitzgerald

 http://arduino.cc/en/Tutorial/GSMToolsGsmScanNetworks

 This example code is part of the public domain
 */

// libraries
#include <GSM.h>

// PIN Number
#define PINNUMBER ""

// initialize the library instance
GSM gsmAccess;     // include a 'true' parameter to enable debugging
GSMScanner scannerNetworks;
GSMModem modemTest;

// Save data variables
String IMEI = "";

// serial monitor result messages
String errortext = "ERROR";

void setup()
{
  // initialize serial communications and wait for port to open:
  SerialUSB.begin(9600);
  while (!SerialUSB) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  SerialUSB.println("GSM networks scanner");
  scannerNetworks.begin();

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

  // get modem parameters
  // IMEI, modem unique identifier
  SerialUSB.print("Modem IMEI: ");
  IMEI = modemTest.getIMEI();
  IMEI.replace("\n", "");
  if (IMEI != NULL)
    SerialUSB.println(IMEI);
}

void loop()
{
  // scan for existing networks, displays a list of networks
  SerialUSB.println("Scanning available networks. May take some seconds.");
  SerialUSB.println(scannerNetworks.readNetworks());

  // currently connected carrier
  SerialUSB.print("Current carrier: ");
  SerialUSB.println(scannerNetworks.getCurrentCarrier());

  // returns strength and ber
  // signal strength in 0-31 scale. 31 means power > 51dBm
  // BER is the Bit Error Rate. 0-7 scale. 99=not detectable
  SerialUSB.print("Signal Strength: ");
  SerialUSB.print(scannerNetworks.getSignalStrength());
  SerialUSB.println(" [0-31]");

}

