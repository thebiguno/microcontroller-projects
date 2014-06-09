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
Date "9 jun 2014"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L CONN_13 P2
U 1 1 539609FF
P 3950 4100
F 0 "P2" V 3910 4100 60  0000 C CNN
F 1 "CONN_13" V 4020 4100 60  0000 C CNN
F 2 "" H 3950 4100 60  0000 C CNN
F 3 "" H 3950 4100 60  0000 C CNN
	1    3950 4100
	1    0    0    -1  
$EndComp
NoConn ~ 3600 3700
NoConn ~ 3600 3800
NoConn ~ 3600 3900
NoConn ~ 3600 4000
NoConn ~ 3600 4100
NoConn ~ 3600 4200
NoConn ~ 3600 4300
NoConn ~ 3600 4400
NoConn ~ 3600 4500
$Comp
L CONN_2 P3
U 1 1 53960A6A
P 3050 5050
F 0 "P3" V 3000 5050 40  0000 C CNN
F 1 "CONN_2" V 3100 5050 40  0000 C CNN
F 2 "" H 3050 5050 60  0000 C CNN
F 3 "" H 3050 5050 60  0000 C CNN
	1    3050 5050
	-1   0    0    1   
$EndComp
$Comp
L CONN_1 P4
U 1 1 53960A79
P 3750 5150
F 0 "P4" H 3830 5150 40  0000 L CNN
F 1 "CONN_1" H 3750 5205 30  0001 C CNN
F 2 "" H 3750 5150 60  0000 C CNN
F 3 "" H 3750 5150 60  0000 C CNN
	1    3750 5150
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 4600 3550 4600
Wire Wire Line
	3550 3500 3550 4950
Wire Wire Line
	3300 4700 3600 4700
Wire Wire Line
	3600 5150 3400 5150
Wire Wire Line
	3550 4950 3400 4950
Connection ~ 3550 4600
$Comp
L CONN_10 P1
U 1 1 53960BDC
P 2900 3950
F 0 "P1" V 2850 3950 60  0000 C CNN
F 1 "CONN_10" V 2950 3950 60  0000 C CNN
F 2 "" H 2900 3950 60  0000 C CNN
F 3 "" H 2900 3950 60  0000 C CNN
	1    2900 3950
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3550 3500 3250 3500
Wire Wire Line
	3250 4400 3300 4400
Wire Wire Line
	3300 4400 3300 4700
Wire Wire Line
	3600 3500 3600 3550
Wire Wire Line
	3600 3550 3350 3550
Wire Wire Line
	3350 3550 3350 3600
Wire Wire Line
	3350 3600 3250 3600
Wire Wire Line
	3600 3600 3450 3600
Wire Wire Line
	3450 3600 3450 3700
Wire Wire Line
	3450 3700 3250 3700
Text Label 3300 3700 0    60   ~ 0
RX
Text Label 3300 3600 0    60   ~ 0
TX
Text Label 3300 3500 0    60   ~ 0
VCC
Text Label 3350 4700 0    60   ~ 0
GND
$EndSCHEMATC
