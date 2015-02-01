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
Sheet 2 3
Title ""
Date "28 jan 2015"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L ATMEGA32U4-A U?
U 1 1 54C81AA2
P 3600 3850
F 0 "U?" H 2650 5550 40  0000 C CNN
F 1 "ATMEGA32U4-A" H 4300 2350 40  0000 C CNN
F 2 "TQFP44" H 3600 3850 35  0000 C CIN
F 3 "" H 4700 4950 60  0000 C CNN
	1    3600 3850
	1    0    0    -1  
$EndComp
Text HLabel 5550 3300 2    60   Output ~ 0
S0
Text HLabel 5550 3400 2    60   Output ~ 0
S1
Text HLabel 5550 3500 2    60   Output ~ 0
S2
Text HLabel 5550 3600 2    60   Output ~ 0
A0
Text HLabel 5550 3700 2    60   Output ~ 0
A1
Text HLabel 5550 3800 2    60   Output ~ 0
A2
Text HLabel 5550 3900 2    60   Output ~ 0
A3
Text HLabel 4800 3800 2    60   Output ~ 0
TXD
Text HLabel 4800 3700 2    60   Input ~ 0
RXD
Wire Wire Line
	4700 3700 4800 3700
Wire Wire Line
	4700 3800 4800 3800
$EndSCHEMATC
