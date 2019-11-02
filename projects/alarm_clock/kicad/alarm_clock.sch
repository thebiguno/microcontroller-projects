EESchema Schematic File Version 4
LIBS:alarm_clock-cache
EELAYER 30 0
EELAYER END
$Descr User 7000 7000
encoding utf-8
Sheet 1 1
Title "Alarm Clock"
Date ""
Rev ""
Comp "Digital Cave"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector:Conn_01x05_Female J3
U 1 1 5CBDD7A8
P 1000 5550
F 0 "J3" H 950 5150 50  0000 R CNN
F 1 "MAX2719" H 1100 5250 50  0000 R CNN
F 2 "" H 1000 5550 50  0001 C CNN
F 3 "~" H 1000 5550 50  0001 C CNN
	1    1000 5550
	-1   0    0    -1  
$EndComp
$Comp
L power:+5V #PWR05
U 1 1 5CBDD863
P 1700 5350
F 0 "#PWR05" H 1700 5200 50  0001 C CNN
F 1 "+5V" V 1715 5479 50  0000 L CNN
F 2 "" H 1700 5350 50  0001 C CNN
F 3 "" H 1700 5350 50  0001 C CNN
	1    1700 5350
	0    1    -1   0   
$EndComp
$Comp
L power:GND #PWR06
U 1 1 5CBDD8C2
P 1700 5450
F 0 "#PWR06" H 1700 5200 50  0001 C CNN
F 1 "GND" V 1705 5321 50  0000 R CNN
F 2 "" H 1700 5450 50  0001 C CNN
F 3 "" H 1700 5450 50  0001 C CNN
	1    1700 5450
	0    -1   1    0   
$EndComp
Wire Wire Line
	1700 5450 1200 5450
Wire Wire Line
	1200 5350 1700 5350
$Comp
L power:+5V #PWR010
U 1 1 5CBDD9EF
P 3550 3250
F 0 "#PWR010" H 3550 3100 50  0001 C CNN
F 1 "+5V" V 3565 3379 50  0000 L CNN
F 2 "" H 3550 3250 50  0001 C CNN
F 3 "" H 3550 3250 50  0001 C CNN
	1    3550 3250
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR09
U 1 1 5CBDDB07
P 3550 3150
F 0 "#PWR09" H 3550 2900 50  0001 C CNN
F 1 "GND" V 3555 3021 50  0000 R CNN
F 2 "" H 3550 3150 50  0001 C CNN
F 3 "" H 3550 3150 50  0001 C CNN
	1    3550 3150
	0    -1   -1   0   
$EndComp
$Comp
L Connector:Conn_01x06_Female J2
U 1 1 5CC0BC2D
P 1000 4650
F 0 "J2" H 893 5038 50  0000 C CNN
F 1 "DS3231" H 893 4945 50  0000 C CNN
F 2 "" H 1000 4650 50  0001 C CNN
F 3 "~" H 1000 4650 50  0001 C CNN
	1    1000 4650
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR03
U 1 1 5CC0D59C
P 1300 4350
F 0 "#PWR03" H 1300 4100 50  0001 C CNN
F 1 "GND" V 1305 4221 50  0000 R CNN
F 2 "" H 1300 4350 50  0001 C CNN
F 3 "" H 1300 4350 50  0001 C CNN
	1    1300 4350
	0    -1   1    0   
$EndComp
Wire Wire Line
	1300 4350 1200 4350
$Comp
L power:+5V #PWR04
U 1 1 5CC0EFEA
P 1300 4450
F 0 "#PWR04" H 1300 4300 50  0001 C CNN
F 1 "+5V" V 1315 4579 50  0000 L CNN
F 2 "" H 1300 4450 50  0001 C CNN
F 3 "" H 1300 4450 50  0001 C CNN
	1    1300 4450
	0    1    -1   0   
$EndComp
Wire Wire Line
	1300 4450 1200 4450
