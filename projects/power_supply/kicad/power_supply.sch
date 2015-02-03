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
Sheet 1 5
Title "Adjustable Modular Power Supply"
Date "3 feb 2015"
Rev "1"
Comp "Digital Cave"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Sheet
S 1600 1200 1300 400 
U 54C2A519
F0 "Transformer / Rectifier" 50
F1 "transformer_rectifier.sch" 50
F2 "AC1" I L 1600 1300 60 
F3 "AC2" I L 1600 1500 60 
F4 "V+" O R 2900 1300 60 
F5 "V-" O R 2900 1400 60 
$EndSheet
Wire Wire Line
	3200 1300 2900 1300
Wire Wire Line
	2900 1400 3200 1400
$Sheet
S 900  2150 850  300 
U 54C32205
F0 "Fan Controller" 50
F1 "fan.sch" 50
$EndSheet
Wire Wire Line
	2900 1950 3200 1950
Wire Wire Line
	3200 2050 2900 2050
Wire Wire Line
	3200 2350 2900 2350
Wire Wire Line
	2900 2250 3200 2250
$Comp
L PINS_2 K2
U 1 1 54C9744A
P 4900 1350
F 0 "K2" H 4900 1200 60  0000 C CNN
F 1 "REGULATED_OUTPUT" H 4900 1500 60  0000 C CNN
F 2 "~" H 4900 1400 60  0000 C CNN
F 3 "~" H 4900 1400 60  0000 C CNN
	1    4900 1350
	-1   0    0    -1  
$EndComp
Wire Wire Line
	4600 1300 4250 1300
$Comp
L GND #PWR01
U 1 1 54C97614
P 4600 1400
F 0 "#PWR01" H 4600 1400 30  0001 C CNN
F 1 "GND" H 4600 1330 30  0001 C CNN
F 2 "" H 4600 1400 60  0000 C CNN
F 3 "" H 4600 1400 60  0000 C CNN
	1    4600 1400
	0    1    1    0   
$EndComp
$Comp
L PINS_3 K1
U 1 1 54C979E0
P 1000 1400
F 0 "K1" H 1000 1200 60  0000 C CNN
F 1 "AC_INPUT" H 1000 1600 60  0000 C CNN
F 2 "~" H 1000 1500 60  0000 C CNN
F 3 "~" H 1000 1500 60  0000 C CNN
	1    1000 1400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 54C97A9B
P 1300 1400
F 0 "#PWR02" H 1300 1400 30  0001 C CNN
F 1 "GND" H 1300 1330 30  0001 C CNN
F 2 "" H 1300 1400 60  0000 C CNN
F 3 "" H 1300 1400 60  0000 C CNN
	1    1300 1400
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1600 1300 1300 1300
Wire Wire Line
	1300 1500 1600 1500
$Sheet
S 2200 1800 700  650 
U 54C2B08D
F0 "Controller" 50
F1 "controller.sch" 50
F2 "I_ADJUST" O R 2900 2050 60 
F3 "V_ADJUST" O R 2900 1950 60 
F4 "V_IN" I R 2900 2250 60 
F5 "I_IN" I R 2900 2350 60 
$EndSheet
$Sheet
S 3200 1200 1050 1250
U 54C2B0D8
F0 "Regulator" 50
F1 "regulator.sch" 50
F2 "V+" I L 3200 1300 60 
F3 "V-" I L 3200 1400 60 
F4 "REGULATED_V" O R 4250 1300 60 
F5 "V_ADJUST" I L 3200 1950 60 
F6 "I_ADJUST" I L 3200 2050 60 
F7 "I_OUT" O L 3200 2350 60 
F8 "V_OUT" O L 3200 2250 60 
$EndSheet
$EndSCHEMATC
