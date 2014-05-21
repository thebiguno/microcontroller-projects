EESchema Schematic File Version 1
LIBS:power,device,conn,linear,regul,74xx,cmos4000,adc-dac,memory,xilinx,special,microcontrollers,dsp,microchip,analog_switches,motorola,texas,intel,audio,interface,digital-audio,philips,display,cypress,siliconi,contrib,valves
EELAYER 43  0
EELAYER END
$Descr User 8000 4000
Sheet 1 1
Title "Drum Master Cymbal Mute (Capacitance Sensor)"
Date "1 jul 2008"
Rev "1.0"
Comp "Wyatt Olson"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	3350 1800 3350 1450
Wire Wire Line
	3350 1450 3450 1450
Wire Wire Line
	3150 1400 3150 1500
Connection ~ 3350 1800
Wire Wire Line
	7250 950  7250 1050
Wire Wire Line
	6950 1250 6850 1250
Wire Wire Line
	4550 850  4550 1000
Wire Wire Line
	4550 850  5550 850 
Wire Wire Line
	6250 1750 6250 1650
Wire Wire Line
	5100 1350 5150 1350
Wire Wire Line
	4450 1150 4550 1150
Connection ~ 5100 1150
Wire Wire Line
	5100 1150 5100 1000
Wire Wire Line
	5100 1000 5050 1000
Wire Wire Line
	4050 950  4050 1100
Wire Wire Line
	4700 1800 2700 1800
Wire Wire Line
	3400 2000 3550 2000
Connection ~ 2800 2000
Wire Wire Line
	2900 2000 2700 2000
Wire Wire Line
	1300 1300 1300 1800
Wire Wire Line
	1300 1800 1050 1800
Wire Wire Line
	1800 1300 2800 1300
Wire Wire Line
	2800 1300 2800 2000
Wire Wire Line
	2700 2200 2800 2200
Wire Wire Line
	2800 2200 2800 2550
Wire Wire Line
	2800 2550 1200 2550
Wire Wire Line
	1200 2550 1200 1800
Connection ~ 1200 1800
Wire Wire Line
	1050 1150 1050 1400
Wire Wire Line
	5050 1150 5150 1150
Wire Wire Line
	4550 1000 4450 1000
Wire Wire Line
	4700 1400 4700 1350
Wire Wire Line
	6250 1650 5550 1650
Wire Wire Line
	6150 1250 6350 1250
Connection ~ 6250 1250
Wire Wire Line
	7250 1450 7250 1600
Connection ~ 4050 1600
Wire Wire Line
	3150 1000 3150 900 
Wire Wire Line
	3700 1600 4400 1600
$Comp
L POT RV?
U 1 1 486C185E
P 3700 1450
F 0 "RV?" H 3700 1350 50  0000 C C
F 1 "200k" H 3700 1450 50  0000 C C
	1    3700 1450
	1    0    0    1   
$EndComp
Text Notes 2300 800  0    60   ~
from oscillating
Text Notes 2300 700  0    60   ~
power supply
Text Notes 2300 600  0    60   ~
Cap. prevents
$Comp
L C C3
U 1 1 486A5A09
P 3150 1200
F 0 "C3" H 3200 1300 50  0000 L C
F 1 "1uF" H 3200 1100 50  0000 L C
	1    3150 1200
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR3
U 1 1 486A59FF
P 3150 900
F 0 "#PWR3" H 3150 990 20  0001 C C
F 1 "+5V" H 3150 990 30  0000 C C
	1    3150 900 
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR8
U 1 1 486A59FA
P 3150 1500
F 0 "#PWR8" H 3150 1500 30  0001 C C
F 1 "GND" H 3150 1430 30  0001 C C
	1    3150 1500
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P2
U 1 1 48651457
P 7250 800
F 0 "P2" H 7330 800 40  0000 C C
F 1 "CONN_1" H 7200 840 30  0001 C C
	1    7250 800 
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR7
U 1 1 48651442
P 7250 1600
F 0 "#PWR7" H 7250 1600 30  0001 C C
F 1 "GND" H 7250 1530 30  0001 C C
	1    7250 1600
	1    0    0    -1  
$EndComp
$Comp
L NPN Q2
U 1 1 48651435
P 7150 1250
F 0 "Q2" H 7300 1250 50  0000 C C
F 1 "NPN" H 7052 1400 50  0000 C C
	1    7150 1250
	1    0    0    -1  
