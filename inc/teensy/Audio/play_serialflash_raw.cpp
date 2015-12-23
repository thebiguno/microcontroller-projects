/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 * Modified to use SerialFlash instead of SD library by Wyatt Olson <wyatt@digitalcave.ca>
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "play_serialflash_raw.h"
#include "spi_interrupt.h"


void AudioPlaySerialflashRaw::begin(void)
{
	playing = false;
	file_offset = 0;
	file_size = 0;
}

#define B2M (uint32_t)((double)4294967296000.0 / AUDIO_SAMPLE_RATE_EXACT / 2.0) // 97352592

bool AudioPlaySerialflashRaw::play(const char *filename){
	return play(filename, 0);
}
bool AudioPlaySerialflashRaw::play(const char *filename, uint32_t positionMillis)
{
	stop();
	AudioStartUsingSPI();
	rawfile = SerialFlash.open(filename);
	if (!rawfile) {
		//Serial.println("unable to open file");
		AudioStopUsingSPI();
		return false;
	}
	file_size = rawfile.size();
	if (positionMillis == 0) file_offset = 0;
	else file_offset = (((uint64_t) positionMillis << 32) / B2M) & ~((uint32_t) ((AUDIO_BLOCK_SAMPLES * 2) - 1));
	//Serial.println("able to open file");
// 	Serial.print("File offset: ");
// 	Serial.println(file_offset);
	playing = true;
	
	if (file_offset > file_size){
		stop();
	}
	else {
		rawfile.seek(file_offset);
	}
	return true;
}

void AudioPlaySerialflashRaw::stop(void)
{
	__disable_irq();
	if (playing) {
		playing = false;
		__enable_irq();
		rawfile.close();
		AudioStopUsingSPI();
	} else {
		__enable_irq();
	}
}


void AudioPlaySerialflashRaw::update(void)
{
	unsigned int i, n;
	audio_block_t *block;

	// only update if we're playing
	if (!playing) return;

	// allocate the audio blocks to transmit
	block = allocate();
	if (block == NULL) return;

	if (rawfile.available()) {
		// we can read more data from the file...
		n = rawfile.read(block->data, AUDIO_BLOCK_SAMPLES*2);
		file_offset += n;
		for (i=n/2; i < AUDIO_BLOCK_SAMPLES; i++) {
			block->data[i] = 0;
		}
		transmit(block);
	} else {
		rawfile.close();
		AudioStopUsingSPI();
		playing = false;
		//Serial.println("Finished playing sample");		//TODO
	}
	release(block);
}

uint32_t AudioPlaySerialflashRaw::positionMillis(void)
{
	return ((uint64_t)file_offset * B2M) >> 32;
}

uint32_t AudioPlaySerialflashRaw::lengthMillis(void)
{
	return ((uint64_t)file_size * B2M) >> 32;
}


