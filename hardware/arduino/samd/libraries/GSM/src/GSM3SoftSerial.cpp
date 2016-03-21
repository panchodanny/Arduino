/*
This file is part of the GSM3 communications library for Arduino
-- Multi-transport communications platform
-- Fully asynchronous
-- Includes code for the Arduino-Telefonica GSM/GPRS Shield V1
-- Voice calls
-- SMS
-- TCP/IP connections
-- HTTP basic clients

This library has been developed by Telefónica Digital - PDI -
- Physical Internet Lab, as part as its collaboration with
Arduino and the Open Hardware Community. 

September-December 2012

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

The latest version of this library can always be found at
https://github.com/BlueVia/Official-Arduino
*/
#include "GSM3SoftSerial.h"
#include <WInterrupts.h>
#include <HardwareSerial.h>
#include <Arduino.h>

#define __XON__ 0x11
#define __XOFF__ 0x13

#define _GSMSOFTSERIALFLAGS_ESCAPED_ 0x01
#define _GSMSOFTSERIALFLAGS_SENTXOFF_ 0x02

//
// Lookup table
//
#define __PARAGRAPHGUARD__ 50
typedef struct _DELAY_TABLE
{
  long baud;
  unsigned short rx_delay_centering;
  unsigned short rx_delay_intrabit;
  unsigned short rx_delay_stopbit;
  unsigned short tx_delay;
} DELAY_TABLE;


static const DELAY_TABLE PROGMEM table[] = 
{
  //  baud    rxcenter   rxintra    rxstop    tx
  { 115200,   2,         6,         8,        8,     },
  { 57600,    6,         15,        17,       17,    },
  { 38400,    11,        24,        26,       26,    },
  { 31250,    14,        30,        32,       32,    },
  { 28800,    15,        32,        34,       34,    },
  { 19200,    24,        50,        52,       52,    },
  { 14400,    32,        67,        69,       69,    },
  { 9600,     50,        102,       104,      104,   },
  { 4800,     102,       206,       208,      208,   },
  { 2400,     206,       414,       416,      416,   },
  { 1200,     414,       831,       833,      833,   },
  { 300,      1664,      3331,      3333,     3333,  },
};

//const int XMIT_START_ADJUSTMENT = 5;


GSM3SoftSerial* GSM3SoftSerial::_activeObject=0;

GSM3SoftSerial::GSM3SoftSerial():
	_rx_delay_centering(0),
	_rx_delay_intrabit(0),
	_rx_delay_stopbit(0),
	_tx_delay(0),
	cb(this)
{
	
	//comStatus=0;
	//waitingAnswer=false;
}

int GSM3SoftSerial::begin(long speed)
{
  _rx_delay_centering = _rx_delay_intrabit = _rx_delay_stopbit = _tx_delay = 0;
        setTX();
	setRX();
  for (unsigned i=0; i<sizeof(table)/sizeof(table[0]); ++i)
  {
    long baud = pgm_read_dword(&table[i].baud);
    if (baud == speed)
    {
      _rx_delay_centering = pgm_read_word(&table[i].rx_delay_centering);
      _rx_delay_intrabit = pgm_read_word(&table[i].rx_delay_intrabit);
      _rx_delay_stopbit = pgm_read_word(&table[i].rx_delay_stopbit);
      _tx_delay = pgm_read_word(&table[i].tx_delay);
      break;
    }
  }
  
  if (_rx_delay_stopbit)
  {
    if (digitalPinToInterrupt(4))
    {
     attachInterrupt(4, handle_interrupt, FALLING);
    }
    delayMicroseconds(_tx_delay); // if we were low this establishes the end
  }
  
  _activeObject=this;

}

void GSM3SoftSerial::close()
 {
	_activeObject=0;
 }

size_t GSM3SoftSerial::write(uint8_t c)
{
	if (_tx_delay == 0)
		return 0;

	// Characters to be escaped under XON/XOFF control with Quectel
	if(c==0x11)
	{
		this->finalWrite(0x77);
		return this->finalWrite(0xEE);
	}

	if(c==0x13)
	{
		this->finalWrite(0x77);
		return this->finalWrite(0xEC);
	}

	if(c==0x77)
	{
		this->finalWrite(0x77);
		return this->finalWrite(0x88);
	}
	
	return this->finalWrite(c);
}

size_t GSM3SoftSerial::finalWrite(uint8_t c)
{
	
        // turn off interrupts for a clean txmit
       EIC->INTENCLR.reg = EIC_INTENCLR_EXTINT( 1 << digitalPinToInterrupt( 4 ));


	// Write the start bit
	tx_pin_write(LOW);
	delayMicroseconds(_tx_delay);// + XMIT_START_ADJUSTMENT);

	// Write each of the 8 bits
	for (byte mask = 0x01; mask; mask <<= 1)
	{
		if (c & mask) // choose bit
			tx_pin_write(HIGH); // send 1
		else
			tx_pin_write(LOW); // send 0
		delayMicroseconds(_tx_delay);
	}

	tx_pin_write(HIGH); // restore pin to natural state
	
	// turn interrupts back on
        EIC->INTENSET.reg = EIC_INTENSET_EXTINT( 1 << digitalPinToInterrupt( 4 ));
	delayMicroseconds(_tx_delay);
				
	return 1;
}

  /*inline*/ void GSM3SoftSerial::tunedDelay(uint16_t delay) { 
	__asm__ __volatile__(
    "1:              \n"
    "   sub %0, #1   \n" // substract 1 from %0 (n)
    "   bne 1b       \n" // if result is not 0 jump to 1
    : "+r" (delay)           // '%0' is n variable with RW constraints
    :                    // no input
    :                    // no clobber
  );
  }

