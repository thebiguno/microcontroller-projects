EESchema Schematic File Version 2  date Mon 18 Jun 2012 02:15:11 AM MDT
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
LIBS:seven-segment-cache
EELAYER 25  0
EELAYER END
$Descr A4 11700 8267
encoding utf-8
Sheet 1 1
Title ""
Date "18 jun 2012"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Connection ~ 7150 4600
Wire Wire Line
	7500 4600 6050 4600
Wire Wire Line
	9600 2500 9600 2750
Connection ~ 9600 2750
Wire Wire Line
	4050 4850 4050 4800
Wire Wire Line
	3050 1900 4050 1900
Wire Wire Line
	3150 3100 2850 3100
Wire Wire Line
	2850 3100 2850 3500
Wire Wire Line
	2850 3500 2450 3500
Wire Wire Line
	6450 4500 6450 4750
Connection ~ 6450 4600
Connection ~ 6850 4600
Wire Wire Line
	2650 1900 2650 2200
Wire Wire Line
	2450 2900 3150 2900
Wire Wire Line
	2050 3900 2050 2900
Connection ~ 2050 3500
Wire Wire Line
	7500 4200 6850 4200
Connection ~ 7150 4200
$Comp
L DIODE D?
U 1 1 4FDEE361
P 7500 4400
F 0 "D?" H 7500 4500 40  0000 C CNN
F 1 "DIODE" H 7500 4300 40  0000 C CNN
	1    7500 4400
	0    -1   -1   0   
$EndComp
Text Label 9250 4500 3    60   ~ 0
D10
Text Label 9150 4500 3    60   ~ 0
D9
Text Label 9050 4500 3    60   ~ 0
D4
$Comp
L CONN_3 K2
U 1 1 4FDEDD1B
P 9150 4150
F 0 "K2" V 9100 4150 50  0000 C CNN
F 1 "CONN_3" V 9200 4150 40  0000 C CNN
	1    9150 4150
	0    -1   -1   0   
$EndComp
Text Label 8250 4500 3    60   ~ 0
+12
Text Label 8450 4500 3    60   ~ 0
GND
$Comp
L CONN_2 P5
U 1 1 4FDEDCD7
P 8350 4150
F 0 "P5" V 8300 4150 40  0000 C CNN
F 1 "CONN_2" V 8400 4150 40  0000 C CNN
	1    8350 4150
	0    -1   -1   0   
$EndComp
Text Label 10500 4550 3    60   ~ 0
D11'
Text Label 10400 4550 3    60   ~ 0
VCC
Text Label 10300 4550 3    60   ~ 0
SCK
Text Label 10200 4550 3    60   ~ 0
RCK
Text Label 10100 4550 3    60   ~ 0
GND
$Comp
L CONN_5 P4
U 1 1 4FDED9B5
P 10300 4150
F 0 "P4" V 10250 4150 50  0000 C CNN
F 1 "CONN_5" V 10350 4150 50  0000 C CNN
	1    10300 4150
	0    -1   -1   0   
$EndComp
Text Label 9900 4500 3    60   ~ 0
A0
Text Label 9800 4500 3    60   ~ 0
A1
Text Label 9700 4500 3    60   ~ 0
A2
Text Label 9600 4500 3    60   ~ 0
A3
Text Label 9500 4500 3    60   ~ 0
A4
Text Label 9400 4500 3    60   ~ 0
A5
Text Label 8850 4500 3    60   ~ 0
VCC
Text Label 8750 4500 3    60   ~ 0
AREF
Text Label 8650 4500 3    60   ~ 0
GND
$Comp
L CONN_6 P3
U 1 1 4FDED930
P 9650 4150
F 0 "P3" V 9600 4150 60  0000 C CNN
F 1 "CONN_6" V 9700 4150 60  0000 C CNN
	1    9650 4150
	0    -1   -1   0   
$EndComp
$Comp
L CONN_3 K1
U 1 1 4FDED920
P 8750 4150
F 0 "K1" V 8700 4150 50  0000 C CNN
F 1 "CONN_3" V 8800 4150 40  0000 C CNN
	1    8750 4150
	0    -1   -1   0   
$EndComp
Text Label 9000 2750 2    60   ~ 0
BTN1
Text Label 9000 2500 2    60   ~ 0
BTN0
Text Label 9600 2750 0    60   ~ 0
GND
$Comp
L SW_PUSH SW2
U 1 1 4FDED3D3
P 9300 2750
F 0 "SW2" H 9450 2860 50  0000 C CNN
F 1 "SW_PUSH" H 9300 2670 50  0000 C CNN
	1    9300 2750
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH SW1
U 1 1 4FDED3D1
P 9300 2500
F 0 "SW1" H 9450 2610 50  0000 C CNN
F 1 "SW_PUSH" H 9300 2420 50  0000 C CNN
	1    9300 2500
	1    0    0    -1  
