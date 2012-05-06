EESchema Schematic File Version 2  date Sun 06 May 2012 02:22:23 PM MDT
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
Date "6 may 2012"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Label 750  3550 0    60   ~ 0
GND
Text Label 750  3450 0    60   ~ 0
VCC
Text Label 750  2000 0    60   ~ 0
GND
Text Label 750  1900 0    60   ~ 0
VCC
Wire Wire Line
	750  3550 1150 3550
Wire Wire Line
	750  2000 1150 2000
Wire Wire Line
	3300 3250 3750 3250
Wire Wire Line
	3300 3450 3750 3450
Wire Wire Line
	3300 3650 3750 3650
Wire Wire Line
	3300 3850 3750 3850
Wire Wire Line
	750  3150 1150 3150
Wire Wire Line
	750  2850 1150 2850
Wire Wire Line
	1150 1600 750  1600
Wire Wire Line
	750  1100 1150 1100
Wire Wire Line
	1150 2650 1150 2350
Wire Wire Line
	1150 2350 2550 2350
Wire Wire Line
	2550 2350 2550 2000
Wire Wire Line
	3100 1600 3350 1600
Wire Wire Line
	3100 1400 3350 1400
Wire Wire Line
	3100 1200 3350 1200
Wire Wire Line
	3350 1800 3100 1800
Wire Wire Line
	3100 1100 3350 1100
Wire Wire Line
	3350 1300 3100 1300
Wire Wire Line
	3350 1500 3100 1500
Wire Wire Line
	3350 1700 3100 1700
Wire Wire Line
	3350 2700 3100 2700
Wire Wire Line
	3350 2500 3100 2500
Wire Wire Line
	3350 2300 3100 2300
Wire Wire Line
	3100 2100 3350 2100
Wire Wire Line
	3350 2800 3100 2800
Wire Wire Line
	3100 2200 3350 2200
Wire Wire Line
	3100 2400 3350 2400
Wire Wire Line
	3100 2600 3350 2600
Wire Wire Line
	1150 1300 750  1300
Wire Wire Line
	750  1400 1150 1400
Wire Wire Line
	750  1700 1150 1700
Wire Wire Line
	750  2950 1150 2950
Wire Wire Line
	1150 3250 750  3250
Wire Wire Line
	3300 3750 3750 3750
Wire Wire Line
	3300 3550 3750 3550
Wire Wire Line
	3750 3350 3300 3350
Wire Wire Line
	750  1900 1150 1900
Wire Wire Line
	1150 3450 750  3450
$Comp
L STPIC6D595 U2
U 1 1 4FA6DD2D
P 1850 3100
F 0 "U2" H 2000 3700 70  0000 C CNN
F 1 "STPIC6D595" H 1850 2500 70  0000 C CNN
	1    1850 3100
	1    0    0    -1  
$EndComp
$Comp
L STPIC6D595 U1
U 1 1 4FA6DD24
P 1850 1550
F 0 "U1" H 2000 2150 70  0000 C CNN
F 1 "STPIC6D595" H 1850 950 70  0000 C CNN
	1    1850 1550
	1    0    0    -1  
$EndComp
Text Label 4550 3650 0    60   ~ 0
ROW8
Text Label 4550 3550 0    60   ~ 0
ROW7
Text Label 4550 3450 0    60   ~ 0
ROW6
Text Label 4550 3350 0    60   ~ 0
ROW5
Text Label 3300 3650 0    60   ~ 0
ROW4
Text Label 3300 3550 0    60   ~ 0
ROW3
Text Label 3300 3450 0    60   ~ 0
ROW2
Text Label 3300 3350 0    60   ~ 0
ROW1
Text Label 4550 3750 0    60   ~ 0
RCLK
Text Label 3300 3750 0    60   ~ 0
SRCLK
Text Label 4550 3250 0    60   ~ 0
GND
Text Label 3300 3250 0    60   ~ 0
VCC
$Comp
L CONN_7X2 P1
U 1 1 4FA6D47A
P 4150 3550
F 0 "P1" H 4150 3950 60  0000 C CNN
F 1 "CONN_7X2" V 4150 3550 60  0000 C CNN
	1    4150 3550
	1    0    0    -1  
