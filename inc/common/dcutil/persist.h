#ifndef DCUTIL_PERSIST
#define DCUTIL_PERSIST
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Returns 0 for success, non-zero for failure.  Implementations which do not
 * return information on success should always return 0.
 */
uint8_t persist_write(uint32_t address, uint8_t* data, uint16_t length);

/*
 * Returns 0 for success, non-zero for failure.  Implementations which do not
 * return information on success should always return 0.
 */
uint8_t persist_read(uint32_t address, uint8_t* data, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif
