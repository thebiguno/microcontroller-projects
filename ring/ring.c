#include "ring.h"

void ring_init(volatile ring_t *ring, uint8_t size) {
	ring->buffer = (uint8_t *)malloc(sizeof(uint8_t[size]));
}

inline uint8_t ring_buffer_empty(volatile ring_t *ring){
	return (ring->head == ring->tail);
}

inline uint8_t ring_buffer_full(volatile ring_t *ring){
	return ((ring->head + 1) % ring->size == ring->tail);
}

inline char ring_buffer_get(volatile ring_t *ring){
	char c = ring->buffer[ring->tail];
	if (++ring->tail >= ring->size) ring->tail = 0;
	return c;
}

inline void ring_buffer_put(volatile ring_t *ring, char data){
	ring->buffer[ring->head] = data;
	if (++ring->head >= ring->size) ring->head = 0;
}