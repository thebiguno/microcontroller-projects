#ifndef DCUTIL_PERSIST
#define DCUTIL_PERSIST
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void persist_write(uint32_t address, uint8_t* data, uint16_t length);

void persist_read(uint32_t address, uint8_t* data, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif
