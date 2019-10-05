#ifndef SOUND_H
#define SOUND_H

#include <Serial/SerialAVR.h>
#include <DFPlayerMini.h>

#define SOUND_STATE_STOP			0
#define SOUND_STATE_PLAY			1

//The folder number.  The folder should be 2 digits, e.g. "01"
#define SOUND_FOLDER_NUMBER			1
//The total number of files.  Files should be named with three digits, starting at 001.mp3, etc.
// Real DFPlayerMinis can query the number of files, but the cheap Chinese clones don't.  Lame Amazon.
#define SOUND_FILE_COUNT			12

void music_init();

void music_poll();

uint8_t music_get_volume();
void music_set_volume(int8_t volume);

void music_start();
void music_stop();
void music_toggle();
uint8_t music_is_playing();

#endif
