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

static uint8_t read(uint8_t row);
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

static uint8_t read(uint8_t row)
{
	_delay_us(30);  // without this wait read unstable value.
	return ~PINB;
}

/* Row pin configuration
 *
 * row: 0   1   2   3   4   5   6   7   8   9
 * pin: D0  D1  D2  D3  F0  F1  F4  F5  F6  F7
 */
static void unselect(void)
{
    // hi-z (DDR:0, PORT:0)
	DDRD   &= ~(1<<0 | 1<<1 | 1<<2 | 1<<3);
	PORTD  &= ~(1<<0 | 1<<1 | 1<<2 | 1<<3);
	DDRF   &= ~(1<<0 | 1<<1 | 1<<4 | 1<<5 | 1<<6 | 1<<7);
	PORTF  &= ~(1<<0 | 1<<1 | 1<<4 | 1<<5 | 1<<6 | 1<<7);
}

static void select(uint8_t row)
{
    // Output low (DDR:1, PORT:0)
    switch (row) {
        case 0:
            DDRD |= (1<<0);
            break;
        case 1:
            DDRD |= (1<<1);
            break;
        case 2:
            DDRD |= (1<<2);
            break;
        case 3:
            DDRD |= (1<<3);
            break;
        case 4:
            DDRF |= (1<<0);
            break;
        case 5:
            DDRF |= (1<<1);
            break;
        case 6:
            DDRF |= (1<<4);
            break;
        case 7:
            DDRF |= (1<<5);
            break;
        case 8:
            DDRF |= (1<<6);
            break;
        case 9:
            DDRF |= (1<<7);
            break;
        }

}

