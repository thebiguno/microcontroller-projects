/*
Copyright 2014 Warren Janssens <warren.janssens@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * scan matrix
 */
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>
#include "action_layer.h"
#include "print.h"
#include "debug.h"
#include "util.h"
#include "matrix.h"

#ifndef DEBOUNCE
#   define DEBOUNCE	5
#endif
static uint8_t debouncing = DEBOUNCE;

/* matrix state(1:on, 0:off) */
static uint8_t matrix[MATRIX_ROWS];
static uint8_t matrix_debouncing[MATRIX_ROWS];

static matrix_row_t read(uint8_t row);
static void unselect();
static void select(uint8_t row);

inline
uint8_t matrix_rows(void)
{
    return MATRIX_ROWS;
}

inline
uint8_t matrix_cols(void)
{
    return MATRIX_COLS;
}

void matrix_init(void) {
	//debug_enable = true;
	
	//dprint("matrix_init"); dprintln();
	// output (mux & led) low
	DDRB    = 0xFF;
	PORTB   = 0x00;
	//DDRD      = 0xFF;
	//PORTD     = 0x00;
	
	// input with pullup
	DDRD    = 0x00;
	PORTD   = 0xFF;
	//DDRB      = 0x00;
	//PORTB     = 0xFF;
	
    // initialize row and col
    unselect();

    // initialize matrix state: all keys off
    for (uint8_t i=0; i < MATRIX_ROWS; i++) {
        matrix[i] = 0;
        matrix_debouncing[i] = 0;
    }
}

uint8_t matrix_scan(void) {
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        select(i);
        uint8_t row = read(i);
        if (matrix_debouncing[i] != row) {
            matrix_debouncing[i] = row;
            if (debouncing) {
                debug("bounce!: "); debug_hex(debouncing); debug("\n");
            }
            debouncing = DEBOUNCE;
        }
        unselect();
    }

    if (debouncing) {
        if (--debouncing) {
            _delay_ms(1);
        } else {
            for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
                matrix[i] = matrix_debouncing[i];
            }
        }
    }

    return 1;
}

bool matrix_is_modified(void)
{
    if (debouncing) return false;
    return true;
}

inline
bool matrix_is_on(uint8_t row, uint8_t col)
{
    return (matrix[row] & ((matrix_row_t)1<<col));
}

inline
matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

void matrix_print(void)
{
    print("\nr/c 01234567\n");
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        phex(row); print(": ");
        pbin_reverse(matrix_get_row(row));
        print("\n");
    }
}

uint8_t matrix_key_count(void)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        count += bitpop16(matrix[i]);
    }
    return count;
}

static matrix_row_t read(uint8_t row)
{
	_delay_us(30);  // without this wait read unstable value.

	// only bother with row 13 since HWB mashes entire column
	if (row != 13) {
		return KC_NO;
	}
	
	if (~PIND > 0) PORTB |= _BV(PB4);
	else PORTB = 0x00;
	return ~PIND;

	//if (~PINB > 0) PORTD |= _BV(PD4);
	//else PORTD = 0x00;
	//return ~PINB;
}

static void unselect(void)
{
	//PORTB &= 0xF0;	// set lower 4 bits A,B,C,G to 0
}

static void select(uint8_t row)
{
	//PORTB |= row;
}

/* Row pin configuration
PB0		A
PB1		B
PB2		C
PB3		G	0 = U4, 1 = U5

				4y0	4y1	4y2	4y3	4y4	4y5	4y6	4y7	5y0	5y1	5y2	5y3	5y4	5y5	5y6	5y7	
				r1	r2	 r3 r4	r5	r6	r7	r8	r9	r10	r11	r12	r13	r14	r15	r16	
PD0		21	c1	f6	f8	f7	5	4	3	2	1	=+								
PD1		22	c2	f3	f5	f4	t	r	e	w	q	TAB								
PD2		23	c3	ESC	f2	f1	g	f	d	s	a	CL								
PD3		24	c4	f9	f11	f10	b	v	c	x	z	LS	UP		DN		[{	]}		
PD4		25	c5  f12	SL	PS	RT		LT	§±	`~		6	7	8		9	0	-_ 	
PD5		26	c6	PB	PGM	KPD							y	u	i		o	p	\	
PD6		27	c7  			LC	DL	BS	RC	EN	SP	h	j	k		l	;:	'"	
PD7		28	c8					RA		PU		PD	n	m	,<		.>	/?	RS	
 */