NoConn ~ 1200 4850
Text Label 1200 4750 0    50   ~ 0
RTC_SQW
Text Label 1200 4550 0    50   ~ 0
SDA
Text Label 1200 4650 0    50   ~ 0
SCL
Text Label 5050 1800 1    50   ~ 0
LED_NEUTRAL
Text Label 1200 5550 0    50   ~ 0
MATRIX_DIN
Text Label 1200 5650 0    50   ~ 0
MATRIX_CS
Text Label 1200 5750 0    50   ~ 0
MATRIX_CLK
$Comp
L Connector:Conn_01x16_Female J1
U 1 1 5CC2F117
P 750 1700
F 0 "J1" H 600 700 50  0000 C CNN
F 1 "DF_Player" H 600 800 50  0000 C CNN
F 2 "" H 750 1700 50  0001 C CNN
F 3 "~" H 750 1700 50  0001 C CNN
	1    750  1700
	-1   0    0    -1  
$EndComp
$Comp
L power:+5V #PWR01
U 1 1 5CC37E22
P 1050 1000
F 0 "#PWR01" H 1050 850 50  0001 C CNN
F 1 "+5V" V 1065 1129 50  0000 L CNN
F 2 "" H 1050 1000 50  0001 C CNN
F 3 "" H 1050 1000 50  0001 C CNN
	1    1050 1000
	0    1    -1   0   
$EndComp
Wire Wire Line
	1050 1000 950  1000
$Comp
L power:GND #PWR02
U 1 1 5CC39A8E
P 1050 1600
F 0 "#PWR02" H 1050 1350 50  0001 C CNN
F 1 "GND" V 1055 1471 50  0000 R CNN
F 2 "" H 1050 1600 50  0001 C CNN
F 3 "" H 1050 1600 50  0001 C CNN
	1    1050 1600
	0    -1   1    0   
$EndComp
Wire Wire Line
	1050 1600 950  1600
$Comp
L Device:Speaker LS1
U 1 1 5CC3B89F
P 1900 1550
F 0 "LS1" H 2070 1547 50  0000 L CNN
F 1 "Speaker" H 2070 1454 50  0000 L CNN
F 2 "" H 1900 1350 50  0001 C CNN
F 3 "~" H 1890 1500 50  0001 C CNN
	1    1900 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	950  1500 1500 1500
Wire Wire Line
	1500 1500 1500 1550
Wire Wire Line
	1500 1550 1700 1550
Wire Wire Line
	950  1700 1500 1700
Wire Wire Line
	1500 1700 1500 1650
Wire Wire Line
	1500 1650 1700 1650
$Comp
L custom:UBBB32U4 U1
U 1 1 5DB210B0
P 2900 3850
F 0 "U1" H 2900 4801 60  0000 C CNN
F 1 "UBBB32U4" H 2900 4693 60  0000 C CNN
F 2 "" H 2950 4000 60  0000 C CNN
F 3 "" H 2950 4000 60  0000 C CNN
	1    2900 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	3550 3150 3400 3150
Wire Wire Line
	3400 3250 3550 3250
$Comp
L Device:Rotary_Encoder_Switch SW1
U 1 1 5DB6288F
P 5600 3400
F 0 "SW1" H 5600 3770 50  0000 C CNN
F 1 "LAMP_ENCODER" H 5600 3677 50  0000 C CNN
F 2 "" H 5450 3560 50  0001 C CNN
F 3 "~" H 5600 3660 50  0001 C CNN
	1    5600 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 3500 5300 3500
$Comp
L power:GND #PWR013
U 1 1 5DB6289F
P 4700 3400
F 0 "#PWR013" H 4700 3150 50  0001 C CNN
F 1 "GND" V 4705 3271 50  0000 R CNN
F 2 "" H 4700 3400 50  0001 C CNN
F 3 "" H 4700 3400 50  0001 C CNN
	1    4700 3400
	0    1    1    0   
$EndComp
Wire Wire Line
	4700 3400 4950 3400
$Comp
L power:GND #PWR017
U 1 1 5DB628AA
P 6000 3500
F 0 "#PWR017" H 6000 3250 50  0001 C CNN
F 1 "GND" V 6005 3371 50  0000 R CNN
F 2 "" H 6000 3500 50  0001 C CNN
F 3 "" H 6000 3500 50  0001 C CNN
	1    6000 3500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5900 3500 6000 3500
