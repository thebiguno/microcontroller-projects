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
LIBS:power_supply-cache
EELAYER 27 0
EELAYER END
$Descr User 6000 4000
encoding utf-8
Sheet 4 6
Title "Rectifier"
Date "28 jan 2015"
Rev "1"
Comp "Digital Cave"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text HLabel 1550 1150 0    60   Input ~ 0
AC1
Text HLabel 1550 2250 0    60   Input ~ 0
AC2
Text HLabel 4600 1600 1    60   Output ~ 0
V+
Text HLabel 2900 1800 3    60   Output ~ 0
V-
$Comp
L TRANSFORMER T?
U 1 1 54C7EE64
P 1950 1350
F 0 "T?" H 1950 1650 60  0000 C CNN
F 1 "TRANSFORMER" H 1950 1050 60  0000 C CNN
F 2 "" H 1950 1350 60  0000 C CNN
F 3 "" H 1950 1350 60  0000 C CNN
	1    1950 1350
	1    0    0    -1  
$EndComp
$Comp
L TRANSFORMER T?
U 1 1 54C7EE71
P 1950 2050
F 0 "T?" H 1950 2350 60  0000 C CNN
F 1 "TRANSFORMER" H 1950 1750 60  0000 C CNN
F 2 "" H 1950 2050 60  0000 C CNN
F 3 "" H 1950 2050 60  0000 C CNN
	1    1950 2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	1550 1550 1550 1850
Wire Wire Line
	2350 1550 2350 1850
Wire Wire Line
	2350 1700 2500 1700
Connection ~ 2350 1700
$Comp
L GND #PWR?
U 1 1 54C7EE84
P 2500 1700
F 0 "#PWR?" H 2500 1700 30  0001 C CNN
F 1 "GND" H 2500 1630 30  0001 C CNN
F 2 "" H 2500 1700 60  0000 C CNN
F 3 "" H 2500 1700 60  0000 C CNN
	1    2500 1700
	0    -1   -1   0   
$EndComp
$Comp
L BRIDGE D?
U 1 1 54C7EE97
P 3750 1700
F 0 "D?" H 3750 1750 70  0000 C CNN
F 1 "BRIDGE" H 3750 1650 70  0000 C CNN
F 2 "~" H 3750 1700 60  0000 C CNN
F 3 "~" H 3750 1700 60  0000 C CNN
	1    3750 1700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4600 1600 4600 1700
Wire Wire Line
	2900 1800 2900 1700
Wire Wire Line
	3750 850  3050 850 
Wire Wire Line
	3050 850  3050 1150
Wire Wire Line
	3050 1150 2350 1150
Wire Wire Line
	3750 2550 3050 2550
Wire Wire Line
	3050 2550 3050 2250
Wire Wire Line
	3050 2250 2350 2250
$EndSCHEMATC
