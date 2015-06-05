#include "Clock.h"

Clock::Clock() {
	
	
}

void Clock::tick() {
}

void Clock::run() {
	uint8_t mode = 1;
	
	while (mode > 0) {
		
		while (mode == 1) {
			// milliseconds to traditional (24:59:59)
			uint8_t hr = ms / 3600000;      // 1/24 day (hour)
			ms -= 3600000 * (uint32_t) hr;
			uint8_t mn = ms / 60000;        // 1/1440 day (minute)
			ms -= 60000 * (uint32_t) mn;
			uint8_t sc = ms / 1000;         // 1/86400 day (second)
		}

		while (mode == 2) {
			// milliseconds to vigesimal (19.19.19.19)
			// This method was used by the ancient Mayans
			uint8_t digits[4];
			digits[0] = ms / 4320000;       // 1/20 day = 1 h 12 m
			ms -= 4320000 * (uint32_t) digits[0];
			digits[1] = ms / 216000;        // 1/400 day = 3 m 36 s
			ms -= 216000 * (uint32_t) digits[1];
			digits[2] = ms / 10800;         // 1/8000 day = 10.8 s
			ms -= 10800 * (uint32_t) digits[2];
			digits[3] = ms / 540;           // 1/160000 day = .54 s
		}
		
		while (mode == 3) {
			uint8_t digits[4];
			//milliseconds to hexadecimal (F_FF_F)
			// This method was first proposed in the 1850s by John W. Nystrom
			digits[0] = ms / 5400000;               // 1/16 day (hex hour) = 1 h 30 m
			ms -= 5400000 * (uint32_t) digits[0];
			digits[1] = ms / 337500;                // 1/256 day (hex maxime) = 5 m 37.5 s
			ms -= 337500 * (uint32_t) digits[1];
			ms *= 100;                                              // bump up the precision
			digits[2] = ms / 2109375;               // 1/4096 day (hex minute) ~= 21 seconds
			ms -= 2109375 * (uint32_t) digits[2];
			ms *= 100;                                              // bump up the precision again
			digits[3] = ms / 13183593;              // 1/65536 day (hex second) ~= 1.32 seconds
		}
		
		while (mode == 4) {
			uint8_t digits[5];
			//milliseconds to dozenal (BBB.BB)
			digits[0] = ms / 7200000;       // 1/12 day = 2 h
			ms -= 7200000 * (uint32_t) digits[0];
			digits[1] = ms / 600000;        // 1/144 day = 10 m
			ms -= 600000 * (uint32_t) digits[1];
			digits[2] = ms / 50000;         // 1/1728 day = 50 s
			ms -= 50000 * (uint32_t) digits[2];
			digits[3] = ms / 4167;          // 1/20736 day ~= 4.167 s
			ms -= 4167 * (uint32_t) digits[3];
			digits[4] = ms / 347;           // 1/248832 day ~= .347 s
		}
		
		while (mode == 5) {
			uint8_t digits[5];
			//milliseconds to decimal (999.99)
			digits[0] = ms / 8640000;       // 1/10 day (deciday) = 2 h 24 m
			ms -= 8640000 * (uint32_t) digits[0];
			digits[1] = ms / 864000;        // 1/100 day (centiday) = 14 m 24 s
			ms -= 864000 * (uint32_t) digits[1];
			digits[2] = ms / 86400;         // 1/1000 day (milliday; beat) = 1 m 26.4 s
			ms -= 86400 * (uint32_t) digits[2];
			digits[3] = ms / 8640;          // 1/10000 day (decibeat)= 8.64 s
			ms -= 8640 * (uint32_t) digits[3];
			digits[4] = ms / 864;           // 1/100000 day (centibeat) / .864 s
		}
		
		while (mode == 6) {
			// milliseconds to octal (777.777)
			uint8_t digits[6];
			digits[0] = ms / 10800000;              // 1/8 day = 3 h
			ms -= 10800000 * (uint32_t) digits[0] ;
			digits[1] = ms / 1350000;               // 1/64 day = 22 m 30 s
			ms -= 1350000 * (uint32_t) digits[1];
			digits[2] = ms / 168750;                // 1/512 day ~= 2 m 49 s
			ms -= 168750 * (uint32_t) digits[2];
			ms *= 100;                                              // bump up the precision
			digits[3] = ms / 2109375;               // 1/4096 day ~= 21 s
			ms -= 2109375 * (uint32_t) digits[3];
			ms *= 100;                                              // bump up the precision again
			digits[4] = ms / 26367187;              // 1/32768 day ~= 2.63 s
			ms -= 26367187 * (uint32_t) digits[4];
			ms *= 100;                                              // bump up the precision again
			digits[5] = ms / 329589843;             // 1/262144 day ~= .329 s
		}
	}
	

