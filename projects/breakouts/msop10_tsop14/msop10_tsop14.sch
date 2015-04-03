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
$Descr User 8000 6000
encoding utf-8
Sheet 1 1
Title "MSSOP_10 / TSSOP_14 Breakout"
Date "3 apr 2015"
Rev "1"
Comp "Digital Cave"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L PINS_7 K1
U 1 1 551EDA37
P 1700 1900
F 0 "K1" H 1700 1500 60  0000 C CNN
F 1 "PINS_7" H 1700 2300 60  0000 C CNN
F 2 "~" H 1700 2200 60  0000 C CNN
F 3 "~" H 1700 2200 60  0000 C CNN
	1    1700 1900
	1    0    0    -1  
$EndComp
$Comp
L PINS_7 K2
U 1 1 551EDA49
P 1700 2600
F 0 "K2" H 1700 2200 60  0000 C CNN
F 1 "PINS_7" H 1700 3000 60  0000 C CNN
F 2 "~" H 1700 2900 60  0000 C CNN
F 3 "~" H 1700 2900 60  0000 C CNN
	1    1700 2600
	1    0    0    -1  
$EndComp
$Comp
L PINS_10 K3
U 1 1 551EDA53
P 3350 3850
F 0 "K3" H 3350 3300 60  0000 C CNN
F 1 "MSSOP_10" H 3350 4400 60  0000 C CNN
F 2 "~" H 3350 4300 60  0000 C CNN
F 3 "~" H 3350 4300 60  0000 C CNN
	1    3350 3850
	0    -1   -1   0   
$EndComp
$Comp
L PINS_14 K4
U 1 1 551EDA62
P 5500 3850
F 0 "K4" H 5500 3100 60  0000 C CNN
F 1 "TSSOP_14" H 5500 4600 60  0000 C CNN
F 2 "~" H 5500 4500 60  0000 C CNN
F 3 "~" H 5500 4500 60  0000 C CNN
	1    5500 3850
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2000 1600 4850 1600
Wire Wire Line
	4850 1600 4850 3550
Wire Wire Line
	2000 1700 4950 1700
Wire Wire Line
	4950 1700 4950 3550
Wire Wire Line
	2000 1800 5050 1800
Wire Wire Line
	5050 1800 5050 3550
Wire Wire Line
	2000 1900 5150 1900
Wire Wire Line
	5150 1900 5150 3550
Wire Wire Line
	2000 2000 5250 2000
Wire Wire Line
	5250 2000 5250 3550
Wire Wire Line
	2000 2100 5350 2100
Wire Wire Line
	5350 2100 5350 3550
Wire Wire Line
	2000 2200 5450 2200
Wire Wire Line
	5450 2200 5450 3550
Wire Wire Line
	2000 2300 5550 2300
Wire Wire Line
	5550 2300 5550 3550
Wire Wire Line
	2000 2400 5650 2400
Wire Wire Line
	5650 2400 5650 3550
Wire Wire Line
	2000 2500 5750 2500
Wire Wire Line
	5750 2500 5750 3550
Wire Wire Line
	2000 2600 5850 2600
Wire Wire Line
	5850 2600 5850 3550
Wire Wire Line
	2000 2700 5950 2700
Wire Wire Line
	5950 2700 5950 3550
Wire Wire Line
	2000 2800 6050 2800
Wire Wire Line
	6050 2800 6050 3550
Wire Wire Line
	2000 2900 6150 2900
Wire Wire Line
	6150 2900 6150 3550
Wire Wire Line
	2900 3550 2900 2900
Connection ~ 2900 2900
Wire Wire Line
	3000 3550 3000 2800
Connection ~ 3000 2800
Wire Wire Line
	3100 3550 3100 2700
Connection ~ 3100 2700
Wire Wire Line
	3200 3550 3200 2600
Connection ~ 3200 2600
Wire Wire Line
	3300 3550 3300 2500
Connection ~ 3300 2500
Wire Wire Line
	3800 3550 3800 1600
Connection ~ 3800 1600
Wire Wire Line
	3700 3550 3700 1700
Connection ~ 3700 1700
Wire Wire Line
	3600 3550 3600 1800
Connection ~ 3600 1800
Wire Wire Line
	3500 3550 3500 1900
Connection ~ 3500 1900
Wire Wire Line
	3400 3550 3400 2000
Connection ~ 3400 2000
$EndSCHEMATC
