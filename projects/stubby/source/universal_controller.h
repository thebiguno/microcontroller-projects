#ifndef UNIVERSAL_CONTROLLER_CLIENT_STUBBY_H
#define UNIVERSAL_CONTROLLER_CLIENT_STUBBY_H

#include "lib/universal_controller/client.h"

/*
 * Called from the Stubby protocol dispatch function; handles the UC-specific dispatching
 */
void uc_dispatch_message(uint8_t cmd, uint8_t *message, uint8_t length);

#endif