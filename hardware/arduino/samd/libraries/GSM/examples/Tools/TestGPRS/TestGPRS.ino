/*

 This sketch test the GSM shield's ability to connect to a
 GPERS network. It asks for APN information through the
 serial monitor and tries to connect to arduino.cc.

 Circuit:
 * GSM shield attached
 * SIM card with data plan

 ATTENTION: to work correctly with M0/M0 pro you have to connect pin 2 to pin 4 of the shield
 
 Created 18 Jun 2012
 by David del Peral

 This example code is part of the public domain

 http://arduino.cc/en/Tutorial/GSMToolsTestGPRS

 */

// libraries
#include <GSM.h>

// PIN Number
#define PINNUMBER ""

// initialize the library instance
GSM gsmAccess;        // GSM access: include a 'true' parameter for debug enabled
GPRS gprsAccess;  // GPRS access
GSMClient client;  // Client service for TCP connection

// messages for serial monitor response
String oktext = "OK";
String errortext = "ERROR";

// URL and path (for example: arduino.cc)
char url[] = "arduino.cc";
char urlproxy[] = "http://arduino.cc";
char path[] = "/";

// variable for save response obtained
String response = "";

// use a proxy
boolean use_proxy = false;

void setup()
{
  // initialize serial communications and wait for port to open:
  SerialUSB.begin(9600);
  while (!SerialUSB) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
}

void loop()
{
  use_proxy = false;

  // start GSM shield
  // if your SIM has PIN, pass it as a parameter of begin() in quotes
  SerialUSB.print("Connecting GSM network...");
  if (gsmAccess.begin(PINNUMBER) != GSM_READY)
  {
    SerialUSB.println(errortext);
    while (true);
  }
  SerialUSB.println(oktext);

  // read APN introduced by user
  char apn[50];
  SerialUSB.print("Enter your APN: ");
  readSerial(apn);
  SerialUSB.println(apn);

  // Read APN login introduced by user
  char login[50];
  SerialUSB.print("Now, enter your login: ");
  readSerial(login);
  SerialUSB.println(login);

  // read APN password introduced by user
  char password[20];
  SerialUSB.print("Finally, enter your password: ");
  readSerial(password);

  // attach GPRS
  SerialUSB.println("Attaching to GPRS with your APN...");
  if (gprsAccess.attachGPRS(apn, login, password) != GPRS_READY)
  {
    SerialUSB.println(errortext);
  }
  else {

    SerialUSB.println(oktext);

    // read proxy introduced by user
    char proxy[100];
    SerialUSB.print("If your carrier uses a proxy, enter it, if not press enter: ");
    readSerial(proxy);
    SerialUSB.println(proxy);

    // if user introduced a proxy, asks him for proxy port
    int pport;
    if (proxy[0] != '\0') {
      // read proxy port introduced by user
      char proxyport[10];
      SerialUSB.print("Enter the proxy port: ");
      readSerial(proxyport);
      // cast proxy port introduced to integer
      pport = (int) proxyport;
      use_proxy = true;
      SerialUSB.println(proxyport);
    }

    // connection with arduino.cc and realize HTTP request
    SerialUSB.print("Connecting and sending GET request to arduino.cc...");
    int res_connect;

    // if use a proxy, connect with it
    if (use_proxy)
      res_connect = client.connect(proxy, pport);
    else
      res_connect = client.connect(url, 80);

    if (res_connect)
    {
      // make a HTTP 1.0 GET request (client sends the request)
      client.print("GET ");

      // if use a proxy, the path is arduino.cc URL
      if (use_proxy)
        client.print(urlproxy);
      else
        client.print(path);

      client.println(" HTTP/1.0");
      client.println();
      SerialUSB.println(oktext);
    }
    else
    {
      // if you didn't get a connection to the server
      SerialUSB.println(errortext);
    }
    SerialUSB.print("Receiving response...");

    boolean test = true;
    while (test)
    {
      // if there are incoming bytes available
      // from the server, read and check them
      if (client.available())
      {
        char c = client.read();
        response += c;

        // cast response obtained from string to char array
        char responsechar[response.length() + 1];
        response.toCharArray(responsechar, response.length() + 1);

        // if response includes a "200 OK" substring
        if (strstr(responsechar, "200 OK") != NULL) {
          SerialUSB.println(oktext);
          SerialUSB.println("TEST COMPLETE!");
          test = false;
        }
      }

      // if the server's disconnected, stop the client:
      if (!client.connected())
      {
        SerialUSB.println();
        SerialUSB.println("disconnecting.");
        client.stop();
        test = false;
      }
    }
  }
}

/*
  Read input serial
 */
int readSerial(char result[])
{
  int i = 0;
  while (1)
  {
    while (SerialUSB.available() > 0)
    {
      char inChar = SerialUSB.read();
      if (inChar == '\n')
      {
        result[i] = '\0';
        return 0;
      }
      if (inChar != '\r')
      {
        result[i] = inChar;
        i++;
      }
    }
  }
}
