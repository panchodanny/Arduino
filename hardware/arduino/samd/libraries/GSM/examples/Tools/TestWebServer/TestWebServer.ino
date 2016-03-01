/*
  Basic Web Server

 A simple web server that replies with nothing, but prints the client's request
 and the server IP address.

 Circuit:
 * GSM shield attached
 
 ATTENTION: to work correctly with M0/M0 pro you have to connect pin 2 to pin 4 of the shield

 created
 by David Cuartielles
 modified 21 Nov 2012
 by Tom Igoe

 http://arduino.cc/en/Tutorial/GSMToolsTestWebServer

 This example code is part of the public domain
 */
#include <GSM.h>

// PIN Number
#define PINNUMBER ""

// APN data
#define GPRS_APN       "GPRS_APN" // replace your GPRS APN
#define GPRS_LOGIN     "login"    // replace with your GPRS login
#define GPRS_PASSWORD  "password" // replace with your GPRS password


// initialize the library instance
GPRS gprs;
GSM gsmAccess;     // include a 'true' parameter for debug enabled
GSMServer server(80); // port 80 (http default)

// timeout
const unsigned long __TIMEOUT__ = 10 * 1000;

void setup()
{
  // initialize serial communications and wait for port to open:
  SerialUSB.begin(9600);
  while (!SerialUSB) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  SerialUSB.println("starting,..");
  // connection state
  boolean connected = true;

  // Start GSM shield
  // If your SIM has PIN, pass it as a parameter of begin() in quotes
  while (!connected)
  {
    if ((gsmAccess.begin(PINNUMBER) == GSM_READY) &
        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY))
      connected = true;
    else
    {
      SerialUSB.println("Not connected");
      delay(1000);
    }
  }

  SerialUSB.println("Connected to GPRS network");

  // start server
  server.begin();

  //Get IP.
  IPAddress LocalIP = gprs.getIPAddress();
  SerialUSB.println("Server IP address=");
  SerialUSB.println(LocalIP);
}

void loop() {
  GSMClient client = server.available();

  if (client) {
    if (client.available()) {
      SerialUSB.write(client.read());
    }
  }

}

