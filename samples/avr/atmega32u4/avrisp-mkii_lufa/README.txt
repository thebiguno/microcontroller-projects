Some random thoughts about this programmer:

1) It does not work with avrdude version 6.0.1 or 6.1.  Use version 5.x until avrdude is fixed.
2) This .hex version has been compiled to use PORTB4 as the ISP reset pin, and ADC0 (PORTF0) as the VCC voltage sense pin.
3) View the KiCad schematic for wiring details
4) To recompile, download the latest LUFA source, and go to the AVRISP-MKII folder.  The following files need to be changed:
	a) In makefile:
		MCU=atmega32u4
		F_CPU=16000000
	b) In Config/AppConfig.h:
		VTARGET_ADC_CHANNEL=0
	Run 'make' and it should just work.
