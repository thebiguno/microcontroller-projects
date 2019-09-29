#include "Sound.h"

using namespace digitalcave;

extern SerialAVR serialAVR;

Sound::Sound() :
	mp3(&serialAVR),
	playbackState(SOUND_STATE_STOP)
{
	for (uint8_t i = 0; i < SOUND_FILE_COUNT; i++){
		queue[i] = (i + 1);
	}

	shuffleQueue();
}

void Sound::shuffleQueue(){
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

void Sound::poll(){
	while (uint8_t* response = mp3.poll()){
		if (playbackState == SOUND_STATE_PLAY && response[3] == DFPLAYER_RESPONSE_TRACK_DONE){
			currentFileIndex++;
			if (currentFileIndex >= SOUND_FILE_COUNT){
				currentFileIndex = 0;
			}

			mp3.sendCommand(DFPLAYER_COMMAND_FOLDER_SET, (SOUND_FOLDER_NUMBER << 8) + queue[currentFileIndex]);
		}
	}
}

uint8_t Sound::getVolume(){
	return volume;
}

void Sound::setVolume(uint8_t volume){
	if (volume > 30){
		volume = 30;
	}
	this->volume = volume;
	mp3.sendCommand(DFPLAYER_COMMAND_VOL_SET, volume);
}

void Sound::start(){
	shuffleQueue();
	mp3.sendCommand(DFPLAYER_COMMAND_FOLDER_SET, (SOUND_FOLDER_NUMBER << 8) + queue[currentFileIndex]);
	playbackState = SOUND_STATE_PLAY;
}

void Sound::stop(){
	mp3.sendCommand(DFPLAYER_COMMAND_PAUSE);
	playbackState = SOUND_STATE_STOP;
}

void Sound::toggle(){
	if (playbackState == SOUND_STATE_PLAY){
		stop();
	}
	else {
		start();
	}
}

uint8_t Sound::isPlaying(){
	return playbackState == SOUND_STATE_PLAY;
}

ISR(USART1_RX_vect){
	serialAVR.isr();
}
