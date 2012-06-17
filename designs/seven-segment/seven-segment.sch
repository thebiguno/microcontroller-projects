EESchema Schematic File Version 2  date Sun 17 Jun 2012 01:43:22 PM MDT
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
EELAYER 43  0
EELAYER END
$Descr A4 11700 8267
encoding utf-8
Sheet 1 1
Title ""
Date "17 jun 2012"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Label 5050 4200 0    60   ~ 0
D10
Text Label 5050 4300 0    60   ~ 0
D11
Text Label 5050 4400 0    60   ~ 0
D12
Text Label 5050 4500 0    60   ~ 0
D13
Text Label 3150 2600 2    60   ~ 0
VCC
Text Label 3150 2500 2    60   ~ 0
AREF
Text Label 5050 2700 0    60   ~ 0
A5
Text Label 5050 2600 0    60   ~ 0
A4
Text Label 5050 2500 0    60   ~ 0
DGT4
Text Label 5050 2400 0    60   ~ 0
DGT3
Text Label 5050 2300 0    60   ~ 0
DGT2
Text Label 5050 2200 0    60   ~ 0
DGT1
Connection ~ 2050 3500
Wire Wire Line
	2050 3900 2050 2900
Wire Wire Line
	2450 2900 3150 2900
Wire Wire Line
	4050 1900 2650 1900
Wire Wire Line
	2650 1900 2650 2200
Connection ~ 4550 6700
Wire Wire Line
	4850 6700 3750 6700
Connection ~ 4150 6700
Wire Wire Line
	4150 6600 4150 6850
Wire Wire Line
	4550 6300 4850 6300
Wire Wire Line
	2450 3500 2850 3500
Wire Wire Line
	2850 3500 2850 3100
Wire Wire Line
	2850 3100 3150 3100
$Comp
L GND #PWR?
U 1 1 4FDE3137
P 2050 3900
F 0 "#PWR?" H 2050 3900 30  0001 C CNN
F 1 "GND" H 2050 3830 30  0001 C CNN
	1    2050 3900
	1    0    0    -1  
$EndComp
$Comp
L C C?
U 1 1 4FDE30FB
P 2250 3500
F 0 "C?" H 2300 3600 50  0000 L CNN
F 1 "C" H 2300 3400 50  0000 L CNN
	1    2250 3500
	0    1    1    0   
$EndComp
$Comp
L C C?
U 1 1 4FDE30F7
P 2250 2900
F 0 "C?" H 2300 3000 50  0000 L CNN
F 1 "C" H 2300 2800 50  0000 L CNN
	1    2250 2900
	0    1    1    0   
$EndComp
$Comp
L CRYSTAL X?
U 1 1 4FDE30E6
P 2450 3200
F 0 "X?" H 2450 3350 60  0000 C CNN
F 1 "CRYSTAL" H 2450 3050 60  0000 C CNN
	1    2450 3200
	0    1    1    0   
$EndComp
Text Label 5900 3250 2    60   ~ 0
DGT4
Text Label 5900 3150 2    60   ~ 0
DGT3
Text Label 5900 3050 2    60   ~ 0
DGT2
Text Label 5900 2950 2    60   ~ 0
DGT1
Text Label 7500 3750 0    60   ~ 0
DP
Text Label 7500 3550 0    60   ~ 0
G
Text Label 7500 3450 0    60   ~ 0
F
Text Label 7500 3350 0    60   ~ 0
E
Text Label 7500 3250 0    60   ~ 0
D
Text Label 7500 3150 0    60   ~ 0
C
Text Label 7500 3050 0    60   ~ 0
B
Text Label 7500 2950 0    60   ~ 0
A
Text Label 3150 2400 2    60   ~ 0
GND
Text Label 6800 1300 3    60   ~ 0
GND
Text Label 6600 1300 3    60   ~ 0
VCC
Text Label 6500 1300 3    60   ~ 0
TXD
Text Label 6400 1300 3    60   ~ 0
RXD
Text Label 6300 1300 3    60   ~ 0
RTS#
$Comp
L CONN_6 P?
U 1 1 4FDE2FFC
P 6550 950
F 0 "P?" V 6500 950 60  0000 C CNN
F 1 "CONN_6" V 6600 950 60  0000 C CNN
	1    6550 950 
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR?
U 1 1 4FDE2F8B
P 4150 6850
F 0 "#PWR?" H 4150 6850 30  0001 C CNN
F 1 "GND" H 4150 6780 30  0001 C CNN
	1    4150 6850
	1    0    0    -1  
