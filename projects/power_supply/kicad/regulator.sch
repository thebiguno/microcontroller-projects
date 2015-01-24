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
$Descr User 8000 8000
encoding utf-8
Sheet 4 7
Title "Power Regulator"
Date "24 jan 2015"
Rev "1"
Comp "Digital Cave"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text HLabel 2275 1450 1    60   Input ~ 0
V+
Text HLabel 2275 4000 3    60   Input ~ 0
V-
Text HLabel 5950 2750 2    60   Output ~ 0
REGULATED_V
Text HLabel 1825 3600 0    60   BiDi ~ 0
SDA
Text HLabel 1825 3900 0    60   BiDi ~ 0
SCL
Text HLabel 5350 1950 0    60   Output ~ 0
TEMP_OUT
Wire Wire Line
	2275 3250 2275 4000
Wire Wire Line
	2275 1450 2275 2200
Wire Wire Line
	1975 2100 2275 2100
Connection ~ 2275 3350
Connection ~ 2275 2100
$Comp
L CSMALL C?
U 1 1 54C383F3
P 2125 3350
F 0 "C?" H 2150 3400 30  0000 L CNN
F 1 "0.01uF" H 2150 3300 30  0000 L CNN
F 2 "~" H 2125 3350 60  0000 C CNN
F 3 "~" H 2125 3350 60  0000 C CNN
	1    2125 3350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2225 3350 2275 3350
Wire Wire Line
	2025 3350 1975 3350
Wire Wire Line
	1975 3350 1975 2100
$Comp
L CSMALL C?
U 1 1 54C38415
P 2575 1900
F 0 "C?" H 2600 1950 30  0000 L CNN
F 1 "0.1uF" H 2600 1850 30  0000 L CNN
F 2 "~" H 2575 1900 60  0000 C CNN
F 3 "~" H 2575 1900 60  0000 C CNN
	1    2575 1900
	0    -1   -1   0   
$EndComp
$Comp
L CP1 C?
U 1 1 54C3844D
P 2575 1650
F 0 "C?" H 2625 1750 50  0000 L CNN
F 1 "10uF" H 2625 1550 50  0000 L CNN
F 2 "~" H 2575 1650 60  0000 C CNN
F 3 "~" H 2575 1650 60  0000 C CNN
	1    2575 1650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2375 1650 2275 1650
Connection ~ 2275 1650
Wire Wire Line
	2475 1900 2275 1900
Connection ~ 2275 1900
Wire Wire Line
	2675 1900 2875 1900
Wire Wire Line
	2875 1650 2875 2000
Wire Wire Line
	2875 1650 2775 1650
Connection ~ 2875 1900
$Comp
L GND #PWR?
U 1 1 54C384AE
P 2875 2000
F 0 "#PWR?" H 2875 2000 30  0001 C CNN
F 1 "GND" H 2875 1930 30  0001 C CNN
F 2 "" H 2875 2000 60  0000 C CNN
F 3 "" H 2875 2000 60  0000 C CNN
	1    2875 2000
	1    0    0    -1  
$EndComp
$Comp
L CSMALL C?
U 1 1 54C384CF
P 2575 3800
F 0 "C?" H 2600 3850 30  0000 L CNN
F 1 "0.1uF" H 2600 3750 30  0000 L CNN
F 2 "~" H 2575 3800 60  0000 C CNN
F 3 "~" H 2575 3800 60  0000 C CNN
	1    2575 3800
	0    -1   -1   0   
$EndComp
$Comp
L CP1 C?
U 1 1 54C384D5
P 2575 3550
F 0 "C?" H 2625 3650 50  0000 L CNN
F 1 "10uF" H 2625 3450 50  0000 L CNN
F 2 "~" H 2575 3550 60  0000 C CNN
F 3 "~" H 2575 3550 60  0000 C CNN
	1    2575 3550
	0    1    -1   0   
$EndComp
Wire Wire Line
	2375 3550 2275 3550
Wire Wire Line
	2475 3800 2275 3800
Wire Wire Line
	2675 3800 2875 3800
Wire Wire Line
	2875 3550 2875 3900
Wire Wire Line
	2875 3550 2775 3550
