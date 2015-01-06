#ifndef BCD_H

inline uint8_t bcd2hex(uint8_t bcd) {
	return (bcd & 0xF) + ((bcd & 0xF0) >> 4) * 10;
}

inline uint8_t hex2bcd(uint8_t hex) {
	return ((hex / 10) << 4) + (hex % 10);
}

#endif