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
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date "23 mar 2014"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L ATMEGA1284P-P IC?
U 1 1 532C95F4
P 3250 3650
F 0 "IC?" H 2400 5530 40  0000 L BNN
F 1 "ATMEGA1284P-P" H 3650 1700 40  0000 L BNN
F 2 "DIL40" H 3250 3650 30  0000 C CIN
F 3 "" H 3250 3650 60  0000 C CNN
	1    3250 3650
	1    0    0    -1  
$EndComp
$Comp
L CONN_9 P?
U 1 1 532C96F2
P 7350 2300
F 0 "P?" V 7300 2300 60  0000 C CNN
F 1 "CONN_9" V 7400 2300 60  0000 C CNN
F 2 "" H 7350 2300 60  0000 C CNN
F 3 "" H 7350 2300 60  0000 C CNN
	1    7350 2300
	1    0    0    -1  
$EndComp
$Comp
L CONN_9 P?
U 1 1 532C9705
P 7350 3200
F 0 "P?" V 7300 3200 60  0000 C CNN
F 1 "CONN_9" V 7400 3200 60  0000 C CNN
F 2 "" H 7350 3200 60  0000 C CNN
F 3 "" H 7350 3200 60  0000 C CNN
	1    7350 3200
	1    0    0    -1  
$EndComp
Text GLabel 4250 1950 2    60   Input ~ 0
PWM00
Text GLabel 4250 2050 2    60   Input ~ 0
PWM01
Text GLabel 4250 2150 2    60   Input ~ 0
PWM02
Text GLabel 4250 2250 2    60   Input ~ 0
PWM03
Text GLabel 4250 2350 2    60   Input ~ 0
PWM04
Text GLabel 4250 2450 2    60   Input ~ 0
PWM05
Text GLabel 4250 2550 2    60   Input ~ 0
PWM06
Text GLabel 4250 2850 2    60   Input ~ 0
PWM07
Text GLabel 4250 4050 2    60   Input ~ 0
PWM15
Text GLabel 4250 4150 2    60   Input ~ 0
PWM14
Text GLabel 4250 4250 2    60   Input ~ 0
PWM13
Text GLabel 4250 4350 2    60   Input ~ 0
PWM12
Text GLabel 4250 4450 2    60   Input ~ 0
PWM11
Text GLabel 4250 3150 2    60   Input ~ 0
PWM10
Text GLabel 4250 3050 2    60   Input ~ 0
PWM09
Text GLabel 4250 2950 2    60   Input ~ 0
PWM08
Text GLabel 4250 3250 2    60   Input ~ 0
PWM17
Text GLabel 4250 3950 2    60   Input ~ 0
PWM16
Text GLabel 7000 1900 0    60   Input ~ 0
PWM00
Text GLabel 7000 2000 0    60   Input ~ 0
PWM01
Text GLabel 7000 2100 0    60   Input ~ 0
PWM02
Text GLabel 7000 2200 0    60   Input ~ 0
PWM03
Text GLabel 7000 2300 0    60   Input ~ 0
PWM04
Text GLabel 7000 2400 0    60   Input ~ 0
PWM05
Text GLabel 7000 2500 0    60   Input ~ 0
PWM06
Text GLabel 7000 2600 0    60   Input ~ 0
PWM07
Text GLabel 7000 3400 0    60   Input ~ 0
PWM15
Text GLabel 7000 3300 0    60   Input ~ 0
PWM14
Text GLabel 7000 3200 0    60   Input ~ 0
PWM13
Text GLabel 7000 3100 0    60   Input ~ 0
PWM12
Text GLabel 7000 3000 0    60   Input ~ 0
PWM11
Text GLabel 7000 2900 0    60   Input ~ 0
PWM10
Text GLabel 7000 2800 0    60   Input ~ 0
PWM09
Text GLabel 7000 2700 0    60   Input ~ 0
PWM08
Text GLabel 7000 3600 0    60   Input ~ 0
PWM17
Text GLabel 7000 3500 0    60   Input ~ 0
PWM16
$Comp
L +3.3V #PWR?
U 1 1 532C9B7F
P 3050 1500
F 0 "#PWR?" H 3050 1460 30  0001 C CNN
F 1 "+3.3V" H 3050 1610 30  0000 C CNN
F 2 "" H 3050 1500 60  0000 C CNN
F 3 "" H 3050 1500 60  0000 C CNN
	1    3050 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	3050 1500 3050 1650
