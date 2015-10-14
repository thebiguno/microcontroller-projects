#ifndef HSV_H
#define HSV_H

#include <stdint.h>

namespace digitalcave {
	class Hsv {
	private:
		uint16_t h;
		uint8_t s;
		uint8_t v;
	
	public:
		Hsv(uint16_t h);
		Hsv(uint16_t h, uint8_t s, uint8_t v);
		Hsv(const Hsv &c);
		~Hsv();
		
		void setHue(uint16_t h);
		uint16_t getHue() const;
		void addHue(int16_t h);

		void setSaturation(uint8_t s);
		uint8_t getSaturation() const;
		void addSaturation(int8_t s);

		void setValue(uint8_t v);
		uint8_t getValue() const;
		void addValue(int8_t v);
	};
}

#endif