void GSM3SoftSerial::tx_pin_write(uint8_t pin_state)
{
  // Direct port manipulation is faster than digitalWrite/Read
  if (pin_state == LOW)
    _transmitPortRegister->reg &= ~_transmitBitMask;
  else
    _transmitPortRegister->reg |= _transmitBitMask;
}

void GSM3SoftSerial::setTX()
{
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);
  // For digital port direct manipulation
  _transmitBitMask = digitalPinToBitMask(3);
  PortGroup * port = digitalPinToPort(3);
  _transmitPortRegister = portOutputRegister(port);
}

void GSM3SoftSerial::setRX()
{
  pinMode(4, INPUT);
  digitalWrite(4, HIGH);  // pullup for normal logic!
  // For digital port direct manipulation
  _receiveBitMask = digitalPinToBitMask(4);
  PortGroup * port = digitalPinToPort(4);
  _receivePortRegister = portInputRegister(port);
}

void GSM3SoftSerial::handle_interrupt()
{
	if(_activeObject)
		_activeObject->recv();
}

uint32_t GSM3SoftSerial::rx_pin_read()
{
  // Digital port manipulation
  return _receivePortRegister->reg & digitalPinToBitMask(4);
}

void GSM3SoftSerial::recv()
{

  bool firstByte=true;
  byte thisHead;
  
  uint8_t d = 0;
  bool morebytes=false;
  //bool fullbuffer=(cb.availableBytes()<3);
  bool fullbuffer;
  bool capturado_fullbuffer = 0;
  int i;
  byte oldTail;

  // If RX line is high, then we don't see any start bit
  // so interrupt is probably not for us
  if (!rx_pin_read())
  {
 	do
	{
		oldTail=cb.getTail();
		// Wait approximately 1/2 of a bit width to "center" the sample
		delayMicroseconds(_rx_delay_centering);
		
		fullbuffer=(cb.availableBytes()<6);

		
		if(fullbuffer&&(!capturado_fullbuffer))
			tx_pin_write(LOW);

		
		// Read each of the 8 bits
		for (uint8_t i=0x1; i; i <<= 1)
		{
			delayMicroseconds(_rx_delay_intrabit);
			uint8_t noti = ~i;
			if (rx_pin_read())
				d |= i;
			else // else clause added to ensure function timing is ~balanced
				d &= noti;
			if(fullbuffer&&(!capturado_fullbuffer))
			{
			  if((uint8_t)__XOFF__ & i)
				tx_pin_write(HIGH);
			  else
				tx_pin_write(LOW);
			}
		}

		if(fullbuffer&&(!capturado_fullbuffer))
		{
			delayMicroseconds(_rx_delay_intrabit);
			tx_pin_write(HIGH);	
		}
		
		// So, we know the buffer is full, and we have sent a XOFF
		if (fullbuffer) 
		{
			capturado_fullbuffer =1;
			_flags |=_GSMSOFTSERIALFLAGS_SENTXOFF_;
		}


		// skip the stop bit
		if (!fullbuffer) delayMicroseconds(_rx_delay_stopbit);		
		if(keepThisChar(&d))
		{
			cb.write(d);
			if(firstByte)
			{
				firstByte=false;
				thisHead=cb.getTail();
			}
		}
		
		
		// This part is new. It is used to detect the end of a "paragraph"
		// Caveat: the old fashion would let processor a bit of time between bytes, 
		// that here is lost
		// This active waiting avoids drifting
		morebytes=false;
		// TO-DO. This PARAGRAPHGUARD is empyric. We should test it for every speed
		for(i=0;i<__PARAGRAPHGUARD__;i++)
		{	
			delayMicroseconds(1);
			if(!rx_pin_read())
			{
				morebytes=true;
				break;
			}
		}
	}while(morebytes);
	// If we find a line feed, we are at the end of a paragraph
	// check!
	
	if (fullbuffer)
	{
		// And... go handle it!
		if(mgr)
			mgr->manageMsg(thisHead, cb.getTail());
	}
	else if(d==10)
	{
		// And... go handle it!
		if(mgr)
			mgr->manageMsg(thisHead, cb.getTail());
	}
	else if (d==32)
	{
		// And... go handle it!
		if(mgr)
			mgr->manageMsg(thisHead, cb.getTail());
	}
  }

}

bool GSM3SoftSerial::keepThisChar(uint8_t* c)
{
	// Horrible things for Quectel XON/XOFF
	// 255 is the answer to a XOFF
	// It comes just once
	if((*c==255)&&(_flags & _GSMSOFTSERIALFLAGS_SENTXOFF_))
	{
		_flags ^= _GSMSOFTSERIALFLAGS_SENTXOFF_;
		return false;
	}

	// 0x77, w, is the escape character
	if(*c==0x77)
	{
		_flags |= _GSMSOFTSERIALFLAGS_ESCAPED_;
		return false;
	}
	
	// and these are the escaped codes
	if(_flags & _GSMSOFTSERIALFLAGS_ESCAPED_)
	{
		if(*c==0xEE)
			*c=0x11;
		else if(*c==0xEC)
			*c=0x13;
		else if(*c==0x88)
			*c=0x77;
			
		_flags ^= _GSMSOFTSERIALFLAGS_ESCAPED_;
		return true;
	}
	
	return true;
}

void GSM3SoftSerial::spaceAvailable()
{
	// If there is spaceAvailable in the buffer, lets send a XON
	finalWrite((byte)__XON__);
}


// This is here to avoid problems with Arduino compiler
void GSM3SoftSerialMgr::manageMsg(byte from, byte to){};
