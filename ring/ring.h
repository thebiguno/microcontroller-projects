#include <stdlib.h>
#include <stdint.h>

#ifndef RING_H
#define RING_H

typedef struct ring {
	//Just like a snake eating something and pooping it out...
	uint8_t size;
	volatile uint8_t head;  //You put data into head...
	volatile uint8_t tail;  //...and take it off of tail.
	// If they are equal, then there is nothing in the buffer.  
	// If (head + 1) % length == tail then the buffer is full.  
	// The current position of head points to the location where the next byte will
	// be written (and head will then be incremented after the byte is written); 
	// the position of tail points to the location of the last byte which was read,
	// and must be incremented before the next read.
	// NOTE:  You *cannot* use anything larger than uint8_t for the head / tail
	// indices; doing so will result in corrupt output, presumably because it
	// takes more than one instruction to do comparisons.
	volatile uint8_t *buffer;
} ring_t;


void ring_init(volatile ring_t *ring, uint8_t size);

uint8_t ring_buffer_empty(volatile ring_t *ring);

uint8_t ring_buffer_full(volatile ring_t *ring);

char ring_buffer_get(volatile ring_t *ring);

void ring_buffer_put(volatile ring_t *ring, char data);

#endif

