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
$Descr User 6000 4000
encoding utf-8
Sheet 6 7
Title "User Interface (Display + Encoder)"
Date "24 jan 2015"
Rev "1"
Comp "Digital Cave"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text HLabel 2900 1450 1    60   Input ~ 0
DS_E
Text HLabel 2700 1450 1    60   Input ~ 0
DS_RS
Text HLabel 3400 1450 1    60   Input ~ 0
DS_D4
Text HLabel 3500 1450 1    60   Input ~ 0
DS_D5
Text HLabel 3600 1450 1    60   Input ~ 0
DS_D6
Text HLabel 3700 1450 1    60   Input ~ 0
DS_D7
$Comp
L HD44780 DS?
U 1 1 54C2FA6F
P 2400 1450
F 0 "DS?" H 3850 900 60  0000 C CNN
F 1 "HD44780" H 2700 900 60  0000 C CNN
F 2 "~" H 2400 1450 60  0000 C CNN
F 3 "~" H 2400 1450 60  0000 C CNN
	1    2400 1450
	1    0    0    -1  
$EndComp
$Comp
L RSMALL R?
U 1 1 54C2FA8A
P 3800 950
F 0 "R?" V 3890 945 40  0000 C CNN
F 1 "1k" V 3715 950 40  0000 C CNN
F 2 "~" V 3730 950 30  0000 C CNN
F 3 "~" H 3800 1000 30  0000 C CNN
	1    3800 950 
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR?
U 1 1 54C2FA9E
P 3800 850
F 0 "#PWR?" H 3800 940 20  0001 C CNN
F 1 "+5V" H 3800 940 30  0000 C CNN
F 2 "" H 3800 850 60  0000 C CNN
F 3 "" H 3800 850 60  0000 C CNN
	1    3800 850 
	1    0    0    -1  
$EndComp
Wire Wire Line
	3800 1050 3800 1450
$Comp
L GND #PWR?
U 1 1 54C2FAB0
P 3900 1450
F 0 "#PWR?" H 3900 1450 30  0001 C CNN
F 1 "GND" H 3900 1380 30  0001 C CNN
F 2 "" H 3900 1450 60  0000 C CNN
F 3 "" H 3900 1450 60  0000 C CNN
	1    3900 1450
	-1   0    0    1   
$EndComp
NoConn ~ 3000 1450
NoConn ~ 3100 1450
NoConn ~ 3200 1450
NoConn ~ 3300 1450
NoConn ~ 2800 1450
$Comp
L GND #PWR?
U 1 1 54C2FABD
P 2400 1450
F 0 "#PWR?" H 2400 1450 30  0001 C CNN
F 1 "GND" H 2400 1380 30  0001 C CNN
F 2 "" H 2400 1450 60  0000 C CNN
F 3 "" H 2400 1450 60  0000 C CNN
	1    2400 1450
	-1   0    0    1   
$EndComp
$Comp
L +5V #PWR?
U 1 1 54C2FAC3
P 2500 1450
F 0 "#PWR?" H 2500 1540 20  0001 C CNN
F 1 "+5V" H 2500 1540 30  0000 C CNN
F 2 "" H 2500 1450 60  0000 C CNN
F 3 "" H 2500 1450 60  0000 C CNN
	1    2500 1450
	1    0    0    -1  
$EndComp
Text HLabel 2600 1450 1    60   Input ~ 0
DS_VE
Text HLabel 1200 1400 1    60   Input ~ 0
ENC_1
Text HLabel 1300 1400 1    60   Input ~ 0
ENC_2
Text HLabel 1400 1400 1    60   Input ~ 0
ENC_3
$EndSCHEMATC
