#include "LedTable.h"

using namespace digitalcave;

Matrix matrix = Matrix();
volatile uint32_t ms = 0;
ButtonAVR b1 = ButtonAVR(&PORTF, 0x00, 25, 25, 500, 500);
ButtonAVR b2 = ButtonAVR(&PORTF, 0x01, 25, 25, 500, 500);
Hsv hsv = Hsv(0,0xff,0x1f);

int main() {
	srandom(0);

	TCCR0A = 0x00; // normal mode
	TCCR0B = 0x03; // clk/64
	OCR0A = 250; // 1 ms
	TIMSK0 = 0x02; // enable compare match A

	sei();

	uint8_t selected = 0;

	// input high (pullup)
	DDRF = 0x00;
	PORTF = 0xff;

	DDRB = _BV(PORTB5);

	// output low
	DDRB = 0xff;
	PORTB = 0x00;

	matrix.setFont(font_5X5, codepage_ascii_caps, 5, 5);

	twi_init();

	while (1) {
		_delay_ms(10);
		uint8_t change = 1;
		if (change) {
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
			else if (selected == 3) {
				matrix.text(0, 0, "TI", 0);
				matrix.text(0, 6, "CK", 0);
			} else {
				matrix.text(0, 0, "BO", 0);
				matrix.text(0, 6, "OT", 0);
			}

			matrix.flush();
			change = 0;
		}

		// handle buttons

		b1.sample(ms);
		b2.sample(ms);

		if (b1.longReleaseEvent()) {
			change = 1;
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
		} else if (b1.releaseEvent()) {
			// change selection
			change = 1;
			selected++;
			selected %= 5;
		} else if (b2.longReleaseEvent()) {
			change = 1;
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
		} else if (b2.releaseEvent()) {
			// activate selection
			change = 1;
			switch (selected) {
				case 0: { Clock clk; clk.run(); break; }
				case 1: { Life life; life.run(); break; }
				case 2: { Plasma plasma; plasma.run(); break; }
				case 3: { Tictactoe ttt; ttt.run(); break; }
				case 4: {
					UDCON = 1;
					USBCON = (1<<FRZCLK);  // disable USB
					UCSR1B = 0;
					_delay_ms(5);
					EIMSK = 0; PCICR = 0; SPCR = 0; ACSR = 0; EECR = 0; ADCSRA = 0;
					TIMSK0 = 0; TIMSK1 = 0; TIMSK3 = 0; TIMSK4 = 0; UCSR1B = 0; TWCR = 0;
					DDRB = 0; DDRC = 0; DDRD = 0; DDRE = 0; DDRF = 0; TWCR = 0;
					PORTB = 0; PORTC = 0; PORTD = 0; PORTE = 0; PORTF = 0;
					asm volatile("jmp 0x7000");
				}
			}
		}
	}
	return 0;
}

ISR(TIMER0_COMPA_vect) {
	ms++;
}

extern "C" void __cxa_pure_virtual() { while (1); }
