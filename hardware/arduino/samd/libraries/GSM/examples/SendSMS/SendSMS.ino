/*
 SMS sender

 This sketch, for the Arduino GSM shield,sends an SMS message
 you enter in the serial monitor. Connect your Arduino with the
 GSM shield and SIM card, open the serial monitor, and wait for
 the "READY" message to appear in the monitor. Next, type a
 message to send and press "return". Make sure the serial
 monitor is set to send a newline when you press return.

 Circuit:
 * GSM shield
 * SIM card that can send SMS
 
 ATTENTION: to work correctly with M0/M0 pro you have to connect pin 2 to pin 4 of the shield

 created 25 Feb 2012
 by Tom Igoe

 This example is in the public domain.

 http://arduino.cc/en/Tutorial/GSMExamplesSendSMS
 

 */

// Include the GSM library
#include <GSM.h>

#define PINNUMBER ""

// initialize the library instance
GSM gsmAccess;
GSM_SMS sms;

void setup()
{
  // initialize serial communications and wait for port to open:
  SerialUSB.begin(9600);
  while (!SerialUSB) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  SerialUSB.println("SMS Messages Sender");

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

  SerialUSB.println("GSM initialized");
}

void loop()
{

  SerialUSB.print("Enter a mobile number: ");
  char remoteNum[20];  // telephone number to send sms
  readSerial(remoteNum);
  SerialUSB.println(remoteNum);

  // sms text
  SerialUSB.print("Now, enter SMS content: ");
  char txtMsg[200];
  readSerial(txtMsg);
  SerialUSB.println("SENDING");
  SerialUSB.println();
  SerialUSB.println("Message:");
  SerialUSB.println(txtMsg);

  // send the message
  sms.beginSMS(remoteNum);
  sms.print(txtMsg);
  sms.endSMS();
  SerialUSB.println("\nCOMPLETE!\n");
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
        SerialUSB.flush();
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
