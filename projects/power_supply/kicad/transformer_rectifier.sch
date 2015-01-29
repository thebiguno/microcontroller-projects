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
Sheet 2 6
Title "Rectifier"
Date "28 jan 2015"
Rev "1"
Comp "Digital Cave"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text HLabel 900  1150 0    60   Input ~ 0
AC1
Text HLabel 900  2250 0    60   Input ~ 0
AC2
Text HLabel 5200 1300 2    60   Output ~ 0
V+
Text HLabel 5200 1450 2    60   Output ~ 0
V-
$Comp
L TRANSFORMER T?
U 1 1 54C7EE64
P 1300 1350
F 0 "T?" H 1300 1650 60  0000 C CNN
F 1 "TRANSFORMER" H 1300 1050 60  0000 C CNN
F 2 "" H 1300 1350 60  0000 C CNN
F 3 "" H 1300 1350 60  0000 C CNN
	1    1300 1350
	1    0    0    -1  
$EndComp
$Comp
L TRANSFORMER T?
U 1 1 54C7EE71
P 1300 2050
F 0 "T?" H 1300 2350 60  0000 C CNN
F 1 "TRANSFORMER" H 1300 1750 60  0000 C CNN
F 2 "" H 1300 2050 60  0000 C CNN
F 3 "" H 1300 2050 60  0000 C CNN
	1    1300 2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	900  1550 900  1850
Wire Wire Line
	1700 1550 1700 1850
Wire Wire Line
	1700 1700 1850 1700
Connection ~ 1700 1700
$Comp
L GND #PWR?
U 1 1 54C7EE84
P 1850 1700
F 0 "#PWR?" H 1850 1700 30  0001 C CNN
F 1 "GND" H 1850 1630 30  0001 C CNN
F 2 "" H 1850 1700 60  0000 C CNN
F 3 "" H 1850 1700 60  0000 C CNN
	1    1850 1700
	0    -1   -1   0   
$EndComp
$Comp
L BRIDGE D?
U 1 1 54C7EE97
P 3100 1700
F 0 "D?" H 3100 1750 70  0000 C CNN
F 1 "BRIDGE" H 3100 1650 70  0000 C CNN
F 2 "~" H 3100 1700 60  0000 C CNN
F 3 "~" H 3100 1700 60  0000 C CNN
	1    3100 1700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3950 1300 3950 1700
Wire Wire Line
	2250 1700 2250 2350
Wire Wire Line
	1800 850  3100 850 
Wire Wire Line
	1800 2550 3100 2550
Wire Wire Line
	1700 2250 1800 2250
Wire Wire Line
	1800 2250 1800 2550
Wire Wire Line
	1700 1150 1800 1150
Wire Wire Line
	1800 1150 1800 850 
Wire Wire Line
	3950 1300 5200 1300
Wire Wire Line
	4050 1450 4050 2350
Wire Wire Line
	4050 1450 5200 1450
$Comp
L CP1 C?
U 1 1 54C98489
P 4250 1850
F 0 "C?" H 4300 1950 50  0000 L CNN
F 1 "6800uF" V 4350 1500 50  0000 L CNN
F 2 "~" H 4250 1850 60  0000 C CNN
F 3 "~" H 4250 1850 60  0000 C CNN
	1    4250 1850
	1    0    0    -1  
$EndComp
$Comp
L CP1 C?
U 1 1 54C984D1
P 4500 1850
F 0 "C?" H 4550 1950 50  0000 L CNN
F 1 "6800uF" V 4600 1500 50  0000 L CNN
F 2 "~" H 4500 1850 60  0000 C CNN
F 3 "~" H 4500 1850 60  0000 C CNN
	1    4500 1850
	1    0    0    -1  
$EndComp
$Comp
L CP1 C?
U 1 1 54C984D7
P 4750 1850
F 0 "C?" H 4800 1950 50  0000 L CNN
F 1 "6800uF" V 4850 1500 50  0000 L CNN
F 2 "~" H 4750 1850 60  0000 C CNN
F 3 "~" H 4750 1850 60  0000 C CNN
	1    4750 1850
	1    0    0    1   
$EndComp
$Comp
L CP1 C?
U 1 1 54C984DD
P 5000 1850
F 0 "C?" H 5050 1950 50  0000 L CNN
F 1 "6800uF" V 5100 1500 50  0000 L CNN
F 2 "~" H 5000 1850 60  0000 C CNN
F 3 "~" H 5000 1850 60  0000 C CNN
	1    5000 1850
	1    0    0    1   
$EndComp
Wire Wire Line
	4250 1650 4250 1300
Connection ~ 4250 1300
Wire Wire Line
	4500 1650 4500 1300
Connection ~ 4500 1300
Wire Wire Line
	4250 2050 4250 2350
Wire Wire Line
	4250 2350 5000 2350
Wire Wire Line
	5000 2050 5000 2450
Wire Wire Line
	4750 2050 4750 2350
Connection ~ 4750 2350
Wire Wire Line
	4500 2050 4500 2350
Connection ~ 4500 2350
Wire Wire Line
	4050 2350 2250 2350
$Comp
L GND #PWR?
U 1 1 54C9856B
P 5000 2450
F 0 "#PWR?" H 5000 2450 30  0001 C CNN
F 1 "GND" H 5000 2380 30  0001 C CNN
F 2 "" H 5000 2450 60  0000 C CNN
F 3 "" H 5000 2450 60  0000 C CNN
	1    5000 2450
	1    0    0    -1  
$EndComp
Connection ~ 5000 2350
Wire Wire Line
	4750 1650 4750 1450
Connection ~ 4750 1450
Wire Wire Line
	5000 1650 5000 1450
Connection ~ 5000 1450
$EndSCHEMATC
