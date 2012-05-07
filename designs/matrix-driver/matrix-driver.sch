EESchema Schematic File Version 2  date Mon 07 May 2012 03:22:21 PM MDT
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
LIBS:matrix-driver-cache
EELAYER 25  0
EELAYER END
$Descr A4 11700 8267
encoding utf-8
Sheet 1 1
Title ""
Date "7 may 2012"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	8150 2450 8600 2450
Wire Wire Line
	8150 2250 8600 2250
Wire Wire Line
	8150 2050 8600 2050
Connection ~ 10550 1800
Wire Wire Line
	11050 600  11050 1800
Wire Wire Line
	11050 1800 7550 1800
Connection ~ 9550 1800
Wire Wire Line
	10050 1800 10050 800 
Connection ~ 8550 1800
Wire Wire Line
	9050 1800 9050 1000
Wire Wire Line
	8050 1800 8050 1200
Wire Wire Line
	6900 600  10650 600 
Wire Wire Line
	6900 800  9650 800 
Wire Wire Line
	6900 1000 8650 1000
Wire Wire Line
	6900 1200 7650 1200
Wire Wire Line
	600  3150 1000 3150
Wire Wire Line
	600  1600 1000 1600
Wire Wire Line
	6900 2050 7350 2050
Wire Wire Line
	6900 2250 7350 2250
Wire Wire Line
	6900 2450 7350 2450
Wire Wire Line
	600  2750 1000 2750
Wire Wire Line
	600  2450 1000 2450
Wire Wire Line
	1000 1200 600  1200
Wire Wire Line
	600  700  1000 700 
Wire Wire Line
	1000 2250 1000 1950
Wire Wire Line
	1000 1950 2400 1950
Wire Wire Line
	2400 1950 2400 1600
Wire Wire Line
	2950 1200 3200 1200
Wire Wire Line
	2950 1000 3200 1000
Wire Wire Line
	2950 800  3200 800 
Wire Wire Line
	3200 1400 2950 1400
Wire Wire Line
	2950 700  3200 700 
Wire Wire Line
	3200 900  2950 900 
Wire Wire Line
	3200 1100 2950 1100
Wire Wire Line
	3200 1300 2950 1300
Wire Wire Line
	3200 2300 2950 2300
Wire Wire Line
	3200 2100 2950 2100
Wire Wire Line
	3200 1900 2950 1900
Wire Wire Line
	2950 1700 3200 1700
Wire Wire Line
	3200 2400 2950 2400
Wire Wire Line
	2950 1800 3200 1800
Wire Wire Line
	2950 2000 3200 2000
Wire Wire Line
	2950 2200 3200 2200
Wire Wire Line
	1000 900  600  900 
Wire Wire Line
	600  1000 1000 1000
Wire Wire Line
	600  1300 1000 1300
Wire Wire Line
	600  2550 1000 2550
Wire Wire Line
	1000 2850 600  2850
Wire Wire Line
	6900 2550 7350 2550
Wire Wire Line
	6900 2350 7350 2350
Wire Wire Line
	7350 2150 6900 2150
Wire Wire Line
	600  1500 1000 1500
Wire Wire Line
	1000 3050 600  3050
Wire Wire Line
	6900 1300 7150 1300
Wire Wire Line
	8150 1100 6900 1100
Wire Wire Line
	6900 900  9150 900 
Wire Wire Line
	6900 700  10150 700 
Wire Wire Line
	7550 1800 7550 1300
Wire Wire Line
	8550 1800 8550 1100
Connection ~ 8050 1800
Wire Wire Line
	9550 1800 9550 900 
Connection ~ 9050 1800
Wire Wire Line
	10550 1800 10550 700 
Connection ~ 10050 1800
Wire Wire Line
	2950 2750 3400 2750
Wire Wire Line
	2950 2850 3400 2850
Wire Wire Line
	8150 2150 8600 2150
Wire Wire Line
	8150 2350 8600 2350
Wire Wire Line
	8150 2550 8600 2550
$Comp
L CONN_6 P4
U 1 1 4FA7DACE
P 8950 2300
F 0 "P4" V 8900 2300 60  0000 C CNN
F 1 "CONN_6" V 9000 2300 60  0000 C CNN
	1    8950 2300
	1    0    0    -1  
$EndComp
$Comp
L CONN_6 P3
U 1 1 4FA7DAC0
P 7700 2300
F 0 "P3" V 7650 2300 60  0000 C CNN
F 1 "CONN_6" V 7750 2300 60  0000 C CNN
	1    7700 2300
	1    0    0    -1  