Text Label 4650 3000 2    50   ~ 0
ENC1_A
Text Label 4650 3800 2    50   ~ 0
ENC1_B
Text Label 6050 3300 0    50   ~ 0
ENC1_SW
Text Label 2400 4550 2    50   ~ 0
ENC1_A
Text Label 3400 4550 0    50   ~ 0
ENC1_B
Text Label 3400 4450 0    50   ~ 0
ENC2_A
Text Label 2400 3550 2    50   ~ 0
ENC2_B
Text Label 950  2500 0    50   ~ 0
MUSIC_BUSY
Text Label 3400 3650 0    50   ~ 0
MUSIC_BUSY
Text Label 3400 4350 0    50   ~ 0
ENC1_SW
Text Label 3400 4250 0    50   ~ 0
ENC2_SW
Wire Wire Line
	5300 3300 5150 3300
Wire Wire Line
	5150 3300 5150 3000
Wire Wire Line
	5150 3000 4950 3000
$Comp
L Device:C_Small C1
U 1 1 5DB80D42
P 4950 3200
F 0 "C1" H 4859 3153 50  0000 R CNN
F 1 "10nF" H 4859 3246 50  0000 R CNN
F 2 "" H 4950 3200 50  0001 C CNN
F 3 "~" H 4950 3200 50  0001 C CNN
	1    4950 3200
	1    0    0    1   
$EndComp
$Comp
L Device:C_Small C2
U 1 1 5DB87E64
P 4950 3600
F 0 "C2" H 4859 3553 50  0000 R CNN
F 1 "10nF" H 4859 3646 50  0000 R CNN
F 2 "" H 4950 3600 50  0001 C CNN
F 3 "~" H 4950 3600 50  0001 C CNN
	1    4950 3600
	1    0    0    1   
$EndComp
Wire Wire Line
	5150 3800 4950 3800
Wire Wire Line
	5150 3500 5150 3800
Wire Wire Line
	4950 3700 4950 3800
Connection ~ 4950 3800
Wire Wire Line
	4950 3800 4650 3800
Wire Wire Line
	4950 3100 4950 3000
Connection ~ 4950 3000
Wire Wire Line
	4950 3000 4650 3000
Wire Wire Line
	4950 3300 4950 3400
Connection ~ 4950 3400
Wire Wire Line
	4950 3400 5300 3400
Wire Wire Line
	4950 3400 4950 3500
Wire Wire Line
	6050 3300 5900 3300
$Comp
L Device:Rotary_Encoder_Switch SW2
U 1 1 5DB9C09E
P 5600 4500
F 0 "SW2" H 5600 4870 50  0000 C CNN
F 1 "MUSIC_ENCODER" H 5600 4777 50  0000 C CNN
F 2 "" H 5450 4660 50  0001 C CNN
F 3 "~" H 5600 4760 50  0001 C CNN
	1    5600 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 4600 5300 4600
$Comp
L power:GND #PWR014
U 1 1 5DB9C0A9
P 4700 4500
F 0 "#PWR014" H 4700 4250 50  0001 C CNN
F 1 "GND" V 4705 4371 50  0000 R CNN
F 2 "" H 4700 4500 50  0001 C CNN
F 3 "" H 4700 4500 50  0001 C CNN
	1    4700 4500
	0    1    1    0   
$EndComp
Wire Wire Line
	4700 4500 4950 4500
$Comp
L power:GND #PWR018
U 1 1 5DB9C0B4
P 6000 4600
F 0 "#PWR018" H 6000 4350 50  0001 C CNN
F 1 "GND" V 6005 4471 50  0000 R CNN
F 2 "" H 6000 4600 50  0001 C CNN
F 3 "" H 6000 4600 50  0001 C CNN
	1    6000 4600
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5900 4600 6000 4600
Text Label 4650 4100 2    50   ~ 0
ENC2_A
Text Label 4650 4900 2    50   ~ 0
ENC2_B
Text Label 6050 4400 0    50   ~ 0
ENC2_SW
Wire Wire Line
	5300 4400 5150 4400
Wire Wire Line
	5150 4400 5150 4100