$EndComp
Text Label 5050 2200 0    60   ~ 0
A0
Text Label 2650 1900 2    60   ~ 0
RTS#
Text Label 7150 4200 0    60   ~ 0
VCC
Text Label 8100 4500 3    60   ~ 0
GND
Text Label 7900 4500 3    60   ~ 0
+12
Text Label 6050 4200 2    60   ~ 0
+12
$Comp
L CONN_2 P2
U 1 1 4FDE87BD
P 8000 4150
F 0 "P2" V 7950 4150 40  0000 C CNN
F 1 "CONN_2" V 8050 4150 40  0000 C CNN
	1    8000 4150
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR2
U 1 1 4FDE8148
P 4050 4850
F 0 "#PWR2" H 4050 4850 30  0001 C CNN
F 1 "GND" H 4050 4780 30  0001 C CNN
	1    4050 4850
	1    0    0    -1  
$EndComp
$Comp
L C C3
U 1 1 4FDE72D5
P 2850 1900
F 0 "C3" H 2900 2000 50  0000 L CNN
F 1 ".1uF" H 2900 1800 50  0000 L CNN
	1    2850 1900
	0    1    1    0   
$EndComp
Text Label 5050 3300 0    60   ~ 0
BTN0
Text Label 5050 3400 0    60   ~ 0
BTN1
Text Label 5050 3500 0    60   ~ 0
D4
Text Label 5050 3600 0    60   ~ 0
DGT3
Text Label 5050 3700 0    60   ~ 0
DGT2
Text Label 5050 3800 0    60   ~ 0
DGT4
Text Label 5050 4000 0    60   ~ 0
DGT1
Text Label 5050 4100 0    60   ~ 0
D9
Text Label 7300 2600 0    60   ~ 0
D11'
Text Label 7800 2400 0    60   ~ 0
A
Text Label 7800 2300 0    60   ~ 0
F
Text Label 7800 2200 0    60   ~ 0
G
Text Label 7800 2100 0    60   ~ 0
C
Text Label 7800 2000 0    60   ~ 0
DP
Text Label 7800 1900 0    60   ~ 0
D
Text Label 7800 1800 0    60   ~ 0
E
Text Label 7800 1700 0    60   ~ 0
B
Text Label 5900 1700 2    60   ~ 0
MOSI
Text Label 5900 2300 2    60   ~ 0
GND
Text Label 5900 2200 2    60   ~ 0
RCK
Text Label 5900 2000 2    60   ~ 0
VCC
Text Label 5900 1900 2    60   ~ 0
SCK
$Comp
L 74HC595 U3
U 1 1 4FDE71B4
P 6600 2150
F 0 "U3" H 6750 2750 70  0000 C CNN
F 1 "74HC595" H 6600 1550 70  0000 C CNN
	1    6600 2150
	1    0    0    -1  
$EndComp
Text Label 5050 4200 0    60   ~ 0
D10
Text Label 5050 4300 0    60   ~ 0
MOSI
Text Label 5050 4400 0    60   ~ 0
RCK
Text Label 5050 4500 0    60   ~ 0
SCK
Text Label 3150 2600 2    60   ~ 0
VCC
Text Label 3150 2500 2    60   ~ 0
AREF
Text Label 5050 2700 0    60   ~ 0
A5
Text Label 5050 2600 0    60   ~ 0
A4
Text Label 5050 2500 0    60   ~ 0
A3
Text Label 5050 2400 0    60   ~ 0
A2
Text Label 5050 2300 0    60   ~ 0
A1
$Comp
L GND #PWR1
U 1 1 4FDE3137
P 2050 3900
F 0 "#PWR1" H 2050 3900 30  0001 C CNN
F 1 "GND" H 2050 3830 30  0001 C CNN
	1    2050 3900
	1    0    0    -1  
$EndComp
$Comp
L C C2
U 1 1 4FDE30FB
P 2250 3500
F 0 "C2" H 2300 3600 50  0000 L CNN
F 1 "C" H 2300 3400 50  0000 L CNN
	1    2250 3500
	0    1    1    0   
$EndComp
$Comp
L C C1
U 1 1 4FDE30F7
P 2250 2900
F 0 "C1" H 2300 3000 50  0000 L CNN
F 1 "C" H 2300 2800 50  0000 L CNN
	1    2250 2900
	0    1    1    0   
$EndComp
$Comp
L CRYSTAL X1
U 1 1 4FDE30E6
P 2450 3200
F 0 "X1" H 2450 3350 60  0000 C CNN
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
Text Label 7000 3750 0    60   ~ 0
DP
Text Label 7000 3550 0    60   ~ 0
G
Text Label 7000 3450 0    60   ~ 0
F
Text Label 7000 3350 0    60   ~ 0
E
Text Label 7000 3250 0    60   ~ 0
D
Text Label 7000 3150 0    60   ~ 0
C
Text Label 7000 3050 0    60   ~ 0
B
Text Label 7000 2950 0    60   ~ 0
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
L CONN_6 P1
U 1 1 4FDE2FFC
P 6550 950
F 0 "P1" V 6500 950 60  0000 C CNN
F 1 "CONN_6" V 6600 950 60  0000 C CNN
	1    6550 950 
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR3
U 1 1 4FDE2F8B
P 6450 4750
F 0 "#PWR3" H 6450 4750 30  0001 C CNN
F 1 "GND" H 6450 4680 30  0001 C CNN
	1    6450 4750
	1    0    0    -1  
