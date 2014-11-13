/* Teensy RawHID example
 * http://www.pjrc.com/teensy/rawhid.html
 * Copyright (c) 2009 PJRC.COM, LLC
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above description, website URL and copyright notice and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// Version 1.0: Initial Release
// Version 1.1: fixed bug in analog

#define USB_PRIVATE_INCLUDE
#include "rawhid.h"
#include <stdint.h>

/**************************************************************************
 *
 *  Configurable Options
 *
 **************************************************************************/

// You can change these to give your code its own name.
#ifndef USB_STR_MANUFACTURER
#define USB_STR_MANUFACTURER	L"digitalcave.ca"
#endif
#ifndef USB_STR_PRODUCT
#define USB_STR_PRODUCT		L"RawHID"
#endif
#ifndef USB_STRING_DESCRIPTOR_LENGTH
//The maximum length of USB_STR_MANUFACTURER and USB_STR_PRODUCT.  Keep this as small as possible to save PROGMEM space
#define USB_STRING_DESCRIPTOR_LENGTH 16
#endif

// These 4 numbers identify your device.  Set these to
// something that is (hopefully) not used by any others!
#ifndef USB_VENDOR_ID
#define USB_VENDOR_ID		0x16C0
#endif
#ifndef USB_PRODUCT_ID
#define USB_PRODUCT_ID		0x0480
#endif
#ifndef USB_RAWHID_USAGE_PAGE
#define USB_RAWHID_USAGE_PAGE	0xFFAB	// recommended: 0xFF00 to 0xFFFF
#endif
#ifndef USB_RAWHID_USAGE
#define USB_RAWHID_USAGE		0x0200	// recommended: 0x0100 to 0xFFFF
#endif

// These determine the bandwidth that will be allocated
// for your communication.  You do not need to use it
// all, but allocating more than necessary means reserved
// bandwidth is no longer available to other USB devices.
#ifndef USB_RAWHID_TX_SIZE
#define USB_RAWHID_TX_SIZE		64	// transmit packet size
#endif
#ifndef USB_RAWHID_TX_INTERVAL
#define USB_RAWHID_TX_INTERVAL	2	// max # of ms between transmit packets
#endif
#ifndef USB_RAWHID_RX_SIZE
#define USB_RAWHID_RX_SIZE		64	// receive packet size
#endif
#ifndef USB_RAWHID_RX_INTERVAL
#define USB_RAWHID_RX_INTERVAL	8	// max # of ms between receive packets
#endif


/**************************************************************************
 *
 *  Endpoint Buffer Configuration
 *
 **************************************************************************/

#define ENDPOINT0_SIZE		32
#define RAWHID_INTERFACE	0
#define RAWHID_TX_ENDPOINT	1
#define RAWHID_RX_ENDPOINT	2

#if defined(__AVR_AT90USB162__) || defined(__AVR_ATmega32U2__)
#define RAWHID_TX_BUFFER	EP_SINGLE_BUFFER
#define RAWHID_RX_BUFFER	EP_SINGLE_BUFFER
#else
#define RAWHID_TX_BUFFER	EP_DOUBLE_BUFFER
#define RAWHID_RX_BUFFER	EP_DOUBLE_BUFFER
#endif

static const uint8_t PROGMEM endpoint_config_table[] = {
	1, EP_TYPE_INTERRUPT_IN,  EP_SIZE(USB_RAWHID_TX_SIZE) | RAWHID_TX_BUFFER,
	1, EP_TYPE_INTERRUPT_OUT,  EP_SIZE(USB_RAWHID_RX_SIZE) | RAWHID_RX_BUFFER,
	0,
	0
};


/**************************************************************************
 *
 *  Descriptor Data
 *
 **************************************************************************/

// Descriptors are the data that your computer reads when it auto-detects
// this USB device (called "enumeration" in USB lingo).  The most commonly
// changed items are editable at the top of this file.  Changing things
// in here should only be done by those who've read chapter 9 of the USB
// spec and relevant portions of any USB class specifications!