Connection ~ 2875 3800
$Comp
L GND #PWR?
U 1 1 54C384E1
P 2875 3900
F 0 "#PWR?" H 2875 3900 30  0001 C CNN
F 1 "GND" H 2875 3830 30  0001 C CNN
F 2 "" H 2875 3900 60  0000 C CNN
F 3 "" H 2875 3900 60  0000 C CNN
	1    2875 3900
	1    0    0    -1  
$EndComp
$Comp
L RESISTOR R?
U 1 1 54C384FC
P 4250 2750
F 0 "R?" V 4340 2745 40  0000 C CNN
F 1 "0.1" V 4165 2750 40  0000 C CNN
F 2 "~" V 4180 2750 30  0000 C CNN
F 3 "~" H 4250 2750 30  0000 C CNN
	1    4250 2750
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3050 2750 4000 2750
$Comp
L INA169 U?
U 1 1 54C38E43
P 3700 3750
F 0 "U?" V 3600 3650 60  0000 C CNN
F 1 "INA169" V 3500 3550 60  0000 C CNN
F 2 "" H 3700 3750 60  0000 C CNN
F 3 "" H 3700 3750 60  0000 C CNN
	1    3700 3750
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3500 2750 3500 3050
$Comp
L +5V #PWR?
U 1 1 54C38F24
P 4250 3550
F 0 "#PWR?" H 4250 3640 20  0001 C CNN
F 1 "+5V" H 4250 3640 30  0000 C CNN
F 2 "" H 4250 3550 60  0000 C CNN
F 3 "" H 4250 3550 60  0000 C CNN
	1    4250 3550
	1    0    0    -1  
$EndComp
Connection ~ 3500 2750
Wire Wire Line
	4500 2750 5950 2750
Wire Wire Line
	5000 3050 5000 2750
Connection ~ 5000 2750
Wire Wire Line
	3900 3050 3900 2950
Wire Wire Line
	3900 2950 4600 2950
Wire Wire Line
	4600 2950 4600 2750
Connection ~ 4600 2750
Wire Wire Line
	4600 3050 4000 3050
Wire Wire Line
	4000 3050 4000 2850
Wire Wire Line
	4000 2850 3900 2850
Wire Wire Line
	3900 2850 3900 2750
Connection ~ 3900 2750
Wire Wire Line
	4200 3700 4300 3700
Wire Wire Line
	4250 3700 4250 3550
Connection ~ 4250 3700
$Comp
L INA169 U?
U 1 1 54C38E57
P 4800 3750
F 0 "U?" V 4700 3650 60  0000 C CNN
F 1 "INA169" V 4600 3550 60  0000 C CNN
F 2 "" H 4800 3750 60  0000 C CNN
F 3 "" H 4800 3750 60  0000 C CNN
	1    4800 3750
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 54C3912E
P 3750 4350
F 0 "#PWR?" H 3750 4350 30  0001 C CNN
F 1 "GND" H 3750 4280 30  0001 C CNN
F 2 "" H 3750 4350 60  0000 C CNN
F 3 "" H 3750 4350 60  0000 C CNN
	1    3750 4350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 54C39134
P 4750 4350
F 0 "#PWR?" H 4750 4350 30  0001 C CNN
F 1 "GND" H 4750 4280 30  0001 C CNN
F 2 "" H 4750 4350 60  0000 C CNN
F 3 "" H 4750 4350 60  0000 C CNN
	1    4750 4350
	1    0    0    -1  
$EndComp
Text Notes 3650 2450 0    40   ~ 0
Bipolar current measurement mode - \nsee page 7 of INA169 datasheet \nhttp://www.adafruit.com/datasheets/ina169.pdf
$Comp
L OPA548 U?
U 1 1 54C39345
P 2400 2750
F 0 "U?" H 2750 2900 60  0000 C CNN
F 1 "OPA548" H 2825 3025 60  0000 C CNN
F 2 "" H 2200 2750 60  0000 C CNN
F 3 "http://www.ti.com/lit/ds/sbos070b/sbos070b.pdf_(http://www.ti.com/lit/ds/sbos056f/sbos056f.pdf_for_OP547)" H 2200 2750 60  0001 C CNN
	1    2400 2750
	1    0    0    -1  
$EndComp
$EndSCHEMATC
