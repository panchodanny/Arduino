/*
  Web client

 This sketch connects to a website through a GSM shield. Specifically,
 this example downloads the URL "http://arduino.cc/asciilogo.txt" and
 prints it to the Serial monitor.

 Circuit:
 * GSM shield attached to an Arduino
 * SIM card with a data plan
 
 ATTENTION: to work correctly with M0/M0 pro you have to connect pin 2 to pin 4 of the shield

 created 8 Mar 2012
 by Tom Igoe

 http://arduino.cc/en/Tutorial/GSMExamplesWebClient

 */

// libraries
#include <GSM.h>

// PIN Number
#define PINNUMBER ""

// APN data
#define GPRS_APN       "GPRS_APN" // replace your GPRS APN
#define GPRS_LOGIN     "login"    // replace with your GPRS login
#define GPRS_PASSWORD  "password" // replace with your GPRS password

// initialize the library instance
GSMClient client;
GPRS gprs;
GSM gsmAccess;

// URL, path & port (for example: arduino.cc)
char server[] = "arduino.cc";
char path[] = "/asciilogo.txt";
int port = 80; // port 80 is the default for HTTP

void setup()
{
  // initialize serial communications and wait for port to open:
  SerialUSB.begin(9600);
  while (!SerialUSB) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  SerialUSB.println("Starting Arduino web client.");
  // connection state
  boolean notConnected = true;

  // After starting the modem with GSM.begin()
  // attach the shield to the GPRS network with the APN, login and password
  while (notConnected)
  {
    if ((gsmAccess.begin(PINNUMBER) == GSM_READY) &
        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY))
      notConnected = false;
    else
    {
      SerialUSB.println("Not connected");
      delay(1000);
    }
  }

  SerialUSB.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, port))
  {
    SerialUSB.println("connected");
    // Make a HTTP request:
    client.print("GET ");
    client.print(path);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();
  }
  else
  {
    // if you didn't get a connection to the server:
    SerialUSB.println("connection failed");
  }
}

void loop()
{
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available())
  {
    char c = client.read();
    SerialUSB.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.available() && !client.connected())
  {
    SerialUSB.println();
    SerialUSB.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    for (;;)
      ;
  }
}