static const uint8_t PROGMEM device_descriptor[] = {
	18,					// bLength
	1,					// bDescriptorType
	0x00, 0x02,				// bcdUSB
	0,					// bDeviceClass
	0,					// bDeviceSubClass
	0,					// bDeviceProtocol
	ENDPOINT0_SIZE,				// bMaxPacketSize0
	LSB(USB_VENDOR_ID), MSB(USB_VENDOR_ID),		// idVendor
	LSB(USB_PRODUCT_ID), MSB(USB_PRODUCT_ID),	// idProduct
	0x00, 0x01,				// bcdDevice
	1,					// iManufacturer
	2,					// iProduct
	0,					// iSerialNumber
	1					// bNumConfigurations
};

static const uint8_t PROGMEM rawhid_hid_report_desc[] = {
	0x06, LSB(USB_RAWHID_USAGE_PAGE), MSB(USB_RAWHID_USAGE_PAGE),
	0x0A, LSB(USB_RAWHID_USAGE), MSB(USB_RAWHID_USAGE),
	0xA1, 0x01,				// Collection 0x01
	0x75, 0x08,				// report size = 8 bits
	0x15, 0x00,				// logical minimum = 0
	0x26, 0xFF, 0x00,			// logical maximum = 255
	0x95, USB_RAWHID_TX_SIZE,			// report count
	0x09, 0x01,				// usage
	0x81, 0x02,				// Input (array)
	0x95, USB_RAWHID_RX_SIZE,			// report count
	0x09, 0x02,				// usage
	0x91, 0x02,				// Output (array)
	0xC0					// end collection
};


#define CONFIG1_DESC_SIZE        (9+9+9+7+7)
#define RAWHID_HID_DESC_OFFSET   (9+9)
static const uint8_t PROGMEM config1_descriptor[CONFIG1_DESC_SIZE] = {
	// configuration descriptor, USB spec 9.6.3, page 264-266, Table 9-10
	9, 					// bLength;
	2,					// bDescriptorType;
	LSB(CONFIG1_DESC_SIZE),			// wTotalLength
	MSB(CONFIG1_DESC_SIZE),
	1,					// bNumInterfaces
	1,					// bConfigurationValue
	0,					// iConfiguration
	0xC0,					// bmAttributes
	50,					// bMaxPower

	// interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
	9,					// bLength
	4,					// bDescriptorType
	RAWHID_INTERFACE,			// bInterfaceNumber
	0,					// bAlternateSetting
	2,					// bNumEndpoints
	0x03,					// bInterfaceClass (0x03 = HID)
	0x00,					// bInterfaceSubClass (0x01 = Boot)
	0x00,					// bInterfaceProtocol (0x01 = Keyboard)
	0,					// iInterface
	// HID interface descriptor, HID 1.11 spec, section 6.2.1
	9,					// bLength
	0x21,					// bDescriptorType
	0x11, 0x01,				// bcdHID
	0,					// bCountryCode
	1,					// bNumDescriptors
	0x22,					// bDescriptorType
	sizeof(rawhid_hid_report_desc),		// wDescriptorLength
	0,
	// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
	7,					// bLength
	5,					// bDescriptorType
	RAWHID_TX_ENDPOINT | 0x80,		// bEndpointAddress
	0x03,					// bmAttributes (0x03=intr)
	USB_RAWHID_TX_SIZE, 0,			// wMaxPacketSize
	USB_RAWHID_TX_INTERVAL,			// bInterval
	// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
	7,					// bLength
	5,					// bDescriptorType
	RAWHID_RX_ENDPOINT,			// bEndpointAddress
	0x03,					// bmAttributes (0x03=intr)
	USB_RAWHID_RX_SIZE, 0,			// wMaxPacketSize
	USB_RAWHID_RX_INTERVAL			// bInterval
};