$EndComp
$Comp
L STPIC6D595 U2
U 1 1 4FA7D62E
P 1700 2700
F 0 "U2" H 1850 3300 70  0000 C CNN
F 1 "STPIC6D595" H 1700 2100 70  0000 C CNN
	1    1700 2700
	1    0    0    -1  
$EndComp
$Comp
L STPIC6D595 U1
U 1 1 4FA7D62A
P 1700 1150
F 0 "U1" H 1850 1750 70  0000 C CNN
F 1 "STPIC6D595" H 1700 550 70  0000 C CNN
	1    1700 1150
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P2
U 1 1 4FA7D599
P 3550 2850
F 0 "P2" H 3630 2850 40  0000 L CNN
F 1 "CONN_1" H 3550 2905 30  0001 C CNN
	1    3550 2850
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P1
U 1 1 4FA7D58C
P 3550 2750
F 0 "P1" H 3630 2750 40  0000 L CNN
F 1 "CONN_1" H 3550 2805 30  0001 C CNN
	1    3550 2750
	1    0    0    -1  
$EndComp
Text Label 4900 1900 0    60   ~ 0
R1
Text Label 4900 1800 0    60   ~ 0
R2
Text Label 4900 1700 0    60   ~ 0
R3
Text Label 4900 1600 0    60   ~ 0
R4
Text Label 4900 1500 0    60   ~ 0
R5
Text Label 4900 1400 0    60   ~ 0
R6
Text Label 4900 1300 0    60   ~ 0
R7
Text Label 4900 1200 0    60   ~ 0
R8
Text Label 6900 1300 0    60   ~ 0
R1
Text Label 6900 1200 0    60   ~ 0
R2
Text Label 6900 1100 0    60   ~ 0
R3
Text Label 6900 1000 0    60   ~ 0
R4
Text Label 6900 900  0    60   ~ 0
R5
Text Label 6900 800  0    60   ~ 0
R6
Text Label 6900 700  0    60   ~ 0
R7
Text Label 6900 600  0    60   ~ 0
R8
Text Label 11050 1800 0    60   ~ 0
VCC
$Comp
L PNP Q1
U 1 1 4FA743FC
P 7350 1400
F 0 "Q1" H 7350 1250 60  0000 R CNN
F 1 "PNP" H 7350 1550 60  0000 R CNN
	1    7350 1400
	0    -1   -1   0   
$EndComp
$Comp
L PNP Q2
U 1 1 4FA743F2
P 7850 1300
F 0 "Q2" H 7850 1150 60  0000 R CNN
F 1 "PNP" H 7850 1450 60  0000 R CNN
	1    7850 1300
	0    -1   -1   0   
$EndComp
$Comp
L PNP Q4
U 1 1 4FA743BC
P 8850 1100
F 0 "Q4" H 8850 950 60  0000 R CNN
F 1 "PNP" H 8850 1250 60  0000 R CNN
	1    8850 1100
	0    -1   -1   0   
$EndComp
$Comp
L PNP Q3
U 1 1 4FA743B2
P 8350 1200
F 0 "Q3" H 8350 1050 60  0000 R CNN
F 1 "PNP" H 8350 1350 60  0000 R CNN
	1    8350 1200
	0    -1   -1   0   
$EndComp
$Comp
L PNP Q5
U 1 1 4FA743A5
P 9350 1000
F 0 "Q5" H 9350 850 60  0000 R CNN
F 1 "PNP" H 9350 1150 60  0000 R CNN
	1    9350 1000
	0    -1   -1   0   
$EndComp
$Comp
L PNP Q6
U 1 1 4FA74394
P 9850 900
F 0 "Q6" H 9850 750 60  0000 R CNN
F 1 "PNP" H 9850 1050 60  0000 R CNN
	1    9850 900 
	0    -1   -1   0   
$EndComp
$Comp
L PNP Q7
U 1 1 4FA7437E
P 10350 800
F 0 "Q7" H 10350 650 60  0000 R CNN
F 1 "PNP" H 10350 950 60  0000 R CNN
	1    10350 800 
	0    -1   -1   0   
$EndComp
Text Label 10850 900  0    60   ~ 0
ROW8
$Comp
L PNP Q8
U 1 1 4FA742C5
P 10850 700
F 0 "Q8" H 10850 550 60  0000 R CNN
F 1 "PNP" H 10850 850 60  0000 R CNN
	1    10850 700 
	0    -1   -1   0   
