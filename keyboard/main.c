/*
 * Keyboard driver.  Can drive a 8x16 keyboard matrix, plus has ICSP and USB HID output.
 * Uses an ATMega124 (32 I/O) running at 20MHz (requires a crystal).
 * 
 * Pinouts: 
 *		PORTA: 8x Row Drivers
 *		PORTB: 8x Column Readers + ICSP breakout
 *		PORTC: 8x Column Readers
 *		PORTD: Misc I/O
 *			D0, D1:	Serial Breakout (Rx Tx)
 *			D2, D3:	USB Data D+ / D- (via level converters)
 *			D4..D6:	Outputs for LEDs
 *			D7: Unused?
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>  /* for sei() */
#include <avr/pgmspace.h>   /* required by usbdrv.h */
#include <avr/wdt.h>	/* No idea why we need this... */

#include "usbdrv/usbdrv.h"
#include "hid.h"
#include "qwerty.h"

#include "lib/serial/serial.h"

PROGMEM char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] = { /* USB report descriptor, size must match usbconfig.h */
    USAGE_PAGE     , 0x01, // Generic Desktop
    USAGE          , 0x06, // Keyboard
    COLLECTION     , 0x01, // Application
    //REPORT_ID      , 0x01,
    // 6

	// modifier byte
    USAGE_PAGE     , 0x07, // Keyboard
    USAGE_MINIMUM  , 0xe0, // Keyboard LeftControl
    USAGE_MAXIMUM  , 0xe7, // Keyboard Right GUI
    LOGICAL_MINIMUM, 0x00,
    LOGICAL_MAXIMUM, 0x01,
    REPORT_SIZE    , 0x01, // 1x
    REPORT_COUNT   , 0x08, // byte
    INPUT          , FLAG_DATA | FLAG_VAR | FLAG_ABS,
    // 16 -> 22
    
    // reserved byte
    REPORT_COUNT   , 0x01, // 1x
    REPORT_SIZE    , 0x08, // byte
    INPUT          , FLAG_CNST,
    // 6 -> 28

	// led bitmask
	REPORT_COUNT   , 0x05, // 5x
	REPORT_SIZE    , 0x01, // bits
	USAGE_PAGE     , 0x08, // LEDs
	USAGE_MINIMUM  , 0x01, // Num Lock
	USAGE_MAXIMUM  , 0x05, // Kana
	OUTPUT         , FLAG_DATA | FLAG_VAR | FLAG_ABS,
	// 12 -> 40
		
	// led padding
	REPORT_COUNT   , 0x01, // 1x
	REPORT_SIZE    , 0x03, // 3 bits
	OUTPUT         , FLAG_CNST,
	// 6 -> 46

	// key codes
    REPORT_COUNT   , 0x06, // 6x
    REPORT_SIZE    , 0x08, // bytes
    LOGICAL_MINIMUM, 0x00,
    LOGICAL_MAXIMUM, 0x7F,
    USAGE_PAGE     , 0x07, // keycodes
    USAGE_MINIMUM  , 0x00, // Reserved (no event indicated)
    USAGE_MAXIMUM  , 0x7F, // Keyboard Right GUI
    INPUT          , FLAG_DATA | FLAG_ARR | FLAG_ABS,
    // 16 -> 62

    END_COLLECTION
	// 1 -> 63
};

static uchar report_buffer[8];
static uchar idleRate;   		/* repeat rate in 4ms units */
static uchar protocolVer=1;      /* 0 is the boot protocol, 1 is report protocol */
uchar ledState = 0;   // bit0: numlock, bit1: caps lock, bit2: scroll lock, bit3: compose, bit4: kana, bit5..7 reserved
uchar expectReport = 0; // incoming led report is available

/*
 * Callback, used in vusb
 */