// If you're desperate for a little extra code memory, these strings
// can be completely removed if iManufacturer, iProduct, iSerialNumber
// in the device desciptor are changed to zeros.
struct usb_string_descriptor_struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	const wchar_t wString[USB_STRING_DESCRIPTOR_LENGTH];
};
static struct usb_string_descriptor_struct string0 = {
	4,
	3,
	{0x0409}
};
static const struct usb_string_descriptor_struct PROGMEM string1 = {
	sizeof(USB_STR_MANUFACTURER),
	3,
	USB_STR_MANUFACTURER
};
static const struct usb_string_descriptor_struct PROGMEM string2 = {
	sizeof(USB_STR_PRODUCT),
	3,
	USB_STR_PRODUCT
};

// This table defines which descriptor data is sent for each specific
// request from the host (in wValue and wIndex).
static const struct descriptor_list_struct {
	uint16_t	wValue;
	uint16_t	wIndex;
	const uint8_t	*addr;
	uint8_t		length;
} PROGMEM descriptor_list[] = {
	{0x0100, 0x0000, device_descriptor, sizeof(device_descriptor)},
	{0x0200, 0x0000, config1_descriptor, sizeof(config1_descriptor)},
	{0x2200, RAWHID_INTERFACE, rawhid_hid_report_desc, sizeof(rawhid_hid_report_desc)},
	{0x2100, RAWHID_INTERFACE, config1_descriptor+RAWHID_HID_DESC_OFFSET, 9},
	{0x0300, 0x0000, (const uint8_t *)&string0, 4},
	{0x0301, 0x0409, (const uint8_t *)&string1, sizeof(USB_STR_MANUFACTURER)},
	{0x0302, 0x0409, (const uint8_t *)&string2, sizeof(USB_STR_PRODUCT)}
};
#define NUM_DESC_LIST (sizeof(descriptor_list)/sizeof(struct descriptor_list_struct))


/**************************************************************************
 *
 *  Variables - these are the only non-stack RAM usage
 *
 **************************************************************************/

// zero when we are not configured, non-zero when enumerated
static volatile uint8_t usb_configuration=0;

// these are a more reliable timeout than polling the
// frame counter (UDFNUML)
static volatile uint8_t rx_timeout_count=0;
static volatile uint8_t tx_timeout_count=0;



/**************************************************************************
 *
 *  Public Functions - these are the API intended for the user
 *
 **************************************************************************/


// initialize USB
void usb_init(void)
{
	HW_CONFIG();
	USB_FREEZE();				// enable USB
	PLL_CONFIG();				// config PLL
	while (!(PLLCSR & (1<<PLOCK))) ;	// wait for PLL lock
	USB_CONFIG();				// start USB clock
	UDCON = 0;				// enable attach resistor
	usb_configuration = 0;
	UDIEN = (1<<EORSTE)|(1<<SOFE);
	sei();
}

// return 0 if the USB is not configured, or the configuration
// number selected by the HOST
uint8_t usb_configured(void)
{
	return usb_configuration;
}

// receive a packet, with timeout
int8_t usb_rawhid_recv(uint8_t *buffer, uint8_t timeout){
	uint8_t intr_state;

	// if we're not online (enumerated and configured), error
	if (!usb_configuration) return -1;
	intr_state = SREG;
	cli();
	rx_timeout_count = timeout;
	UENUM = RAWHID_RX_ENDPOINT;
	// wait for data to be available in the FIFO
	while (1) {
		if (UEINTX & _BV(RWAL)) break;	
		SREG = intr_state;
		if (rx_timeout_count == 0) return 0;
		if (!usb_configuration) return -1;
		intr_state = SREG;
		cli();
		UENUM = RAWHID_RX_ENDPOINT;
	}
	// read bytes from the FIFO
	for (uint8_t i = 0; i < USB_RAWHID_RX_SIZE; i++){
		*buffer++ = UEDATX;
	}
	// release the buffer
	UEINTX = 0x6B;
	SREG = intr_state;
	return USB_RAWHID_RX_SIZE;
}

