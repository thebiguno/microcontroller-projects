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

#ifdef DEBUG_MATRIX_FREQ
uint32_t matrix_timer;
uint32_t matrix_scan_count;
#endif

uint8_t test;

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

void matrix_init(void)
{
	// input with pull-up (DDR:0, PORT:1)
	DDRB    = 0x00;
	PORTB   = 0xFF;

    // initialize row and col
    unselect();

    // initialize matrix state: all keys off
    for (uint8_t i=0; i < MATRIX_ROWS; i++) {
        matrix[i] = 0;
        matrix_debouncing[i] = 0;
    }

#ifdef DEBUG_MATRIX_FREQ
    matrix_timer = timer_read32();
    matrix_scan_count = 0;
#endif
}

uint8_t matrix_scan(void)
{

#ifdef DEBUG_MATRIX_FREQ
    matrix_scan_count++;

    uint32_t timer_now = timer_read32();
    if (TIMER_DIFF_32(timer_now, matrix_timer)>1000) {
        print("matrix scan frequency: ");
        pdec(matrix_scan_count);
        print("\n");

        matrix_timer = timer_now;
        matrix_scan_count = 0;
    }
#endif

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
    return (matrix[row] & ((uint8_t)1<<col));
}

inline
matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

void matrix_print(void)
{
    print("\nr/c 0123456789ABCDEF\n");
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        phex(row); print(": ");
        pbin_reverse16(matrix_get_row(row));
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
	uint16_t result = ~PIND;
	uint16_t extras = ~PINB & 0x60;
	result |= (extras << 3);
	return ~PIND;
}

/* Row pin configuration
PB0		A
PB1		B
PB2		C
PB3		G	0 = U4, 1 = U5

				4y0	4y1	4y2	4y3	4y4	4y5	4y6	4y7	5y0	5y1	5y2	5y3	5y4	5y5	5y6	5y7	
				r1	r2	 r3 r4	r5	r6	r7	r8	r9	r10	r11	r12	r13	r14	r15	r16	
PC5		5									KP	KP								
PC4		6									PG	PG								
PD0		21	c1	f6	f8	f7	5	4	3	2	1	=+								
PD1		22	c2	f3	f5	f4	t	r	e	w	q	TAB								
PD2		23	c3	ESC	f2	f1	g	f	d	s	a	CL								
PD3		24	c4	f9	f11	f10	b	v	c	x	z	LS	UP		DN		[{	]}		
PD4		25	c5  f12	SL	PS	RT		LT	§±	`~		6	7	8		9	0	-_ 	
PD5		26	c6	PB									y	u	i		o	p	\	
PD6		27	c7  			LC	DL	BS	RC	EN	SP	h	j	k		l	;:	'"	
PD7		28	c8					RA		PU		PD	n	m	,<		.>	/?	RS	

 */
static void unselect(void)
{
	PORTB  &= 0xF0;
}

static void select(uint8_t row)
{
    // Output low (DDR:1, PORT:0)
	PORTB |= row;
}

