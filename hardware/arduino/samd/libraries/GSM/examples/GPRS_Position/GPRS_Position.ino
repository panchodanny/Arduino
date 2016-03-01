/*
 GPRS_Position

 A simple Sketch that shows the Longitude  and Latitude
 using a GSM shield.

 Circuit: 
 GSM shield attached
 
 ATTENTION: to work correctly with M0/M0 pro you have to connect pin 2 to pin 4 of the shield

 Notes: 
 You have to change the apn, login and password.
 If your SIM has PIN you have to set 
 
 created 3 Apr. 2015
 by Arduino.org team (http://arduino.org)
 */

// Library include
#include <GSM.h>
// SIM and Network constant
#define PINNUMBER  ""         // if present, insert Your PIN here
#define apn "internet.wind"   // change the apn
//#define apn "ibox.tim.it"
//#define apn "unico.tim.it"
//#define apn "wap.tim"
#define login ""              // insert your login
#define password ""           // isnert your password

// initialize the library instance
GSM gsmAccess;            // GSM access: include a 'true' parameter for debug enabled
GPRS gprsAccess;          // GPRS access
GSMClient client;         // Client service for TCP connection
GSM2 gprsPosition;        // GPRS Posistioning system



void setup() {
  // put your setup code here, to run once:
  SerialUSB.begin(9600);
  
  // start GSM shield
 
  SerialUSB.println(" ");
  SerialUSB.print("Connecting GSM network...");
  if (gsmAccess.begin(PINNUMBER) != GSM_READY)
  {
    SerialUSB.println("Error !");
    while (true);
  }
  SerialUSB.println("Ok !");
  SerialUSB.println("Setting APN...");
  gprsAccess.attachGPRS(apn, login, password);
  SerialUSB.print("APN :");
  SerialUSB.println(apn);
  SerialUSB.print("Login :");
  SerialUSB.println(login);
  SerialUSB.print("Password :");
  SerialUSB.println(password);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  SerialUSB.print("Position: ");
  SerialUSB.println(gprsPosition.getPosition());

}