$EndComp
Text Label 600  3150 0    60   ~ 0
GND
Text Label 600  3050 0    60   ~ 0
VCC
Text Label 600  1600 0    60   ~ 0
GND
Text Label 600  1500 0    60   ~ 0
VCC
Text Label 6900 2350 0    60   ~ 0
ROW8
Text Label 6900 2450 0    60   ~ 0
ROW7
Text Label 8150 2150 0    60   ~ 0
ROW6
Text Label 8150 2350 0    60   ~ 0
ROW5
Text Label 6900 2250 0    60   ~ 0
ROW4
Text Label 6900 2550 0    60   ~ 0
ROW3
Text Label 8150 2050 0    60   ~ 0
ROW2
Text Label 8150 2250 0    60   ~ 0
ROW1
Text Label 8150 2550 0    60   ~ 0
RCLK
Text Label 8150 2450 0    60   ~ 0
SRCLK
Text Label 6900 2150 0    60   ~ 0
GND
Text Label 6900 2050 0    60   ~ 0
VCC
Text Label 2950 2850 0    60   ~ 0
DOUT1
Text Label 2950 2750 0    60   ~ 0
DIN1
Text Label 2400 3150 0    60   ~ 0
DOUT1
Text Label 7350 1600 0    60   ~ 0
ROW1
Text Label 7850 1500 0    60   ~ 0
ROW2
Text Label 8350 1400 0    60   ~ 0
ROW3
Text Label 8850 1300 0    60   ~ 0
ROW4
Text Label 9350 1200 0    60   ~ 0
ROW5
Text Label 9850 1100 0    60   ~ 0
ROW6
Text Label 10350 1000 0    60   ~ 0
ROW7
Text Label 600  2850 0    60   ~ 0
GND
Text Label 600  2750 0    60   ~ 0
RCLK
Text Label 600  2550 0    60   ~ 0
VCC
Text Label 600  2450 0    60   ~ 0
SRCLK
Text Label 600  1200 0    60   ~ 0
RCLK
Text Label 600  1300 0    60   ~ 0
GND
Text Label 600  1000 0    60   ~ 0
VCC
Text Label 600  900  0    60   ~ 0
SRCLK
Text Label 600  700  0    60   ~ 0
DIN1
Text Label 2400 2950 0    60   ~ 0
RED3
Text Label 2400 2850 0    60   ~ 0
GRN3
Text Label 2400 2750 0    60   ~ 0
GRN7
Text Label 2400 2650 0    60   ~ 0
RED7
Text Label 2400 2550 0    60   ~ 0
RED8
Text Label 2400 2450 0    60   ~ 0
GRN8
Text Label 2400 2350 0    60   ~ 0
GRN4
Text Label 2400 2250 0    60   ~ 0
RED4
Text Label 2400 1400 0    60   ~ 0
RED1
Text Label 2400 1300 0    60   ~ 0
GRN1
Text Label 2400 1200 0    60   ~ 0
GRN5
Text Label 2400 1100 0    60   ~ 0
RED5
Text Label 2400 1000 0    60   ~ 0
RED6
Text Label 2400 900  0    60   ~ 0
GRN6
Text Label 2400 800  0    60   ~ 0
GRN2
Text Label 2400 700  0    60   ~ 0
RED2
Text Label 2950 2400 0    60   ~ 0
GRN1
Text Label 2950 2300 0    60   ~ 0
GRN2
Text Label 2950 2200 0    60   ~ 0
GRN3
Text Label 2950 2100 0    60   ~ 0
GRN4
Text Label 2950 2000 0    60   ~ 0
GRN5
Text Label 2950 1900 0    60   ~ 0
GRN6
Text Label 2950 1800 0    60   ~ 0
GRN7
Text Label 2950 1700 0    60   ~ 0
GRN8
Text Label 2950 700  0    60   ~ 0
RED8
Text Label 2950 800  0    60   ~ 0
RED7
Text Label 2950 900  0    60   ~ 0
RED6
Text Label 2950 1000 0    60   ~ 0
RED5
Text Label 2950 1100 0    60   ~ 0
RED4
Text Label 2950 1200 0    60   ~ 0
RED3
Text Label 2950 1300 0    60   ~ 0
RED2
Text Label 2950 1400 0    60   ~ 0
RED1
$Comp
L R R16
U 1 1 4FA15EE0
P 3450 2400
F 0 "R16" V 3450 2500 50  0000 C CNN
F 1 "180" V 3450 2350 50  0000 C CNN
	1    3450 2400
	0    1    1    0   