$EndComp
$Comp
L C C6
U 1 1 4FDE2F5F
P 7150 4400
F 0 "C6" H 7200 4500 50  0000 L CNN
F 1 ".1uF" H 7200 4300 50  0000 L CNN
	1    7150 4400
	1    0    0    -1  
$EndComp
$Comp
L C C5
U 1 1 4FDE2F33
P 6850 4400
F 0 "C5" H 6900 4500 50  0000 L CNN
F 1 "47uF" H 6900 4300 50  0000 L CNN
	1    6850 4400
	1    0    0    -1  
$EndComp
$Comp
L C C4
U 1 1 4FDE2F2D
P 6050 4400
F 0 "C4" H 6100 4500 50  0000 L CNN
F 1 "47uF" H 6100 4300 50  0000 L CNN
	1    6050 4400
	1    0    0    -1  
$EndComp
Text Label 4050 4800 0    60   ~ 0
GND
Text Label 4050 1900 0    60   ~ 0
VCC
$Comp
L R R1
U 1 1 4FDE2EC3
P 2900 2200
F 0 "R1" V 2900 2300 50  0000 C CNN
F 1 "10K" V 2900 2150 50  0000 C CNN
	1    2900 2200
	0    1    1    0   
$EndComp
Text Label 5050 3200 0    60   ~ 0
RXD
Text Label 5050 3100 0    60   ~ 0
TXD
$Comp
L R R9
U 1 1 4FDE2CFA
P 7550 2400
F 0 "R9" V 7550 2500 50  0000 C CNN
F 1 "R" V 7550 2300 50  0000 C CNN
	1    7550 2400
	0    1    1    0   
$EndComp
$Comp
L R R8
U 1 1 4FDE2CF6
P 7550 2300
F 0 "R8" V 7550 2400 50  0000 C CNN
F 1 "R" V 7550 2200 50  0000 C CNN
	1    7550 2300
	0    1    1    0   
$EndComp
$Comp
L R R7
U 1 1 4FDE2CAC
P 7550 2200
F 0 "R7" V 7550 2300 50  0000 C CNN
F 1 "R" V 7550 2100 50  0000 C CNN
	1    7550 2200
	0    1    1    0   
$EndComp
$Comp
L R R6
U 1 1 4FDE2C9F
P 7550 2100
F 0 "R6" V 7550 2200 50  0000 C CNN
F 1 "R" V 7550 2000 50  0000 C CNN
	1    7550 2100
	0    1    1    0   
$EndComp
$Comp
L R R5
U 1 1 4FDE2C95
P 7550 2000
F 0 "R5" V 7550 2100 50  0000 C CNN
F 1 "R" V 7550 1900 50  0000 C CNN
	1    7550 2000
	0    1    1    0   
$EndComp
$Comp
L R R4
U 1 1 4FDE2C8F
P 7550 1900
F 0 "R4" V 7550 2000 50  0000 C CNN
F 1 "R" V 7550 1800 50  0000 C CNN
	1    7550 1900
	0    1    1    0   
$EndComp
$Comp
L R R3
U 1 1 4FDE2C85
P 7550 1800
F 0 "R3" V 7550 1900 50  0000 C CNN
F 1 "R" V 7550 1700 50  0000 C CNN
	1    7550 1800
	0    1    1    0   
$EndComp
$Comp
L R R2
U 1 1 4FDE2C3A
P 7550 1700
F 0 "R2" V 7550 1800 50  0000 C CNN
F 1 "R" V 7550 1600 50  0000 C CNN
	1    7550 1700
	0    1    1    0   
$EndComp
$Comp
L LCQ-5642TW11 U2
U 1 1 4FDE2C19
P 6550 3550
F 0 "U2" V 6350 3400 60  0000 C CNN
F 1 "LCQ-5642TW11" V 6550 3850 60  0000 C CNN
	1    6550 3550
	1    0    0    -1  
$EndComp
$Comp
L 7805 U1
U 1 1 4FDE0F7D
P 6450 4250
F 0 "U1" H 6600 4054 60  0000 C CNN
F 1 "7805" H 6450 4450 60  0000 C CNN
	1    6450 4250
	1    0    0    -1  
$EndComp
$Comp
L ATMEGA8-P IC1
U 1 1 4FDE0F4D
P 4050 3300
F 0 "IC1" H 3350 4550 50  0000 L BNN
F 1 "ATMEGA8-P" H 4300 1900 50  0000 L BNN
F 2 "DIL28" H 4550 1825 50  0001 C CNN
	1    4050 3300
	1    0    0    -1  
$EndComp
$EndSCHEMATC
