/* Copyright (c) 2011, Peter Barrett
**
** Permission to use, copy, modify, and/or distribute this software for
** any purpose with or without fee is hereby granted, provided that the
** above copyright notice and this permission notice appear in all copies.
**
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR
** BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
** OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
** WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
** ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
** SOFTWARE.
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

// Include Atmel headers
#include "Arduino.h"
#include "sam.h"
#include "wiring_constants.h"
#include "USBCore.h"
#include "USB/USB_device.h"
#include "USBDesc.h"
#include "USBAPI.h"

#include "Reset.h"


#ifdef CDC_ENABLED

#define CDC_SERIAL_BUFFER_SIZE	63  //prima era 64

/* For information purpose only since RTS is not always handled by the terminal application */
#define CDC_LINESTATE_DTR		0x01 // Data Terminal Ready
#define CDC_LINESTATE_RTS		0x02 // Ready to Send

#define CDC_LINESTATE_READY		(CDC_LINESTATE_RTS | CDC_LINESTATE_DTR)

struct ring_buffer
{
	uint8_t buffer[CDC_SERIAL_BUFFER_SIZE];
	volatile uint32_t head;
	volatile uint32_t tail;
};

ring_buffer cdc_rx_buffer = { { 0 }, 0, 0};

typedef struct
{
	uint32_t	dwDTERate;
	uint8_t		bCharFormat;
	uint8_t 	bParityType;
	uint8_t 	bDataBits;
	uint8_t		lineState;
} LineInfo;

_Pragma("pack(1)")
static volatile LineInfo _usbLineInfo = { 
   115200, // dWDTERate
    0x00,  // bCharFormat
    0x00,  // bParityType
    0x08,  // bDataBits
    0x00   // lineState
};

static const CDCDescriptor _cdcInterface =
{
#if (defined CDC_ENABLED) && defined(HID_ENABLED)
	D_IAD(0, 2, CDC_COMMUNICATION_INTERFACE_CLASS, CDC_ABSTRACT_CONTROL_MODEL, 0),
#endif
	//	CDC communication interface
	D_INTERFACE(CDC_ACM_INTERFACE,1,CDC_COMMUNICATION_INTERFACE_CLASS,CDC_ABSTRACT_CONTROL_MODEL,0),
	D_CDCCS( CDC_HEADER, CDC_V1_10 & 0xFF, (CDC_V1_10>>8) & 0x0FF ),	// Header (1.10 bcd)
	
	D_CDCCS4(CDC_ABSTRACT_CONTROL_MANAGEMENT,6),				// SET_LINE_CODING, GET_LINE_CODING, SET_CONTROL_LINE_STATE supported
	D_CDCCS(CDC_UNION,CDC_ACM_INTERFACE,CDC_DATA_INTERFACE),	// Communication interface is master, data interface is slave 0
	D_CDCCS(CDC_CALL_MANAGEMENT,1,1),							// Device handles call management (not)
	D_ENDPOINT(USB_ENDPOINT_IN (CDC_ENDPOINT_ACM),USB_ENDPOINT_TYPE_INTERRUPT,0x10, 0x10),

	//	CDC data interface
	D_INTERFACE(CDC_DATA_INTERFACE,2,CDC_DATA_INTERFACE_CLASS,0,0),
	D_ENDPOINT(USB_ENDPOINT_OUT(CDC_ENDPOINT_OUT),USB_ENDPOINT_TYPE_BULK,EPX_SIZE,0),
	D_ENDPOINT(USB_ENDPOINT_IN (CDC_ENDPOINT_IN ),USB_ENDPOINT_TYPE_BULK,EPX_SIZE,0)
};
_Pragma("pack()")

const void* WEAK CDC_GetInterface(void)
{
	return  &_cdcInterface;
}

uint32_t WEAK CDC_GetInterfaceLength(void)
{
    return sizeof( _cdcInterface );
}

bool WEAK CDC_Setup(Setup& setup)
{
	uint8_t r = setup.bRequest;
	uint8_t requestType = setup.bmRequestType;

	if (REQUEST_DEVICETOHOST_CLASS_INTERFACE == requestType)
	{
		if (CDC_GET_LINE_CODING == r)
		{
			USBD_SendControl(0,(void*)&_usbLineInfo,7);
			return true;
		}
	}

	if (REQUEST_HOSTTODEVICE_CLASS_INTERFACE == requestType)
	{
		if (CDC_SET_LINE_CODING == r)
		{
			while( UDD_FifoByteCount(EP0) <15);
			//USBD_RecvControl((void*)&_usbLineInfo,7);
			uint8_t* line = (uint8_t*)&_usbLineInfo;
			for(uint8_t i = 0; i<7; i++)
				line[i] = setup.data[i];
			return false;
		}

		if (CDC_SET_CONTROL_LINE_STATE == r)
		{
			_usbLineInfo.lineState = setup.wValueL;
			// auto-reset into the bootloader is triggered when the port, already
			// open at 1200 bps, is closed.
			if (1200 == _usbLineInfo.dwDTERate)
			{
				// We check DTR state to determine if host port is open (bit 0 of lineState).
				if ((_usbLineInfo.lineState & 0x01) == 0)
					initiateReset(250);
				else
					cancelReset();
			}
			return false;
		}
	}
	return false;
}

