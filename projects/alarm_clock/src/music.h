#ifndef SOUND_H
#define SOUND_H

#include <Serial/SerialAVR.h>
#include <DFPlayerMini.h>
#include <RDA5807.h>
#include <I2CAVR.h>

#define SOUND_STATE_STOP			0
#define SOUND_STATE_PLAY_MP3		1
#define SOUND_STATE_PLAY_FM			2

//The total number of files.  Files should be named with three digits, starting at 001.mp3, etc.
// Real DFPlayerMinis can query the number of files, but the cheap Chinese clones don't.  Lame Amazon.
#define MAX_SOUND_FILE_COUNT		99

void music_init();

void music_poll();

void music_set_volume(int8_t volume);

void music_start_mp3(uint8_t folder, uint8_t file_count);
void music_stop();
void music_toggle_mp3(uint8_t folder, uint8_t file_count);
uint8_t music_is_playing_mp3();

#endif
