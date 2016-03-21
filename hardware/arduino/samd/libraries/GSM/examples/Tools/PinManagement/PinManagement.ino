/*

 This example enables you to change or remove the PIN number of
 a SIM card inserted into a GSM shield.

 Circuit:
 * GSM shield
 * SIM card
 
 ATTENTION: to work correctly with M0/M0 pro you have to connect pin 2 to pin 4 of the shield

 Created 12 Jun 2012
 by David del Peral

 This example code is part of the public domain

 http://arduino.cc/en/Tutorial/GSMToolsPinManagement

 */

// libraries
#include <GSM.h>

// pin manager object
GSMPIN PINManager;

// save input in serial by user
String user_input = "";

// authenticated with PIN code
boolean auth = false;

// serial monitor result messages
String oktext = "OK";
String errortext = "ERROR";

void setup()
{
  // initialize serial communications and wait for port to open:
  SerialUSB.begin(9600);
  while (!SerialUSB) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  SerialUSB.println("Change PIN example\n");
  PINManager.begin();

  // check if the SIM have pin lock
  while (!auth) {
    int pin_query = PINManager.isPIN();
    if (pin_query == 1)
    {
      // if SIM is locked, enter PIN code
      SerialUSB.print("Enter PIN code: ");
      user_input = readSerial();
      // check PIN code
      if (PINManager.checkPIN(user_input) == 0)
      {
        auth = true;
        PINManager.setPINUsed(true);
        SerialUSB.println(oktext);
      }
      else
      {
        // if PIN code was incorrected
        SerialUSB.println("Incorrect PIN. Remember that you have 3 opportunities.");
      }
    }
    else if (pin_query == -1)
    {
      // PIN code is locked, user must enter PUK code
      SerialUSB.println("PIN locked. Enter PUK code: ");
      String puk = readSerial();
      SerialUSB.print("Now, enter a new PIN code: ");
      user_input = readSerial();
      // check PUK code
      if (PINManager.checkPUK(puk, user_input) == 0)
      {
        auth = true;
        PINManager.setPINUsed(true);
        SerialUSB.println(oktext);
      }
      else
      {
        // if PUK o the new PIN are incorrect
        SerialUSB.println("Incorrect PUK or invalid new PIN. Try again!.");
      }
    }
    else if (pin_query == -2)
    {
      // the worst case, PIN and PUK are locked
      SerialUSB.println("PIN & PUK locked. Use PIN2/PUK2 in a mobile phone.");
      while (true);
    }
    else
    {
      // SIM does not requires authetication
      SerialUSB.println("No pin necessary.");
      auth = true;
    }
  }

  // start GSM shield
  SerialUSB.print("Checking register in GSM network...");
  if (PINManager.checkReg() == 0)
    SerialUSB.println(oktext);
  // if you are connect by roaming
  else if (PINManager.checkReg() == 1)
    SerialUSB.println("ROAMING " + oktext);
  else
  {
    // error connection
    SerialUSB.println(errortext);
    while (true);
  }
}

void loop()
{
  // Function loop implements pin management user menu
  // Only if you SIM use pin lock, you can change PIN code
  // user_op variables save user option

  SerialUSB.println("Choose an option:\n1 - On/Off PIN.");
  if (PINManager.getPINUsed())
    SerialUSB.println("2 - Change PIN.");
  String user_op = readSerial();
  if (user_op == "1")
  {
    SerialUSB.println("Enter your PIN code:");
    user_input = readSerial();
    // activate/deactivate PIN lock
    PINManager.switchPIN(user_input);
  }
  else if (user_op == "2" & PINManager.getPINUsed())
  {
    SerialUSB.println("Enter your actual PIN code:");
    String oldPIN = readSerial();
    SerialUSB.println("Now, enter your new PIN code:");
    String newPIN = readSerial();
    // change PIN
    PINManager.changePIN(oldPIN, newPIN);
  }
  else
  {
    SerialUSB.println("Incorrect option. Try again!.");
  }
  delay(1000);
}

/*
  Read input serial
 */
String readSerial()
{
  String text = "";
  while (1)
  {
    while (SerialUSB.available() > 0)
    {
      char inChar = SerialUSB.read();
      if (inChar == '\n')
      {
        return text;
      }
      if (inChar != '\r')
        text += inChar;
    }
  }
}
