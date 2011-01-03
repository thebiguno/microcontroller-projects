EESchema Schematic File Version 2  date Mon 03 Jan 2011 08:35:08 AM PST
LIBS:power,device,transistors,conn,linear,regul,74xx,cmos4000,adc-dac,memory,xilinx,special,microcontrollers,dsp,microchip,analog_switches,motorola,texas,intel,audio,interface,digital-audio,philips,display,cypress,siliconi,opto,atmel,contrib,valves
EELAYER 43  0
EELAYER END
$Descr User 6000 5000
Sheet 1 1
Title "Slave Flash"
Date "3 jan 2011"
Rev ""
Comp "Wyatt Olson"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Connection ~ 4500 2500
Wire Wire Line
	3600 2500 5150 2500
Wire Wire Line
	3600 2500 3600 2800
Wire Wire Line
	3600 2800 3400 2800
Connection ~ 4250 2000
Wire Wire Line
	4250 2200 4250 1750
Wire Wire Line
	3750 2000 3650 2000
Wire Wire Line
	3650 1750 3750 1750
Wire Wire Line
	4350 900  4150 900 
Wire Wire Line
	3950 3300 4150 3300
Wire Wire Line
	4150 3300 4150 2950
Wire Wire Line
	4150 2950 4050 2950
Connection ~ 600  1200
Wire Wire Line
	600  1350 600  1100
Wire Wire Line
	1500 1200 1150 1200
Wire Wire Line
	600  1100 1500 1100
Wire Wire Line
	600  1200 750  1200
Wire Wire Line
	1500 1300 1300 1300
Wire Wire Line
	2400 3700 2400 3500
Wire Wire Line
	3750 3300 3550 3300
Wire Wire Line
	3550 3300 3550 2950
Wire Wire Line
	3550 2950 3650 2950
Wire Wire Line
	3750 900  3400 900 
Wire Wire Line
	4250 1750 4150 1750
Wire Wire Line
	4250 2000 4150 2000
Wire Wire Line
	3650 2000 3650 1500
Connection ~ 3650 1750
Wire Wire Line
	3850 2500 3850 2650
Wire Wire Line
	4600 3300 4800 3300
Wire Wire Line
	4800 3300 4800 2950
Wire Wire Line
	4800 2950 4700 2950
Wire Wire Line
	4400 3300 4200 3300
Wire Wire Line
	4200 3300 4200 2950
Wire Wire Line
	4200 2950 4300 2950
Wire Wire Line
	4500 2500 4500 2650
Wire Wire Line
	5250 3300 5450 3300
Wire Wire Line
	5450 3300 5450 2950
Wire Wire Line
	5450 2950 5350 2950
Wire Wire Line
	5050 3300 4850 3300
Wire Wire Line
	4850 3300 4850 2950
Wire Wire Line
	4850 2950 4950 2950
Wire Wire Line
	5150 2500 5150 2650
Connection ~ 3850 2500
$Comp
L CONN_2 P3
U 1 1 4D21FA46
P 5150 3650
F 0 "P3" V 5100 3650 40  0000 C CNN
F 1 "Flash" V 5200 3650 40  0000 C CNN
	1    5150 3650
	0    1    1    0   
$EndComp
$Comp
L PNP Q3
U 1 1 4D21FA45
P 5150 2850
F 0 "Q3" H 5300 2850 60  0000 C CNN
F 1 "Triac" H 5054 3000 60  0000 C CNN
	1    5150 2850
	0    1    1    0   
$EndComp
$Comp
L CONN_2 P2
U 1 1 4D21FA37
P 4500 3650
F 0 "P2" V 4450 3650 40  0000 C CNN
F 1 "Flash" V 4550 3650 40  0000 C CNN
	1    4500 3650
	0    1    1    0   
$EndComp
$Comp
L PNP Q2
U 1 1 4D21FA36
P 4500 2850
F 0 "Q2" H 4650 2850 60  0000 C CNN
F 1 "Triac" H 4404 3000 60  0000 C CNN
	1    4500 2850
	0    1    1    0   
