#ifndef RGB_H
#define RGB_H

#include <stdint.h>
#include "Hsv.h"

namespace digitalcave {
	class Rgb {
	private:
		uint8_t r;
		uint8_t g;
		uint8_t b;
	
	public:
		Rgb(uint8_t r, uint8_t g, uint8_t b);
		Rgb(const Rgb &c);
		Rgb(const Hsv &c);
		~Rgb();
		
		void setRed(uint8_t r);
		uint8_t getRed() const;
		void addRed(int8_t r);

		void setGreen(uint8_t s);
		uint8_t getGreen() const;
		void addGreen(int8_t g);

		void setBlue(uint8_t b);
		uint8_t getBlue() const;
		void addBlue(int8_t b);
	};
}

#endif