usbMsgLen_t usbFunctionSetup(uchar data[8]) {
	//toggle_led();
    
    usbRequest_t    *rq = (void *)data;

    /* The following requests are never used. But since they are required by
     * the specification, we implement them in this example.
     */
    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){    /* class request type */
        switch(rq->bRequest) {
            case USBRQ_HID_GET_REPORT:
                // wValue: ReportType (highbyte), ReportID (lowbyte)
                // we only have one report type, so don't look at wValue
                // TODO when the consumer devices report is added, return a different report
//				if(rq->wValue.bytes[1] == 0x02) {
//					usbMsgPtr = (void *)&report_buffer2;
//					return sizeof(report_buffer2);
//					return 0;
//				} else {
					usbMsgPtr = report_buffer;
					return sizeof(report_buffer);
//				}
            case USBRQ_HID_SET_REPORT:
				if (rq->wLength.word == 1) {
					expectReport = 1;
				}
				return 0xFF; /* call usbFunctionWrite() */
            case USBRQ_HID_GET_IDLE:
                usbMsgPtr = &idleRate;
                return 1;
            case USBRQ_HID_SET_IDLE:
                idleRate = rq->wValue.bytes[1];
                return 0;
			case USBRQ_HID_GET_PROTOCOL:
				if (rq->wValue.bytes[1] < 1) {
					protocolVer = rq->wValue.bytes[1];
				}
				return 0;
			case USBRQ_HID_SET_PROTOCOL:
				usbMsgPtr = &protocolVer;
				return 1;
        }
    } else {
        /* no vendor specific requests implemented */
    }
    return 0;   /* default for not implemented requests: return no data back to host */
}

/*
 * Callback, used in vusb
 */
uchar usbFunctionWrite(uchar *data, uchar len) {
	if (expectReport && (len == 1)) {
		ledState = data[0];
		
		if (ledState & LED_CAPS) {
			//TODO adjust LEDs
			//LED_PORT &= ~_BV(LED_BIT);
		} else {
			//LED_PORT |= _BV(LED_BIT);
		}

	}
	expectReport = 0;
	return 0x01; // end of transfer
}

/* ------------------------------------------------------------------------- */
 


/*
 * Sets the row (0..7)
 */
void set_row(uint8_t row){
	PORTA = ~(_BV(row));
}

/*
 * Reads and debounces the column.  High bits indicate a key press.
 */
uint16_t read_columns(){
	uint16_t result = PINC;
	result = result << 8;
	result += PINB;
	result = ~result;
	return result;
}

/*
 * Updates the report by scanning all keys, and sending key all currently up keys.  The OS
 * takes care of repeat rates, etc.
 */
void update_report(){
	//Init rebortBuffer back aato 0
	for (uint8_t i = 0; i < 8; i++){
		report_buffer[i] = 0x00;
	}

	uint8_t buffer_index = 2;
	uint16_t columns = 0x0000;

	for (uint8_t row = 0; row < 8; row++){	
		set_row(row);
		columns = read_columns();
	
		if (columns != 0x00){
			for (uint8_t col = 0; col < 16; col++){
				if (_BV(col) & columns){
//					report_buffer[2] = 0x04;	//Hard coded to A; TODO Remove
					uchar key = 0;
					uchar mod = 0;

					lookup_qwerty(row, col, &key, &mod);
					if (key != 0) report_buffer[buffer_index] = key;
					if (mod != 0) report_buffer[0] |= mod;
					
					//At most we have 6 keys (byte 0 in report_buffer is mods; 1 is unused; 2..7 are keys)
					buffer_index++;
					if (buffer_index >= 8) return;
				}
			}
		}	
	}
}

void keyboard_init(){
	//Set rows into low impedence (write) mode
	DDRA = 0xFF;
	
	//Ensure columns are high impedence (read) with pullups
	DDRB = 0x00;
	DDRC = 0x00;
	PORTB = 0xFF;
	PORTC = 0xFF;
	
	//LED Outputs
//	DDRD |= _BV(PD4) | _BV(PD5) | _BV(PD6);
//	PORTD |= _BV(PD4) | _BV(PD5) | _BV(PD6);
}

int main (void){
	//Enable watchdog timer for 1 second.  Not sure why I have to do this, but the examples do.
	wdt_enable(WDTO_1S);
	
#ifdef DEBUG
	serial_init_b(9600);
#endif
	
	keyboard_init();
	usbInit();
	
    usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */
    uint8_t i = 0;
    while(--i){             /* fake USB disconnect for > 250 ms */
        wdt_reset();
        _delay_ms(1);
    }
    usbDeviceConnect();
	
	sei();

#ifdef DEBUG
	//Used as itoa destination
	char temp[32];
#endif

	//Main program loop
	while (1){
		wdt_reset();
		
		usbPoll();
		
		update_report();
		usbSetInterrupt(report_buffer, sizeof(report_buffer));
	}
}