Wire Wire Line
	5150 4100 4950 4100
$Comp
L Device:C_Small C3
U 1 1 5DB9C0C5
P 4950 4300
F 0 "C3" H 4859 4253 50  0000 R CNN
F 1 "10nF" H 4859 4346 50  0000 R CNN
F 2 "" H 4950 4300 50  0001 C CNN
F 3 "~" H 4950 4300 50  0001 C CNN
	1    4950 4300
	1    0    0    1   
$EndComp
$Comp
L Device:C_Small C4
U 1 1 5DB9C0CF
P 4950 4700
F 0 "C4" H 4859 4653 50  0000 R CNN
F 1 "10nF" H 4859 4746 50  0000 R CNN
F 2 "" H 4950 4700 50  0001 C CNN
F 3 "~" H 4950 4700 50  0001 C CNN
	1    4950 4700
	1    0    0    1   
$EndComp
Wire Wire Line
	5150 4900 4950 4900
Wire Wire Line
	5150 4600 5150 4900
Wire Wire Line
	4950 4800 4950 4900
Connection ~ 4950 4900
Wire Wire Line
	4950 4900 4650 4900
Wire Wire Line
	4950 4200 4950 4100
Connection ~ 4950 4100
Wire Wire Line
	4950 4100 4650 4100
Wire Wire Line
	4950 4400 4950 4500
Connection ~ 4950 4500
Wire Wire Line
	4950 4500 5300 4500
Wire Wire Line
	4950 4500 4950 4600
Wire Wire Line
	6050 4400 5900 4400
Text Label 1950 1100 0    50   ~ 0
MUSIC_RX
Text Label 950  1200 0    50   ~ 0
MUSIC_TX
$Comp
L Device:R_Small_US R1
U 1 1 5DBAC9CA
P 1700 1100
F 0 "R1" V 1491 1100 50  0000 C CNN
F 1 "1k" V 1584 1100 50  0000 C CNN
F 2 "" H 1700 1100 50  0001 C CNN
F 3 "~" H 1700 1100 50  0001 C CNN
	1    1700 1100
	0    1    1    0   
$EndComp
Wire Wire Line
	1800 1100 1950 1100
Wire Wire Line
	1600 1100 950  1100
NoConn ~ 950  2400
NoConn ~ 950  2300
NoConn ~ 950  2200
NoConn ~ 950  2100
NoConn ~ 950  2000
NoConn ~ 950  1900
NoConn ~ 950  1800
NoConn ~ 950  1400
NoConn ~ 950  1300
Text Label 2400 4050 2    50   ~ 0
MUSIC_RX
Text Label 2400 3950 2    50   ~ 0
MUSIC_TX
Text Label 4000 1800 1    50   ~ 0
LED_WARM
Text Label 2400 4250 2    50   ~ 0
LED_WARM
Text Label 2400 4350 2    50   ~ 0
LED_NEUTRAL
Text Label 2400 4450 2    50   ~ 0
LED_COOL
Text Label 2400 3850 2    50   ~ 0
SDA
Text Label 2400 3750 2    50   ~ 0
SCL
Text Label 3400 3450 0    50   ~ 0
RTC_SQW
Text Label 2400 3150 2    50   ~ 0
MATRIX_CS
Text Label 2400 3250 2    50   ~ 0
MATRIX_CLK
Text Label 2400 3350 2    50   ~ 0
MATRIX_DIN
$Comp
L power:+12V #PWR07
U 1 1 5DC1759C
P 3250 1100
F 0 "#PWR07" H 3250 950 50  0001 C CNN
F 1 "+12V" H 3265 1275 50  0000 C CNN
F 2 "" H 3250 1100 50  0001 C CNN
F 3 "" H 3250 1100 50  0001 C CNN
	1    3250 1100
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D1
U 1 1 5DC175A6
P 3250 1350
F 0 "D1" H 3242 1091 50  0000 C CNN
F 1 "LED" H 3242 1184 50  0000 C CNN
F 2 "" H 3250 1350 50  0001 C CNN
F 3 "~" H 3250 1350 50  0001 C CNN
	1    3250 1350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3250 1500 3250 1600
