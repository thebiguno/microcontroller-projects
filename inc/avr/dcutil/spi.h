#if defined(__AVR_ATmega48__)   || \
  defined(__AVR_ATmega48P__)    || \
	defined(__AVR_ATmega168__)    || \
	defined(__AVR_ATmega328__)    || \
	defined(__AVR_ATmega328P__)

#define DD_MOSI 3
#define DD_MISO 4
#define DD_SCK 5
#define DD_SS 2
#define DDR_SPI DDRB

#elif defined(__AVR_ATmega324P__) || \
	defined(__AVR_ATmega644__)      || \
	defined(__AVR_ATmega644P__)     || \
	defined(__AVR_ATmega644PA__)    || \
	defined(__AVR_ATmega1284P__)

#define DD_MOSI 5
#define DD_MISO 6
#define DD_SCK 7
#define DD_SS 4
#define DDR_SPI DDRB

#elif defined(__AVR_ATmega16U4__) || \
	defined(__AVR_ATmega32U4__) || \
	defined(__AVR_ATmega16U2__) || \
	defined(__AVR_ATmega32U2__)
#define DD_MOSI 2
#define DD_MISO 3
#define DD_SCK 1
#define DD_SS 0
#define DDR_SPI DDRB

#endif