$EndComp
$Comp
L C C?
U 1 1 4FDE2F5F
P 4850 6500
F 0 "C?" H 4900 6600 50  0000 L CNN
F 1 ".1uF" H 4900 6400 50  0000 L CNN
	1    4850 6500
	1    0    0    -1  
$EndComp
$Comp
L C C?
U 1 1 4FDE2F33
P 4550 6500
F 0 "C?" H 4600 6600 50  0000 L CNN
F 1 "47uF" H 4600 6400 50  0000 L CNN
	1    4550 6500
	1    0    0    -1  
$EndComp
$Comp
L C C?
U 1 1 4FDE2F2D
P 3750 6500
F 0 "C?" H 3800 6600 50  0000 L CNN
F 1 "47uF" H 3800 6400 50  0000 L CNN
	1    3750 6500
	1    0    0    -1  
$EndComp
Text Label 4050 4800 0    60   ~ 0
GND
Text Label 4050 1900 0    60   ~ 0
VCC
$Comp
L R R?
U 1 1 4FDE2EC3
P 2900 2200
F 0 "R?" V 2900 2300 50  0000 C CNN
F 1 "10K" V 2900 2150 50  0000 C CNN
	1    2900 2200
	0    1    1    0   
$EndComp
Text Label 5050 3200 0    60   ~ 0
RXD
Text Label 5050 3100 0    60   ~ 0
TXD
$Comp
L R R?
U 1 1 4FDE2CFA
P 7250 3750
F 0 "R?" V 7250 3850 50  0000 C CNN
F 1 "R" V 7250 3650 50  0000 C CNN
	1    7250 3750
	0    1    1    0   
$EndComp
$Comp
L R R?
U 1 1 4FDE2CF6
P 7250 3550
F 0 "R?" V 7250 3650 50  0000 C CNN
F 1 "R" V 7250 3450 50  0000 C CNN
	1    7250 3550
	0    1    1    0   
$EndComp
$Comp
L R R?
U 1 1 4FDE2CAC
P 7250 3450
F 0 "R?" V 7250 3550 50  0000 C CNN
F 1 "R" V 7250 3350 50  0000 C CNN
	1    7250 3450
	0    1    1    0   
$EndComp
$Comp
L R R?
U 1 1 4FDE2C9F
P 7250 3350
F 0 "R?" V 7250 3250 50  0000 C CNN
F 1 "R" V 7250 3450 50  0000 C CNN
	1    7250 3350
	0    -1   -1   0   
$EndComp
$Comp
L R R?
U 1 1 4FDE2C95
P 7250 3250
F 0 "R?" V 7350 3150 50  0000 C CNN
F 1 "R" V 7250 3350 50  0000 C CNN
	1    7250 3250
	0    -1   -1   0   
$EndComp
$Comp
L R R?
U 1 1 4FDE2C8F
P 7250 3150
F 0 "R?" V 7350 3250 50  0000 C CNN
F 1 "R" V 7250 3050 50  0000 C CNN
	1    7250 3150
	0    1    1    0   
$EndComp
$Comp
L R R?
U 1 1 4FDE2C85
P 7250 3050
F 0 "R?" V 7250 3150 50  0000 C CNN
F 1 "R" V 7250 2950 50  0000 C CNN
	1    7250 3050
	0    1    1    0   
$EndComp
$Comp
L R R?
U 1 1 4FDE2C3A
P 7250 2950
F 0 "R?" V 7250 3050 50  0000 C CNN
F 1 "R" V 7250 2850 50  0000 C CNN
	1    7250 2950
	0    1    1    0   
$EndComp
$Comp
L LCQ-5642TW11 U?
U 1 1 4FDE2C19
P 6550 3550
F 0 "U?" V 6350 3400 60  0000 C CNN
F 1 "LCQ-5642TW11" V 6550 3850 60  0000 C CNN
	1    6550 3550
	1    0    0    -1  
$EndComp
$Comp
L 7805 U?
U 1 1 4FDE0F7D
P 4150 6350
F 0 "U?" H 4300 6154 60  0000 C CNN
F 1 "7805" H 4150 6550 60  0000 C CNN
	1    4150 6350
	1    0    0    -1  
$EndComp
$Comp
L ATMEGA8-P IC?
U 1 1 4FDE0F4D
P 4050 3300
F 0 "IC?" H 3350 4550 50  0000 L BNN
F 1 "ATMEGA8-P" H 4300 1900 50  0000 L BNN
F 2 "DIL28" H 4550 1825 50  0001 C CNN
	1    4050 3300
	1    0    0    -1  
$EndComp
$EndSCHEMATC