// send a packet, with timeout
int8_t usb_rawhid_send(const uint8_t *buffer, uint8_t timeout){
	uint8_t intr_state;

	// if we're not online (enumerated and configured), error
	if (!usb_configuration) return -1;
	intr_state = SREG;
	cli();
	tx_timeout_count = timeout;
	UENUM = RAWHID_TX_ENDPOINT;
	// wait for the FIFO to be ready to accept data
	while (1) {
		if (UEINTX & (1<<RWAL)) break;
		SREG = intr_state;
		if (tx_timeout_count == 0) return 0;
		if (!usb_configuration) return -1;
		intr_state = SREG;
		cli();
		UENUM = RAWHID_TX_ENDPOINT;
	}
	// write bytes from the FIFO
	for (uint8_t i = 0; i < USB_RAWHID_TX_SIZE; i++){
		UEDATX = *buffer++;
	}
	// transmit it now
	UEINTX = 0x3A;
	SREG = intr_state;
	return USB_RAWHID_TX_SIZE;
}




/**************************************************************************
 *
 *  Private Functions - not intended for general user consumption....
 *
 **************************************************************************/


#if (GCC_VERSION >= 40300) && (GCC_VERSION < 40302)
#error "Buggy GCC 4.3.0 compiler, please upgrade!"
#endif


// USB Device Interrupt - handle all device-level events
// the transmit buffer flushing is triggered by the start of frame
//
ISR(USB_GEN_vect)
{
	uint8_t intbits, t;

	intbits = UDINT;
	UDINT = 0;
	if (intbits & (1<<EORSTI)) {
		UENUM = 0;
		UECONX = 1;
		UECFG0X = EP_TYPE_CONTROL;
		UECFG1X = EP_SIZE(ENDPOINT0_SIZE) | EP_SINGLE_BUFFER;
		UEIENX = (1<<RXSTPE);
		usb_configuration = 0;
	}
	if ((intbits & (1<<SOFI)) && usb_configuration) {
		t = rx_timeout_count;
		if (t) rx_timeout_count = --t;
		t = tx_timeout_count;
		if (t) tx_timeout_count = --t;
	}
}



// Misc functions to wait for ready and send/receive packets
static inline void usb_wait_in_ready(void){
	while (!(UEINTX & (1<<TXINI))) ;
}
static inline void usb_send_in(void){
	UEINTX = ~(1<<TXINI);
}
static inline void usb_wait_receive_out(void){
	while (!(UEINTX & (1<<RXOUTI))) ;
}
static inline void usb_ack_out(void){
	UEINTX = ~(1<<RXOUTI);
}