$EndComp
$Comp
L R R6
U 1 1 4865142C
P 6600 1250
F 0 "R6" V 6680 1250 50  0000 C C
F 1 "1k" V 6600 1250 50  0000 C C
	1    6600 1250
	0    1    1    0   
$EndComp
$Comp
L GND #PWR6
U 1 1 48651367
P 6250 1750
F 0 "#PWR6" H 6250 1750 30  0001 C C
F 1 "GND" H 6250 1680 30  0001 C C
	1    6250 1750
	1    0    0    -1  
$EndComp
$Comp
L C C2
U 1 1 4865135F
P 6250 1450
F 0 "C2" H 6300 1550 50  0000 L C
F 1 "1uF" H 6300 1350 50  0000 L C
	1    6250 1450
	1    0    0    -1  
$EndComp
$Comp
L DIODESCH D1
U 1 1 4865134A
P 4900 1350
F 0 "D1" H 4900 1450 40  0000 C C
F 1 "DIODESCH" H 4900 1250 40  0000 C C
	1    4900 1350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR5
U 1 1 486512AD
P 4450 1150
F 0 "#PWR5" H 4450 1150 30  0001 C C
F 1 "GND" H 4450 1080 30  0001 C C
	1    4450 1150
	0    1    1    0   
$EndComp
$Comp
L +5V #PWR2
U 1 1 486512A6
P 4450 1000
F 0 "#PWR2" H 4450 1090 20  0001 C C
F 1 "+5V" H 4450 1090 30  0000 C C
	1    4450 1000
	0    -1   -1   0   
$EndComp
$Comp
L R R2
U 1 1 48651293
P 4800 1000
F 0 "R2" V 4880 1000 50  0000 C C
F 1 "100k" V 4800 1000 50  0000 C C
	1    4800 1000
	0    1    1    0   
$EndComp
$Comp
L R R1
U 1 1 48651285
P 4800 1150
F 0 "R1" V 4880 1150 50  0000 C C
F 1 "100k" V 4800 1150 50  0000 C C
	1    4800 1150
	0    1    1    0   
$EndComp
$Comp
L LM324N U1
U 1 1 48651274
P 5650 1250
F 0 "U1" H 5700 1450 60  0000 C C
F 1 "LM324N" H 5800 1050 50  0000 C C
	1    5650 1250
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P1
U 1 1 4865125A
P 4050 800
F 0 "P1" H 4130 800 40  0000 C C
F 1 "CONN_1" H 4000 840 30  0001 C C
	1    4050 800 
	0    -1   -1   0   
$EndComp
$Comp
L R R3
U 1 1 486511A9
P 4050 1350
F 0 "R3" V 4130 1350 50  0000 C C
F 1 "150" V 4050 1350 50  0000 C C
	1    4050 1350
	1    0    0    -1  
$EndComp
$Comp
L NPN Q1
U 1 1 4865117F
P 4600 1600
F 0 "Q1" H 4750 1600 50  0000 C C
F 1 "NPN" H 4502 1750 50  0000 C C
	1    4600 1600
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR1
U 1 1 4865111A
P 3550 2000
F 0 "#PWR1" H 3550 2090 20  0001 C C
F 1 "+5V" H 3550 2090 30  0000 C C
	1    3550 2000
	0    1    1    0   
$EndComp
$Comp
L GND #PWR4
U 1 1 48651106
P 1050 1150
F 0 "#PWR4" H 1050 1150 30  0001 C C
F 1 "GND" H 1050 1080 30  0001 C C
	1    1050 1150
	-1   0    0    1   
$EndComp
$Comp
L R R5
U 1 1 486510BF
P 3150 2000
F 0 "R5" V 3230 2000 50  0000 C C
F 1 "1k" V 3150 2000 50  0000 C C
	1    3150 2000
	0    1    1    0   
$EndComp
$Comp
L R R4
U 1 1 486510B3
P 1550 1300
F 0 "R4" V 1630 1300 50  0000 C C
F 1 "1k" V 1550 1300 50  0000 C C
	1    1550 1300
	0    1    1    0   
$EndComp
$Comp
L C C1
U 1 1 486510A2
P 1050 1600
F 0 "C1" H 1100 1700 50  0000 L C
F 1 "0.1uF" H 1100 1500 50  0000 L C
	1    1050 1600
	1    0    0    -1  
$EndComp
$Comp
L LM555N U2
U 1 1 48651081
P 2000 2000
F 0 "U2" H 2000 2100 70  0000 C C
F 1 "LM555N" H 2000 1900 70  0000 C C
	1    2000 2000
	1    0    0    -1  
$EndComp
$EndSCHEMATC