Wire Wire Line
	3550 1850 3650 1850
Connection ~ 3650 1850
Wire Wire Line
	3650 1850 3750 1850
Wire Wire Line
	3650 1850 3650 2100
Wire Wire Line
	3250 2000 3250 2100
Wire Wire Line
	3250 2100 3350 2100
$Comp
L power:GND #PWR08
U 1 1 5DC175B7
P 3250 2200
F 0 "#PWR08" H 3250 1950 50  0001 C CNN
F 1 "GND" H 3255 2025 50  0000 C CNN
F 2 "" H 3250 2200 50  0001 C CNN
F 3 "" H 3250 2200 50  0001 C CNN
	1    3250 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	3250 2100 3250 2200
Connection ~ 3250 2100
Wire Wire Line
	3250 1100 3250 1200
$Comp
L custom:MOSFET-SOT-23-3 Q1
U 1 1 5DC175C5
P 3350 1800
F 0 "Q1" H 3150 1950 40  0000 L CNN
F 1 "MOSFET-SOT-23-3" V 3600 1500 40  0000 L CNN
F 2 "SOT-23" H 3100 1950 29  0001 L CNN
F 3 "" H 3350 1800 60  0000 C CNN
	1    3350 1800
	-1   0    0    -1  
$EndComp
$Comp
L Device:R_Small_US R3
U 1 1 5DC175CF
P 3850 1850
F 0 "R3" V 3641 1850 50  0000 C CNN
F 1 "1k" V 3734 1850 50  0000 C CNN
F 2 "" H 3850 1850 50  0001 C CNN
F 3 "~" H 3850 1850 50  0001 C CNN
	1    3850 1850
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small_US R2
U 1 1 5DC175D9
P 3450 2100
F 0 "R2" V 3600 2100 50  0000 C CNN
F 1 "1k" V 3334 2100 50  0000 C CNN
F 2 "" H 3450 2100 50  0001 C CNN
F 3 "~" H 3450 2100 50  0001 C CNN
	1    3450 2100
	0    1    1    0   
$EndComp
Wire Wire Line
	3550 2100 3650 2100
Wire Wire Line
	3950 1850 4000 1850
Wire Wire Line
	4000 1850 4000 1800
$Comp
L power:+12V #PWR011
U 1 1 5DC44B34
P 4300 1100
F 0 "#PWR011" H 4300 950 50  0001 C CNN
F 1 "+12V" H 4315 1275 50  0000 C CNN
F 2 "" H 4300 1100 50  0001 C CNN
F 3 "" H 4300 1100 50  0001 C CNN
	1    4300 1100
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D2
U 1 1 5DC44B3E
P 4300 1350
F 0 "D2" H 4292 1091 50  0000 C CNN
F 1 "LED" H 4292 1184 50  0000 C CNN
F 2 "" H 4300 1350 50  0001 C CNN
F 3 "~" H 4300 1350 50  0001 C CNN
	1    4300 1350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4300 1500 4300 1600
Wire Wire Line
	4600 1850 4700 1850
Connection ~ 4700 1850
Wire Wire Line
	4700 1850 4800 1850
Wire Wire Line
	4700 1850 4700 2100
Wire Wire Line
	4300 2000 4300 2100
Wire Wire Line
	4300 2100 4400 2100
$Comp
L power:GND #PWR012
U 1 1 5DC44B4F
P 4300 2200
F 0 "#PWR012" H 4300 1950 50  0001 C CNN
F 1 "GND" H 4305 2025 50  0000 C CNN
F 2 "" H 4300 2200 50  0001 C CNN
F 3 "" H 4300 2200 50  0001 C CNN
	1    4300 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	4300 2100 4300 2200
Connection ~ 4300 2100
Wire Wire Line
	4300 1100 4300 1200
$Comp
L custom:MOSFET-SOT-23-3 Q2
U 1 1 5DC44B5C
P 4400 1800
F 0 "Q2" H 4200 1950 40  0000 L CNN
F 1 "MOSFET-SOT-23-3" V 4650 1500 40  0000 L CNN
F 2 "SOT-23" H 4150 1950 29  0001 L CNN
F 3 "" H 4400 1800 60  0000 C CNN
	1    4400 1800
	-1   0    0    -1  