uint32_t _serialPeek = -1;
void Serial_::begin(uint32_t baud_count)
{
	//modificato bar
	//flush();
	
}

void Serial_::begin(uint32_t baud_count, uint8_t config)
{
}

void Serial_::end(void)
{
}

//void Serial_::accept(void)  //bar   
//{
	//unsigned int num_char;
	
	//SerialUSB.println("Serial::accept cc");  //bar
	
	//num_char=USBD_Available(CDC_ENDPOINT_OUT);  //bar  //vedo quanti byte devo ricevere
	//SerialUSB.println("num_char: ");     //bar
	//SerialUSB.println(num_char);         //bar
	//uint8_t buffer[CDC_SERIAL_BUFFER_SIZE];  //si dichiara un array di tipo uint8_t di dimensione CDC_SERIAL_BUFFER_SIZE
	//uint8_t car = 0;
	//uint32_t len = USBD_Recv(CDC_ENDPOINT_OUT, (void*)&buffer, CDC_SERIAL_BUFFER_SIZE);
	//uint32_t len = USBD_Recv(CDC_ENDPOINT_OUT);  //commentato bar
	//SerialUSB.print("len: ");  //bar
	//SerialUSB.println(len);  //bar
	//noInterrupts();
	//ring_buffer *ringBuffer = &cdc_rx_buffer;    //si dichiara una variabile di tipo ring buffer
	//uint32_t i = ringBuffer->head;     //si dichiara una variabile indice del buffer
	//SerialUSB.print("i: ");  //bar
	//SerialUSB.println(i);  //bar

	// if we should be storing the received character into the location
	// just before the tail (meaning that the head would advance to the
	// current location of the tail), we're about to overflow the buffer
	// and so we don't write the character or advance the head.
	/*uint32_t k = 0;
	i = (i + 1) % CDC_SERIAL_BUFFER_SIZE;     //ok ci vuole
	while ((i != ringBuffer->tail) && (num_char > 0)) {  //modificare len con num_char
		car = USBD_Recv(CDC_ENDPOINT_OUT);
		ringBuffer->buffer[ringBuffer->head] = car;//buffer[k++];  //vedere meglio qua
		ringBuffer->head = i;  // ok va bene
		i = (i + 1) % CDC_SERIAL_BUFFER_SIZE;  //ok ci vuole
		num_char--;  //sostituire len con num_char
	}*/
	
	//SerialUSB.print("num_char dopo while: ");   //bar
	//SerialUSB.println(num_char);   //bar
	//interrupts();
	
	
	/*ring_buffer *buffer = &cdc_rx_buffer;  //commentato bar
	uint32_t i = (uint32_t)(buffer->head+1) % CDC_SERIAL_BUFFER_SIZE;  //commentata bar
	//uint32_t i = (uint32_t)(buffer->head) % CDC_SERIAL_BUFFER_SIZE;
	
	// if we should be storing the received character into the location
	// just before the tail (meaning that the head would advance to the
	// current location of the tail), we're about to overflow the buffer
	// and so we don't write the character or advance the head.
	while (i != buffer->tail) {
		SerialUSB.print("i: ");  //bar
		SerialUSB.println(i);    //bar
		uint32_t c;
		if (!USBD_Available(CDC_ENDPOINT_OUT)) {
            UDD_ReleaseRX(CDC_ENDPOINT_OUT);
			break;
		}
		c = USBD_Recv(CDC_ENDPOINT_OUT);
		buffer->buffer[buffer->head] = c;
		buffer->head = i;

		i = (i + 1) % CDC_SERIAL_BUFFER_SIZE;
	} */ //commentato bar
	
	
//}   //bar



void Serial_::accept(void)  //implementazione cc
{
	uint8_t buffer[CDC_SERIAL_BUFFER_SIZE];
	uint32_t len = USBD_Recv(CDC_ENDPOINT_OUT, (void*)&buffer,CDC_SERIAL_BUFFER_SIZE);
	//flush(); //modificato bar
	
	//SerialUSB.print("len in accept: ");
	//SerialUSB.println(len);
	noInterrupts();
	ring_buffer *ringBuffer = &cdc_rx_buffer;
	uint32_t i = ringBuffer->head;
	//for (i=0; i < CDC_SERIAL_BUFFER_SIZE; i++) ringBuffer->buffer[i]=0x00;  //modificato bar (inizializzo il buffer)
	
	uint32_t k = 0;
	i = (i+1) % CDC_SERIAL_BUFFER_SIZE;
	while((i != ringBuffer->tail) && (len>0) && (len <= CDC_SERIAL_BUFFER_SIZE))  //modificato bar
	{
		len--;
		ringBuffer->buffer[ringBuffer->head] = buffer[k++];
		ringBuffer->head = i;
		//if(i < CDC_SERIAL_BUFFER_SIZE) 
		i = (i+1) % CDC_SERIAL_BUFFER_SIZE;  //modificato bar
		//SerialUSB.print("len nel while: ");
		//SerialUSB.println(len);
		
	}
	//USBD_Flush(CDC_ENDPOINT_OUT);  //modificato bar
	//ringBuffer->head = ringBuffer->tail;  //modificato bar per prova (così non si riceve niente, OK)
	interrupts();
}  //implementazione cc