Wire Wire Line
	3250 1650 3250 1550
Wire Wire Line
	3250 1550 3050 1550
Connection ~ 3050 1550
$Comp
L +3.3V #PWR?
U 1 1 532C9BA9
P 1750 1950
F 0 "#PWR?" H 1750 1910 30  0001 C CNN
F 1 "+3.3V" H 1750 2060 30  0000 C CNN
F 2 "" H 1750 1950 60  0000 C CNN
F 3 "" H 1750 1950 60  0000 C CNN
	1    1750 1950
	0    -1   -1   0   
$EndComp
$Comp
L R R?
U 1 1 532C9BB8
P 2000 1950
F 0 "R?" V 2080 1950 40  0000 C CNN
F 1 "1k" V 2007 1951 40  0000 C CNN
F 2 "~" V 1930 1950 30  0000 C CNN
F 3 "~" H 2000 1950 30  0000 C CNN
	1    2000 1950
	0    1    1    0   
$EndComp
$Comp
L CONN_6 P?
U 1 1 532C9BE5
P 7350 4300
F 0 "P?" V 7300 4300 60  0000 C CNN
F 1 "CONN_6" V 7400 4300 60  0000 C CNN
F 2 "" H 7350 4300 60  0000 C CNN
F 3 "" H 7350 4300 60  0000 C CNN
	1    7350 4300
	1    0    0    -1  
$EndComp
Text GLabel 4250 3350 2    60   Input ~ 0
MOSI
Text GLabel 4250 3450 2    60   Input ~ 0
MISO
Text GLabel 4250 3550 2    60   Input ~ 0
SCK
Text GLabel 2250 1950 1    60   Input ~ 0
RESET
$Comp
L CONN_3 K?
U 1 1 532EDC04
P 1650 2550
F 0 "K?" V 1600 2550 50  0000 C CNN
F 1 "RESONATOR" V 1700 2550 40  0000 C CNN
F 2 "" H 1650 2550 60  0000 C CNN
F 3 "" H 1650 2550 60  0000 C CNN
	1    1650 2550
	-1   0    0    1   
$EndComp
Wire Wire Line
	2250 2350 2100 2350
Wire Wire Line
	2100 2350 2100 2450
Wire Wire Line
	2100 2450 2000 2450
Wire Wire Line
	2250 2750 2100 2750
Wire Wire Line
	2100 2750 2100 2650
Wire Wire Line
	2100 2650 2000 2650
$Comp
L GND #PWR?
U 1 1 532EDC69
P 2200 2550
F 0 "#PWR?" H 2200 2550 30  0001 C CNN
F 1 "GND" H 2200 2480 30  0001 C CNN
F 2 "" H 2200 2550 60  0000 C CNN
F 3 "" H 2200 2550 60  0000 C CNN
	1    2200 2550
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2000 2550 2200 2550
$Comp
L C C?
U 1 1 532EDCAD
P 2050 3150
F 0 "C?" H 2050 3250 40  0000 L CNN
F 1 "C" H 2056 3065 40  0000 L CNN
F 2 "~" H 2088 3000 30  0000 C CNN
F 3 "~" H 2050 3150 60  0000 C CNN
	1    2050 3150
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR?
U 1 1 532EDCBC
P 1850 3150
F 0 "#PWR?" H 1850 3150 30  0001 C CNN
F 1 "GND" H 1850 3080 30  0001 C CNN
F 2 "" H 1850 3150 60  0000 C CNN
F 3 "" H 1850 3150 60  0000 C CNN
	1    1850 3150
	0    1    1    0   
$EndComp
$Comp
L GND #PWR?
U 1 1 532EDCD5
P 3250 5800
F 0 "#PWR?" H 3250 5800 30  0001 C CNN
F 1 "GND" H 3250 5730 30  0001 C CNN
F 2 "" H 3250 5800 60  0000 C CNN
F 3 "" H 3250 5800 60  0000 C CNN
	1    3250 5800
	1    0    0    -1  
$EndComp
Wire Wire Line
	3250 5800 3250 5650
Wire Wire Line
	3050 5650 3050 5700
Wire Wire Line
	3050 5700 3250 5700
Connection ~ 3250 5700
$EndSCHEMATC