$EndComp
$Comp
L Device:R_Small_US R5
U 1 1 5DC44B66
P 4900 1850
F 0 "R5" V 4691 1850 50  0000 C CNN
F 1 "1k" V 4784 1850 50  0000 C CNN
F 2 "" H 4900 1850 50  0001 C CNN
F 3 "~" H 4900 1850 50  0001 C CNN
	1    4900 1850
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small_US R4
U 1 1 5DC44B70
P 4500 2100
F 0 "R4" V 4650 2100 50  0000 C CNN
F 1 "1k" V 4384 2100 50  0000 C CNN
F 2 "" H 4500 2100 50  0001 C CNN
F 3 "~" H 4500 2100 50  0001 C CNN
	1    4500 2100
	0    1    1    0   
$EndComp
Wire Wire Line
	4600 2100 4700 2100
Wire Wire Line
	5000 1850 5050 1850
Wire Wire Line
	5050 1850 5050 1800
$Comp
L power:+12V #PWR015
U 1 1 5DC4B4FF
P 5350 1100
F 0 "#PWR015" H 5350 950 50  0001 C CNN
F 1 "+12V" H 5365 1275 50  0000 C CNN
F 2 "" H 5350 1100 50  0001 C CNN
F 3 "" H 5350 1100 50  0001 C CNN
	1    5350 1100
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D3
U 1 1 5DC4B509
P 5350 1350
F 0 "D3" H 5342 1091 50  0000 C CNN
F 1 "LED" H 5342 1184 50  0000 C CNN
F 2 "" H 5350 1350 50  0001 C CNN
F 3 "~" H 5350 1350 50  0001 C CNN
	1    5350 1350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5350 1500 5350 1600
Wire Wire Line
	5650 1850 5750 1850
Connection ~ 5750 1850
Wire Wire Line
	5750 1850 5850 1850
Wire Wire Line
	5750 1850 5750 2100
Wire Wire Line
	5350 2000 5350 2100
Wire Wire Line
	5350 2100 5450 2100
$Comp
L power:GND #PWR016
U 1 1 5DC4B51A
P 5350 2200
F 0 "#PWR016" H 5350 1950 50  0001 C CNN
F 1 "GND" H 5355 2025 50  0000 C CNN
F 2 "" H 5350 2200 50  0001 C CNN
F 3 "" H 5350 2200 50  0001 C CNN
	1    5350 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 2100 5350 2200
Connection ~ 5350 2100
Wire Wire Line
	5350 1100 5350 1200
$Comp
L custom:MOSFET-SOT-23-3 Q3
U 1 1 5DC4B527
P 5450 1800
F 0 "Q3" H 5250 1950 40  0000 L CNN
F 1 "MOSFET-SOT-23-3" V 5700 1500 40  0000 L CNN
F 2 "SOT-23" H 5200 1950 29  0001 L CNN
F 3 "" H 5450 1800 60  0000 C CNN
	1    5450 1800
	-1   0    0    -1  
$EndComp
$Comp
L Device:R_Small_US R7
U 1 1 5DC4B531
P 5950 1850
F 0 "R7" V 5741 1850 50  0000 C CNN
F 1 "1k" V 5834 1850 50  0000 C CNN
F 2 "" H 5950 1850 50  0001 C CNN
F 3 "~" H 5950 1850 50  0001 C CNN
	1    5950 1850
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small_US R6
U 1 1 5DC4B53B
P 5550 2100
F 0 "R6" V 5700 2100 50  0000 C CNN
F 1 "1k" V 5434 2100 50  0000 C CNN
F 2 "" H 5550 2100 50  0001 C CNN
F 3 "~" H 5550 2100 50  0001 C CNN
	1    5550 2100
	0    1    1    0   
$EndComp
Wire Wire Line
	5650 2100 5750 2100
Wire Wire Line
	6050 1850 6100 1850
Wire Wire Line
	6100 1850 6100 1800
Text Label 6100 1800 1    50   ~ 0
LED_COOL
$EndSCHEMATC