$EndComp
$Comp
L +5V #PWR5
U 1 1 4D0C51B5
P 3650 1500
F 0 "#PWR5" H 3650 1590 20  0001 C CNN
F 1 "+5V" H 3650 1590 30  0000 C CNN
	1    3650 1500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR6
U 1 1 4D0C51AD
P 4250 2200
F 0 "#PWR6" H 4250 2200 30  0001 C CNN
F 1 "GND" H 4250 2130 30  0001 C CNN
	1    4250 2200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR7
U 1 1 4D0C5173
P 4350 900
F 0 "#PWR7" H 4350 900 30  0001 C CNN
F 1 "GND" H 4350 830 30  0001 C CNN
	1    4350 900 
	0    -1   -1   0   
$EndComp
$Comp
L CONN_2 P1
U 1 1 4D0C5120
P 3850 3650
F 0 "P1" V 3800 3650 40  0000 C CNN
F 1 "Flash" V 3900 3650 40  0000 C CNN
	1    3850 3650
	0    1    1    0   
$EndComp
$Comp
L PNP Q1
U 1 1 4D0C50E6
P 3850 2850
F 0 "Q1" H 4000 2850 60  0000 C CNN
F 1 "Triac" H 3754 3000 60  0000 C CNN
	1    3850 2850
	0    1    1    0   
$EndComp
$Comp
L GND #PWR4
U 1 1 4D0C50BA
P 2400 3700
F 0 "#PWR4" H 2400 3700 30  0001 C CNN
F 1 "GND" H 2400 3630 30  0001 C CNN
	1    2400 3700
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR1
U 1 1 4D0C50AA
P 600 1350
F 0 "#PWR1" H 600 1350 30  0001 C CNN
F 1 "GND" H 600 1280 30  0001 C CNN
	1    600  1350
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR3
U 1 1 4D0C5087
P 2400 600
F 0 "#PWR3" H 2400 690 20  0001 C CNN
F 1 "+5V" H 2400 690 30  0000 C CNN
	1    2400 600 
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR2
U 1 1 4D0C507B
P 1300 1300
F 0 "#PWR2" H 1300 1390 20  0001 C CNN
F 1 "+5V" H 1300 1390 30  0000 C CNN
	1    1300 1300
	0    -1   -1   0   
$EndComp
$Comp
L DIODE D1
U 1 1 4D0C501F
P 3950 900
F 0 "D1" H 3950 1000 40  0000 C CNN
F 1 "Photo Diode" H 3950 800 40  0000 C CNN
	1    3950 900 
	-1   0    0    1   
$EndComp
$Comp
L C C2
U 1 1 4D0C4F1B
P 3950 1750
F 0 "C2" H 4000 1850 50  0000 L CNN
F 1 "10uF" H 4000 1650 50  0000 L CNN
	1    3950 1750
	0    1    1    0   
$EndComp
$Comp
L C C3
U 1 1 4D0C4F10
P 3950 2000
F 0 "C3" H 4000 2100 50  0000 L CNN
F 1 "0.1uF" H 4000 1900 50  0000 L CNN
	1    3950 2000
	0    1    1    0   
$EndComp
$Comp
L C C1
U 1 1 4D0C4EFF
P 950 1200
F 0 "C1" H 1000 1300 50  0000 L CNN
F 1 "C" H 1000 1100 50  0000 L CNN
	1    950  1200
	0    1    1    0   
$EndComp
$Comp
L ATMEGA8-P IC1
U 1 1 4D0C4E61
P 2400 2000
F 0 "IC1" H 1700 3250 50  0000 L BNN
F 1 "ATMEGA 168" H 2650 600 50  0000 L BNN
F 2 "DIL28" H 2900 525 50  0001 C CNN
	1    2400 2000
	1    0    0    -1  
$EndComp
$EndSCHEMATC
