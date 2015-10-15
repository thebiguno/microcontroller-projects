#include "LedTable.h"

#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>
#include <Buttons.h>
#include "lib/ws2812.h"
#include "lib/cp_ascii_caps.h"
#include "lib/f_5x5.h"

#include "Matrix.h"

#include "Clock.h"
#include "Life.h"
#include "Plasma.h"
#include "Tictactoe.h"

using namespace digitalcave;

volatile uint8_t sample = 1;
Matrix matrix = Matrix();
Buttons buttons = Buttons(&PORTF, 0x03); // pins 0 and 1
Hsv hsv = Hsv(0,0xff,0x0f);

int main() {
	srandom(0);

	TCCR0A = 0x00; // normal mode
	TCCR0B = 0x05; // clk/1024
	OCR0A = 0xbb; // 187 clock ticks = 12 ms
	TIMSK0 = 0x02; // enable compare match A
		
	uint8_t selected = 0;
	
	DDRB = 0xff;
	PORTB = 0x00;

	uint8_t released = 0;
	uint8_t held = 0;
	
	matrix.setFont(font_5X5, codepage_ascii_caps, 5, 5);
	
	while (1) {
		if (sample) {
			buttons.sample();
			released = buttons.released();
			held = buttons.held();
			sample = 0;
		}
		
		matrix.setColor(0,0,0);
		matrix.rectangle(0,0,11,11, DRAW_FILLED);
		matrix.setColor(Rgb(hsv));
		
		if (selected == 0) {
			matrix.text(0, 0, "CL", 0);
			matrix.text(0, 6, "CK", 0);
		}
		else if (selected == 1) {
			matrix.text(0, 0, "LI", 0);
			matrix.text(0, 6, "FE", 0);
		}
		else if (selected == 2) {
			matrix.text(0, 0, "PL", 0);
			matrix.text(0, 6, "AS", 0);
		}
		else {
			matrix.text(0, 0, "TI", 0);
			matrix.text(0, 6, "CK", 0);
		}
		
		matrix.flush();
		
		if (held & 0x01 && held & 0x02) {
			UDCON = 1;
			USBCON = (1<<FRZCLK);  // disable USB
			UCSR1B = 0;
			_delay_ms(5);
		    EIMSK = 0; PCICR = 0; SPCR = 0; ACSR = 0; EECR = 0; ADCSRA = 0;
		    TIMSK0 = 0; TIMSK1 = 0; TIMSK3 = 0; TIMSK4 = 0; UCSR1B = 0; TWCR = 0;
		    DDRB = 0; DDRC = 0; DDRD = 0; DDRE = 0; DDRF = 0; TWCR = 0;
		    PORTB = 0; PORTC = 0; PORTD = 0; PORTE = 0; PORTF = 0;
		    asm volatile("jmp 0x7E00"); // TODO not sure if this should be 7E00 or 7000 -- check fuse bits
		}
		
		if (released & 0x01) {
			selected++;
			selected %= 4;
		}
		else if (held & 0x01) {
			// decrease brightness;
			uint8_t v = hsv.getValue();
			switch (v) {
				case 0xff: hsv.setValue(0x7f); break;
				case 0x7f: hsv.setValue(0x3f); break;
				case 0x3f: hsv.setValue(0x1f); break;
				case 0x1f: hsv.setValue(0x0f); break;
				case 0x0f: hsv.setValue(0x07); break;
				case 0x07: hsv.setValue(0x03); break;
				case 0x03: hsv.setValue(0x01); break;
				default: hsv.setValue(0x00);
			}			
		}
		if (released & 0x02) {
			switch (selected) {
				case 0: { Clock clk; clk.run(); break; }
				case 1: { Life life; life.run(); break; }
				case 2: { Plasma plasma; plasma.run(); break; }
				case 3: { Tictactoe ttt; ttt.run(); break; }
			}
		}
		else if (held & 0x02) {
			// increase brightness
			uint8_t v = hsv.getValue();
			switch (v) {
				case 0x00: hsv.setValue(0x01); break;
				case 0x01: hsv.setValue(0x03); break;
				case 0x03: hsv.setValue(0x07); break;
				case 0x07: hsv.setValue(0x0f); break;
				case 0x0f: hsv.setValue(0x1f); break;
				case 0x1f: hsv.setValue(0x3f); break;
				case 0x3f: hsv.setValue(0x7f); break;
				default: hsv.setValue(0xff);
			}
		}

		_delay_ms(127);
	}
}

ISR(TIMER0_COMPA_vect) {
	sample = 1;
	TCNT0 = 0;
}
