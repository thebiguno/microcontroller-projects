#include "music.h"

using namespace digitalcave;

static SerialAVR* serialAVR = NULL;

static uint8_t playbackState;
static uint8_t queue[SOUND_FILE_COUNT];
static uint8_t currentFileIndex;
static uint8_t volume;

void music_shuffle_queue();


void music_init(){
	serialAVR = new SerialAVR(9600, 8, 0, 1, 1);		//Serial Port 1 is the hardware serial port
	dfplayermini_init(serialAVR);
	playbackState = SOUND_STATE_STOP;
	volume = 0;

	for (uint8_t i = 0; i < SOUND_FILE_COUNT; i++){
		queue[i] = (i + 1);
	}

	music_shuffle_queue();
}

void music_shuffle_queue(){
	// Start from the last element and swap one by one. We don't
	// need to run for the first element that's why i > 0
	for (uint8_t i = SOUND_FILE_COUNT - 1; i > 0; i--){
		// Pick a random index from 0 to i
		uint8_t j = random() % (i + 1);

		// Swap arr[i] with the element at random index
		uint8_t temp = queue[i];
		queue[i] = queue[j];
		queue[j] = temp;
	}

	currentFileIndex = 0;
}

void music_poll(){
	while (uint8_t* response = dfplayermini_poll()){
		if (playbackState == SOUND_STATE_PLAY && response[3] == DFPLAYER_RESPONSE_TRACK_DONE){
			currentFileIndex++;
			if (currentFileIndex >= SOUND_FILE_COUNT){
				currentFileIndex = 0;
			}

			dfplayermini_send_command(DFPLAYER_COMMAND_FOLDER_SET, (SOUND_FOLDER_NUMBER << 8) + queue[currentFileIndex]);
		}
	}
}

uint8_t music_get_volume(){
	return volume;
}

void music_set_volume(int8_t _volume){
	if (_volume < 0){
		_volume = 0;
	}
	else if (_volume > 30){
		_volume = 30;
	}
	volume = _volume;
	dfplayermini_send_command(DFPLAYER_COMMAND_VOL_SET, volume);
}

void music_start(){
	music_set_volume(volume);
	music_shuffle_queue();
	dfplayermini_send_command(DFPLAYER_COMMAND_FOLDER_SET, (SOUND_FOLDER_NUMBER << 8) + queue[currentFileIndex]);
	playbackState = SOUND_STATE_PLAY;
}

void music_stop(){
	dfplayermini_send_command(DFPLAYER_COMMAND_PAUSE);
	playbackState = SOUND_STATE_STOP;
}

void music_toggle(){
	if (playbackState == SOUND_STATE_PLAY){
		music_stop();
	}
	else {
		music_start();
	}
}

uint8_t music_is_playing(){
	return playbackState == SOUND_STATE_PLAY;
}

ISR(USART1_RX_vect){
	if (serialAVR != NULL){
		serialAVR->isr();
	}
}
