/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
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

// Plays from Serial Flash (c) Frank Bösing, 2014/12, 2015

#ifndef play_serialflash_h_
#define play_serialflash_h_
/*
	Set AUDIOBOARD to 1 if you use the PJRC-Audioboard, else 0
*/
#define AUDIOBOARD 1

#define SERFLASH_CS 			6	//Chip Select pin W25Q128FV SPI Flash

#include <Arduino.h>
#include <AudioStream.h>
#include <SPI.h>
#include <SPIFIFO.h>
#include "spi_interrupt.h"

class AudioPlaySerialFlash : public AudioStream
{
public:
	AudioPlaySerialFlash(void) : AudioStream(0, NULL), playing(0) { flashinit(); }
	void play(const unsigned int data);
	//void loop(const unsigned int data);
	void stop(void);
	bool isPlaying(void);
	bool pause(bool _paused);
	uint32_t position(void);
	uint32_t positionMillis(void);
	uint32_t length(void);
	uint32_t lengthMillis(void);
//	void setPosition(const unsigned int n);
	void setPositionMillis(const unsigned int millis);
	virtual void update(void);
protected:
	void flashinit(void);	
	inline void readSerStart(const size_t position) __attribute__((always_inline));
	inline void readSerDone(void) __attribute__((always_inline));
private:
	SPISettings spisettings;
	unsigned int sample;
	unsigned int beginning;
	unsigned int len;
	unsigned int b16; //Flag: 1=16-Bit Samples, 0 = 8-Bit Samples
	unsigned short prior;	
	volatile uint8_t playing;
	volatile bool paused;	
	inline uint32_t b2m(void) __attribute__((always_inline));
	inline uint32_t calcMillis(uint32_t position) __attribute__((always_inline));
	int SamplesConsumedPerUpdate(void);
};

#endif