int Serial_::available(void)
{
	ring_buffer *buffer = &cdc_rx_buffer;
	//unsigned int r=0;
	//return USBD_Available(CDC_ENDPOINT_OUT); //modificato bar
	return (uint32_t)(CDC_SERIAL_BUFFER_SIZE + buffer->head - buffer->tail) % CDC_SERIAL_BUFFER_SIZE;
	//uint32_t delta = buffer->head - buffer->tail;   //modificato bar
	//if(delta < 0) return CDC_SERIAL_BUFFER_SIZE + delta;   //modificato bar
	//else return delta;   //modificato bar
}

int Serial_::peek(void)
{
	ring_buffer *buffer = &cdc_rx_buffer;

	if (buffer->head == buffer->tail)
	{
		return -1;
	}
	else
	{
		return buffer->buffer[buffer->tail];
	}
}

int Serial_::read(void)
{
	ring_buffer *buffer = &cdc_rx_buffer;
	unsigned int i=0;

	// if the head isn't ahead of the tail, we don't have any characters
	if (buffer->head == buffer->tail)
	{
		buffer->head = 0;
		buffer->tail = 0;
		//for(i=0; i < CDC_SERIAL_BUFFER_SIZE; i++) buffer->buffer[i]=0x00;  //modificato bar
		return -1;
	}
	else
	{
		unsigned char c = buffer->buffer[buffer->tail];
		/*if((uint32_t)(buffer->tail + 1) % CDC_SERIAL_BUFFER_SIZE)  //modificato bar
		{
			buffer->tail = (uint32_t)(buffer->tail + 1) % CDC_SERIAL_BUFFER_SIZE;
		}
		else
		{
			buffer->head = buffer->tail;  //modificato bar
			return c;
		}*/	
		if(buffer->tail < CDC_SERIAL_BUFFER_SIZE)  //modificato bar
		{
			buffer->tail = (uint32_t)(buffer->tail + 1) % CDC_SERIAL_BUFFER_SIZE;
		}
		else  //modificato bar
		{
			buffer->head = 0;
			buffer->tail = 0;
			//for (i=0; i < CDC_SERIAL_BUFFER_SIZE; i++) buffer->buffer[i]=0x00;
		}	
		//buffer->tail = (uint32_t)(buffer->tail + 1) % CDC_SERIAL_BUFFER_SIZE;
		if (USBD_Available(CDC_ENDPOINT_OUT))
		{
			accept();
			//USBD_Flush(CDC_ENDPOINT_OUT);  //modificato bar
			//buffer->head = buffer->tail; //modificato bar
			//cdc_rx_buffer.head = 0;  //modificato bar
			//cdc_rx_buffer.tail = 0;  //modificato bar
			//cdc_rx_buffer.head = cdc_rx_buffer.tail;  //modificato bar
		}
			//accept();  modificato bar
			//buffer->head = buffer->tail; //modificato bar
		flush();  //aggiunto bar	
		return c;
	}
}

void Serial_::flush(void)
{
	USBD_Flush(CDC_ENDPOINT_IN);
}

size_t Serial_::write(const uint8_t *buffer, size_t size)
{
	/* only try to send bytes if the high-level CDC connection itself
	 is open (not just the pipe) - the OS should set lineState when the port
	 is opened and clear lineState when the port is closed.
	 bytes sent before the user opens the connection or after
	 the connection is closed are lost - just like with a UART. */

	{
		uint32_t r = USBD_Send(CDC_ENDPOINT_IN, buffer, size);

		if (r > 0)
		{
			return r;
		} else
		{
			setWriteError();
			return 0;
		}
	}
	setWriteError();
	return 0;
}

size_t Serial_::write(uint8_t c) {
	return write(&c, 1);
}

// This operator is a convenient way for a sketch to check whether the
// port has actually been configured and opened by the host (as opposed
// to just being connected to the host).  It can be used, for example, in
// setup() before printing to ensure that an application on the host is
// actually ready to receive and display the data.
// We add a short delay before returning to fix a bug observed by Federico
// where the port is configured (lineState != 0) but not quite opened.

Serial_::operator bool()
{
	// this is here to avoid spurious opening after upload
	if (millis() < 500)
		return false;

	bool result = false;

	if (_usbLineInfo.lineState > 0)
	{
		result = true;
	}

	delay(10);
	return result;
}

Serial_ SerialUSB;

#endif
