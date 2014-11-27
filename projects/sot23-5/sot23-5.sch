EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:custom
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date "17 nov 2014"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L NCP1400A VR1
U 1 1 546A2CBD
P 4900 3300
F 0 "VR1" H 4900 3150 60  0000 C CNN
F 1 "NCP1400A" H 4900 3050 60  0000 C CNN
F 2 "~" H 4900 3300 60  0000 C CNN
F 3 "~" H 4900 3300 60  0000 C CNN
	1    4900 3300
	1    0    0    -1  
$EndComp
$Comp
L CONN_5 P1
U 1 1 546A2CCC
P 6300 3300
F 0 "P1" V 6250 3300 50  0000 C CNN
F 1 "CONN_5" V 6350 3300 50  0000 C CNN
F 2 "" H 6300 3300 60  0000 C CNN
F 3 "" H 6300 3300 60  0000 C CNN
	1    6300 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	5400 3100 5900 3100
Wire Wire Line
	5400 3200 5900 3200
Wire Wire Line
	5400 3350 5400 3300
Wire Wire Line
	5400 3300 5900 3300
Wire Wire Line
	4400 3350 4400 3650
Wire Wire Line
	4400 3650 5500 3650
Wire Wire Line
	5500 3650 5500 3400
Wire Wire Line
	5500 3400 5900 3400
Wire Wire Line
	4400 3100 4300 3100
Wire Wire Line
	4300 3100 4300 3750
Wire Wire Line
	4300 3750 5600 3750
Wire Wire Line
	5600 3750 5600 3500
Wire Wire Line
	5600 3500 5900 3500
$Comp
L RGB_LED_COMMON_ANODE_ALT1 D1
U 1 1 546A2F71
P 4900 2350
F 0 "D1" H 4950 2600 50  0000 C CNN
F 1 "RGB_LED_COMMON_ANODE_ALT1" H 4900 2000 50  0000 C CNN
F 2 "~" H 4950 2350 60  0000 C CNN
F 3 "~" H 4950 2350 60  0000 C CNN
	1    4900 2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 2500 5600 2500
Wire Wire Line
	5600 2500 5600 3400
Connection ~ 5600 3400
Wire Wire Line
	5150 2350 5700 2350
Wire Wire Line
	5700 2350 5700 3300
Connection ~ 5700 3300
Wire Wire Line
	5150 2200 5800 2200
Wire Wire Line
	5800 2200 5800 3200
Connection ~ 5800 3200
Wire Wire Line
	4600 2350 4600 2000
Wire Wire Line
	4600 2000 5900 2000
Wire Wire Line
	5900 2000 5900 3100
$EndSCHEMATC
