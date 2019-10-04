#ifndef SOUND_H
#define SOUND_H

#include <Serial/SerialAVR.h>
#include <DFPlayerMini.h>

#ifdef DEBUG
#include <SerialUSB.h>
#endif

#define SOUND_STATE_STOP			0
#define SOUND_STATE_PLAY			1

//The folder number.  The folder should be 2 digits, e.g. "01"
#define SOUND_FOLDER_NUMBER			1
//The total number of files.  Files should be named with three digits, starting at 001.mp3, etc.
// Real DFPlayerMinis can query the number of files, but the cheap Chinese clones don't.  Lame Amazon.
#define SOUND_FILE_COUNT			12

namespace digitalcave {

	class Sound {
		private:
			DFPlayerMini mp3;

			uint8_t queue[SOUND_FILE_COUNT];
			uint8_t playbackState;
			uint8_t volume;
			uint8_t currentFileIndex;

			void shuffleQueue();

		public:
			Sound();

			void poll();

			uint8_t getVolume();
			void setVolume(int8_t volume);

			void start();
			void stop();
			void toggle();
			uint8_t isPlaying();
	};
}

#endif
