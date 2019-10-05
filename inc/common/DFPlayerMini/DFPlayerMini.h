/*
 * Library for using DFPlayer Mini chip.
 */

#ifndef DFPLAYER_MINI_H
#define DFPLAYER_MINI_H

#include <stdlib.h>
#include <math.h>
#include <dctypes.h>
#include <dcutil/delay.h>
#include <Stream/ArrayStream.h>
#include <Stream/Stream.h>

#define DFPLAYER_COMMAND_NEXT			0x01
#define DFPLAYER_COMMAND_PREVIOUS		0x02
#define DFPLAYER_COMMAND_TRACK			0x03
#define DFPLAYER_COMMAND_VOL_UP			0x04
#define DFPLAYER_COMMAND_VOL_DOWN		0x05
#define DFPLAYER_COMMAND_VOL_SET		0x06
#define DFPLAYER_COMMAND_EQ_SET			0x07
#define DFPLAYER_COMMAND_MODE_SET		0x08
#define DFPLAYER_COMMAND_SOURCE_SET		0x09
#define DFPLAYER_COMMAND_STANDBY_ON		0x0A
#define DFPLAYER_COMMAND_STANDBY_OFF	0x0B
#define DFPLAYER_COMMAND_RESET			0x0C
#define DFPLAYER_COMMAND_PLAY			0x0D
#define DFPLAYER_COMMAND_PAUSE			0x0E
#define DFPLAYER_COMMAND_FOLDER_SET		0x0F
#define DFPLAYER_COMMAND_GAIN_SET		0x0A
#define DFPLAYER_COMMAND_REPEAT_SET		0x0B

#define DFPLAYER_RESPONSE_TRACK_DONE	0x3D
#define DFPLAYER_COMMAND_GET_INIT_PARM	0x3F
#define DFPLAYER_COMMAND_GET_STATUS		0x42
#define DFPLAYER_COMMAND_GET_VOLUME		0x43
#define DFPLAYER_COMMAND_GET_EQ			0x44
#define DFPLAYER_COMMAND_GET_MODE		0x45
#define DFPLAYER_COMMAND_GET_SW_VER		0x46
#define DFPLAYER_COMMAND_GET_FILE_COUNT	0x47

//Resets the module and performs initial setup
void dfplayermini_init(digitalcave::Stream* serial);

uint8_t dfplayermini_send_command(uint8_t command, uint16_t arg = 0x00);

//Looks for a response.  If there is one, return the response object.  Otherwise, return NULL.
uint8_t* dfplayermini_poll();

#endif