// USB Endpoint Interrupt - endpoint 0 is handled here.  The
// other endpoints are manipulated by the user-callable
// functions, and the start-of-frame interrupt.
//
ISR(USB_COM_vect){
	uint8_t intbits;
	const uint8_t *list;
	const uint8_t *cfg;
	uint8_t i, n, len, en;
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
	uint16_t desc_val;
	const uint8_t *desc_addr;
	uint8_t	desc_length;

	UENUM = 0;
	intbits = UEINTX;
	if (intbits & (1<<RXSTPI)) {
		bmRequestType = UEDATX;
		bRequest = UEDATX;
		wValue = UEDATX;
		wValue |= (UEDATX << 8);
		wIndex = UEDATX;
		wIndex |= (UEDATX << 8);
		wLength = UEDATX;
		wLength |= (UEDATX << 8);
		UEINTX = ~((1<<RXSTPI) | (1<<RXOUTI) | (1<<TXINI));
		if (bRequest == GET_DESCRIPTOR) {
			list = (const uint8_t *)descriptor_list;
			for (i=0; ; i++) {
				if (i >= NUM_DESC_LIST) {
					UECONX = (1<<STALLRQ)|(1<<EPEN);  //stall
					return;
				}
				desc_val = pgm_read_word(list);
				if (desc_val != wValue) {
					list += sizeof(struct descriptor_list_struct);
					continue;
				}
				list += 2;
				desc_val = pgm_read_word(list);
				if (desc_val != wIndex) {
					list += sizeof(struct descriptor_list_struct)-2;
					continue;
				}
				list += 2;
				desc_addr = (const uint8_t *)pgm_read_word(list);
				list += 2;
				desc_length = pgm_read_byte(list);
				break;
			}
			len = (wLength < 256) ? wLength : 255;
			if (len > desc_length) len = desc_length;
			do {
				// wait for host ready for IN packet
				do {
					i = UEINTX;
				} while (!(i & ((1<<TXINI)|(1<<RXOUTI))));
				if (i & (1<<RXOUTI)) return;	// abort
				// send IN packet
				n = len < ENDPOINT0_SIZE ? len : ENDPOINT0_SIZE;
				for (i = n; i; i--) {
					UEDATX = pgm_read_byte(desc_addr++);
				}
				len -= n;
				usb_send_in();
			} while (len || n == ENDPOINT0_SIZE);
			return;
		}
	
		if (bRequest == SET_ADDRESS) {
			usb_send_in();
			usb_wait_in_ready();
			UDADDR = wValue | (1<<ADDEN);
			return;
		}
		if (bRequest == SET_CONFIGURATION && bmRequestType == 0) {
			usb_configuration = wValue;
			usb_send_in();
			cfg = endpoint_config_table;
			for (i=1; i<5; i++) {
				UENUM = i;
				en = pgm_read_byte(cfg++);
				UECONX = en;
				if (en) {
					UECFG0X = pgm_read_byte(cfg++);
					UECFG1X = pgm_read_byte(cfg++);
				}
			}
			UERST = 0x1E;
			UERST = 0;
			return;
		}
		if (bRequest == GET_CONFIGURATION && bmRequestType == 0x80) {
			usb_wait_in_ready();
			UEDATX = usb_configuration;
			usb_send_in();
			return;
		}

		if (bRequest == GET_STATUS) {
			usb_wait_in_ready();
			i = 0;
			if (bmRequestType == 0x82) {
				UENUM = wIndex;
				if (UECONX & (1<<STALLRQ)) i = 1;
				UENUM = 0;
			}
			UEDATX = i;
			UEDATX = 0;
			usb_send_in();
			return;
		}
		if ((bRequest == CLEAR_FEATURE || bRequest == SET_FEATURE) && bmRequestType == 0x02 && wValue == 0) {
			i = wIndex & 0x7F;
			if (i >= 1 && i <= MAX_ENDPOINT) {
				usb_send_in();
				UENUM = i;
				if (bRequest == SET_FEATURE) {
					UECONX = (1<<STALLRQ)|(1<<EPEN);
				} else {
					UECONX = (1<<STALLRQC)|(1<<RSTDT)|(1<<EPEN);
					UERST = (1 << i);
					UERST = 0;
				}
				return;
			}
		}
		if (wIndex == RAWHID_INTERFACE) {
			if (bmRequestType == 0xA1 && bRequest == HID_GET_REPORT) {
				len = USB_RAWHID_TX_SIZE;
				do {
					// wait for host ready for IN packet
					do {
						i = UEINTX;
					} while (!(i & ((1<<TXINI)|(1<<RXOUTI))));
					if (i & (1<<RXOUTI)) return;	// abort
					// send IN packet
					n = len < ENDPOINT0_SIZE ? len : ENDPOINT0_SIZE;
					for (i = n; i; i--) {
						// just send zeros
						UEDATX = 0;
					}
					len -= n;
					usb_send_in();
				} while (len || n == ENDPOINT0_SIZE);
				return;
			}
			if (bmRequestType == 0x21 && bRequest == HID_SET_REPORT) {
				len = USB_RAWHID_RX_SIZE;
				do {
					n = len < ENDPOINT0_SIZE ? len : ENDPOINT0_SIZE;
					usb_wait_receive_out();
					// ignore incoming bytes
					usb_ack_out();
					len -= n;
				} while (len);
				usb_wait_in_ready();
				usb_send_in();
				return;
			}
		}
	}
	UECONX = (1<<STALLRQ) | (1<<EPEN);	// stall
}