$EndComp
$Comp
L R R15
U 1 1 4FA15ED9
P 3450 2300
F 0 "R15" V 3450 2400 50  0000 C CNN
F 1 "180" V 3450 2250 50  0000 C CNN
	1    3450 2300
	0    1    1    0   
$EndComp
$Comp
L R R14
U 1 1 4FA15ED7
P 3450 2200
F 0 "R14" V 3450 2300 50  0000 C CNN
F 1 "180" V 3450 2150 50  0000 C CNN
	1    3450 2200
	0    1    1    0   
$EndComp
$Comp
L R R13
U 1 1 4FA15ED5
P 3450 2100
F 0 "R13" V 3450 2200 50  0000 C CNN
F 1 "180" V 3450 2050 50  0000 C CNN
	1    3450 2100
	0    1    1    0   
$EndComp
$Comp
L R R12
U 1 1 4FA15ED3
P 3450 2000
F 0 "R12" V 3450 2100 50  0000 C CNN
F 1 "180" V 3450 1950 50  0000 C CNN
	1    3450 2000
	0    1    1    0   
$EndComp
$Comp
L R R11
U 1 1 4FA15ED1
P 3450 1900
F 0 "R11" V 3450 2000 50  0000 C CNN
F 1 "180" V 3450 1850 50  0000 C CNN
	1    3450 1900
	0    1    1    0   
$EndComp
$Comp
L R R10
U 1 1 4FA15ECE
P 3450 1800
F 0 "R10" V 3450 1900 50  0000 C CNN
F 1 "180" V 3450 1750 50  0000 C CNN
	1    3450 1800
	0    1    1    0   
$EndComp
$Comp
L R R9
U 1 1 4FA15EB6
P 3450 1700
F 0 "R9" V 3450 1800 50  0000 C CNN
F 1 "180" V 3450 1650 50  0000 C CNN
	1    3450 1700
	0    1    1    0   
$EndComp
$Comp
L R R8
U 1 1 4FA15EB3
P 3450 1400
F 0 "R8" V 3450 1500 50  0000 C CNN
F 1 "150" V 3450 1350 50  0000 C CNN
	1    3450 1400
	0    1    1    0   
$EndComp
$Comp
L R R7
U 1 1 4FA15EB0
P 3450 1300
F 0 "R7" V 3450 1400 50  0000 C CNN
F 1 "150" V 3450 1250 50  0000 C CNN
	1    3450 1300
	0    1    1    0   
$EndComp
$Comp
L R R6
U 1 1 4FA15EAE
P 3450 1200
F 0 "R6" V 3450 1300 50  0000 C CNN
F 1 "150" V 3450 1150 50  0000 C CNN
	1    3450 1200
	0    1    1    0   
$EndComp
$Comp
L R R5
U 1 1 4FA15EAA
P 3450 1100
F 0 "R5" V 3450 1200 50  0000 C CNN
F 1 "150" V 3450 1050 50  0000 C CNN
	1    3450 1100
	0    1    1    0   
$EndComp
$Comp
L R R4
U 1 1 4FA15EA7
P 3450 1000
F 0 "R4" V 3450 1100 50  0000 C CNN
F 1 "150" V 3450 950 50  0000 C CNN
	1    3450 1000
	0    1    1    0   
$EndComp
$Comp
L R R3
U 1 1 4FA15EA5
P 3450 900
F 0 "R3" V 3450 1000 50  0000 C CNN
F 1 "150" V 3450 850 50  0000 C CNN
	1    3450 900 
	0    1    1    0   
$EndComp
$Comp
L R R2
U 1 1 4FA15EA2
P 3450 800
F 0 "R2" V 3450 900 50  0000 C CNN
F 1 "150" V 3450 750 50  0000 C CNN
	1    3450 800 
	0    1    1    0   
$EndComp
$Comp
L R R1
U 1 1 4FA15D08
P 3450 700
F 0 "R1" V 3450 800 50  0000 C CNN
F 1 "150" V 3450 650 50  0000 C CNN
	1    3450 700 
	0    1    1    0   
$EndComp
$Comp
L LE-MM103 L1
U 1 1 4FA0A62C
P 4350 1550
F 0 "L1" H 3450 1400 60  0000 C CNN
F 1 "LE-MM103" H 5050 1400 60  0000 C CNN
	1    4350 1550
	0    -1   -1   0   
$EndComp
$EndSCHEMATC