$EndComp
Text Label 4550 3850 0    60   ~ 0
DATA_OUT
Text Label 3300 3850 0    60   ~ 0
DATA_IN
Text Label 2550 3550 0    60   ~ 0
DATA_OUT
Text Label 5050 2300 0    60   ~ 0
ROW1
Text Label 5050 2200 0    60   ~ 0
ROW2
Text Label 5050 2100 0    60   ~ 0
ROW3
Text Label 5050 2000 0    60   ~ 0
ROW4
Text Label 5050 1900 0    60   ~ 0
ROW5
Text Label 5050 1800 0    60   ~ 0
ROW6
Text Label 5050 1700 0    60   ~ 0
ROW7
Text Label 5050 1600 0    60   ~ 0
ROW8
Text Label 750  3250 0    60   ~ 0
GND
Text Label 750  3150 0    60   ~ 0
RCLK
Text Label 750  2950 0    60   ~ 0
VCC
Text Label 750  2850 0    60   ~ 0
SRCLK
Text Label 750  1600 0    60   ~ 0
RCLK
Text Label 750  1700 0    60   ~ 0
GND
Text Label 750  1400 0    60   ~ 0
VCC
Text Label 750  1300 0    60   ~ 0
SRCLK
Text Label 750  1100 0    60   ~ 0
DATA_IN
Text Label 2550 3350 0    60   ~ 0
GRN4
Text Label 2550 3250 0    60   ~ 0
RED4
Text Label 2550 3150 0    60   ~ 0
RED8
Text Label 2550 3050 0    60   ~ 0
GRN8
Text Label 2550 2950 0    60   ~ 0
RED7
Text Label 2550 2850 0    60   ~ 0
GRN7
Text Label 2550 2750 0    60   ~ 0
GRN3
Text Label 2550 2650 0    60   ~ 0
RED3
Text Label 2550 1800 0    60   ~ 0
GRN2
Text Label 2550 1700 0    60   ~ 0
RED2
Text Label 2550 1600 0    60   ~ 0
RED6
Text Label 2550 1500 0    60   ~ 0
GRN6
Text Label 2550 1400 0    60   ~ 0
RED5
Text Label 2550 1300 0    60   ~ 0
GRN5
Text Label 2550 1200 0    60   ~ 0
GRN1
Text Label 2550 1100 0    60   ~ 0
RED1
Text Label 3100 2800 0    60   ~ 0
GRN1
Text Label 3100 2700 0    60   ~ 0
GRN2
Text Label 3100 2600 0    60   ~ 0
GRN3
Text Label 3100 2500 0    60   ~ 0
GRN4
Text Label 3100 2400 0    60   ~ 0
GRN5
Text Label 3100 2300 0    60   ~ 0
GRN6
Text Label 3100 2200 0    60   ~ 0
GRN7
Text Label 3100 2100 0    60   ~ 0
GRN8
Text Label 3100 1100 0    60   ~ 0
RED8
Text Label 3100 1200 0    60   ~ 0
RED7
Text Label 3100 1300 0    60   ~ 0
RED6
Text Label 3100 1400 0    60   ~ 0
RED5
Text Label 3100 1500 0    60   ~ 0
RED4
Text Label 3100 1600 0    60   ~ 0
RED3
Text Label 3100 1700 0    60   ~ 0
RED2
Text Label 3100 1800 0    60   ~ 0
RED1
$Comp
L R R16
U 1 1 4FA15EE0
P 3600 2800
F 0 "R16" V 3600 2900 50  0000 C CNN
F 1 "180" V 3600 2750 50  0000 C CNN
	1    3600 2800
	0    1    1    0   
$EndComp
$Comp
L R R15
U 1 1 4FA15ED9
P 3600 2700
F 0 "R15" V 3600 2800 50  0000 C CNN
F 1 "180" V 3600 2650 50  0000 C CNN
	1    3600 2700
	0    1    1    0   
$EndComp
$Comp
L R R14
U 1 1 4FA15ED7
P 3600 2600
F 0 "R14" V 3600 2700 50  0000 C CNN
F 1 "180" V 3600 2550 50  0000 C CNN
	1    3600 2600
	0    1    1    0   
$EndComp
$Comp
L R R13
U 1 1 4FA15ED5
P 3600 2500
F 0 "R13" V 3600 2600 50  0000 C CNN
F 1 "180" V 3600 2450 50  0000 C CNN
	1    3600 2500
	0    1    1    0   
$EndComp
$Comp
L R R12
U 1 1 4FA15ED3
P 3600 2400
F 0 "R12" V 3600 2500 50  0000 C CNN
F 1 "180" V 3600 2350 50  0000 C CNN
	1    3600 2400
	0    1    1    0   
$EndComp
$Comp
L R R11
U 1 1 4FA15ED1
P 3600 2300
F 0 "R11" V 3600 2400 50  0000 C CNN
F 1 "180" V 3600 2250 50  0000 C CNN
	1    3600 2300
	0    1    1    0   
$EndComp
$Comp
L R R10
U 1 1 4FA15ECE
P 3600 2200
F 0 "R10" V 3600 2300 50  0000 C CNN
F 1 "180" V 3600 2150 50  0000 C CNN
	1    3600 2200
	0    1    1    0   
$EndComp
$Comp
L R R9
U 1 1 4FA15EB6
P 3600 2100
F 0 "R9" V 3600 2200 50  0000 C CNN
F 1 "180" V 3600 2050 50  0000 C CNN
	1    3600 2100
	0    1    1    0   
$EndComp
$Comp
L R R8
U 1 1 4FA15EB3
P 3600 1800
F 0 "R8" V 3600 1900 50  0000 C CNN
F 1 "150" V 3600 1750 50  0000 C CNN
	1    3600 1800
	0    1    1    0   
$EndComp
$Comp
L R R7
U 1 1 4FA15EB0
P 3600 1700
F 0 "R7" V 3600 1800 50  0000 C CNN
F 1 "150" V 3600 1650 50  0000 C CNN
	1    3600 1700
	0    1    1    0   
$EndComp
$Comp
L R R6
U 1 1 4FA15EAE
P 3600 1600
F 0 "R6" V 3600 1700 50  0000 C CNN
F 1 "150" V 3600 1550 50  0000 C CNN
	1    3600 1600
	0    1    1    0   
$EndComp
$Comp
L R R5
U 1 1 4FA15EAA
P 3600 1500
F 0 "R5" V 3600 1600 50  0000 C CNN
F 1 "150" V 3600 1450 50  0000 C CNN
	1    3600 1500
	0    1    1    0   
$EndComp
$Comp
L R R4
U 1 1 4FA15EA7
P 3600 1400
F 0 "R4" V 3600 1500 50  0000 C CNN
F 1 "150" V 3600 1350 50  0000 C CNN
	1    3600 1400
	0    1    1    0   
$EndComp
$Comp
L R R3
U 1 1 4FA15EA5
P 3600 1300
F 0 "R3" V 3600 1400 50  0000 C CNN
F 1 "150" V 3600 1250 50  0000 C CNN
	1    3600 1300
	0    1    1    0   
$EndComp
$Comp
L R R2
U 1 1 4FA15EA2
P 3600 1200
F 0 "R2" V 3600 1300 50  0000 C CNN
F 1 "150" V 3600 1150 50  0000 C CNN
	1    3600 1200
	0    1    1    0   
$EndComp
$Comp
L R R1
U 1 1 4FA15D08
P 3600 1100
F 0 "R1" V 3600 1200 50  0000 C CNN
F 1 "150" V 3600 1050 50  0000 C CNN
	1    3600 1100
	0    1    1    0   
$EndComp
$Comp
L LE-MM103 L1
U 1 1 4FA0A62C
P 4500 1950
F 0 "L1" H 3600 1800 60  0000 C CNN
F 1 "LE-MM103" H 5200 1800 60  0000 C CNN
	1    4500 1950
	0    -1   -1   0   
$EndComp
$EndSCHEMATC
