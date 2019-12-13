EESchema Schematic File Version 4
LIBS:lynsyn-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 5
Title "Lynsyn JTAG and Power Monitor - MCU"
Date "2019-12-13"
Rev "3.0"
Comp "NTNU"
Comment1 "Asbjørn Djupdal"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L energymicro-efm32:EFM32GG332F1024 U1
U 2 1 5A323439
P 4350 900
F 0 "U1" H 4680 1080 60  0000 L BNN
F 1 "EFM32GG332F1024" H 4980 1080 60  0000 L BNN
F 2 "Package_QFP:TQFP-64_10x10mm_P0.5mm" H 4350 900 60  0001 C CNN
F 3 "" H 4350 900 60  0001 C CNN
	2    4350 900 
	1    0    0    -1  
$EndComp
$Comp
L energymicro-efm32:EFM32GG332F1024 U1
U 3 1 5A3234EC
P 2500 4100
F 0 "U1" H 2830 4280 60  0000 L BNN
F 1 "EFM32GG332F1024" H 3130 4280 60  0000 L BNN
F 2 "Package_QFP:TQFP-64_10x10mm_P0.5mm" H 2500 4100 60  0001 C CNN
F 3 "" H 2500 4100 60  0001 C CNN
	3    2500 4100
	1    0    0    -1  
$EndComp
Text HLabel 2250 4500 0    60   Output ~ 0
CLK0
Text HLabel 2250 4300 0    60   Output ~ 0
TX0
Text HLabel 2250 4400 0    60   Input ~ 0
RX0
Text HLabel 2250 4600 0    60   Output ~ 0
SRST
Text HLabel 4250 2200 0    60   Input ~ 0
ADC0
Text HLabel 4250 2400 0    60   Input ~ 0
ADC2
Text HLabel 4250 2500 0    60   Input ~ 0
ADC3
$Comp
L power:+3.3V #PWR04
U 1 1 5A396CC7
P 975 3475
F 0 "#PWR04" H 975 3325 50  0001 C CNN
F 1 "+3.3V" H 975 3615 50  0000 C CNN
F 2 "" H 975 3475 50  0001 C CNN
F 3 "" H 975 3475 50  0001 C CNN
	1    975  3475
	1    0    0    -1  
$EndComp
$Comp
L lynsyn-rescue:USB_OTG J?
U 1 1 5A3977D7
P 1150 5750
AR Path="/5A3977D7" Ref="J?"  Part="1" 
AR Path="/5A30ECDB/5A3977D7" Ref="J1"  Part="1" 
F 0 "J1" H 950 6200 50  0000 L CNN
F 1 "USB Micro-B" H 900 6100 50  0000 L CNN
F 2 "Connector_USB:USB_Micro-B_Wuerth_629105150521_CircularHoles" H 1300 5700 50  0001 C CNN
F 3 "" H 1300 5700 50  0001 C CNN
	1    1150 5750
	1    0    0    -1  
$EndComp
$Comp
L lynsyn-rescue:R R3
U 1 1 5A399013
P 2250 5400
F 0 "R3" V 2330 5400 50  0000 C CNN
F 1 "15" V 2250 5400 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 2180 5400 50  0001 C CNN
F 3 "" H 2250 5400 50  0001 C CNN
	1    2250 5400
	0    1    1    0   
$EndComp
$Comp
L lynsyn-rescue:R R2
U 1 1 5A399080
P 1900 5500
F 0 "R2" V 1980 5500 50  0000 C CNN
F 1 "15" V 1900 5500 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 1830 5500 50  0001 C CNN
F 3 "" H 1900 5500 50  0001 C CNN
	1    1900 5500
	0    1    1    0   
$EndComp
$Comp
L lynsyn-rescue:Ferrite_Bead L1
U 1 1 5A39921E
P 1050 6450
F 0 "L1" V 900 6475 50  0000 C CNN
F 1 "Ferrite_Bead" V 1200 6450 50  0000 C CNN
F 2 "Inductor_SMD:L_0805_2012Metric" V 980 6450 50  0001 C CNN
F 3 "" H 1050 6450 50  0001 C CNN
	1    1050 6450
	1    0    0    -1  
$EndComp
$Comp
L lynsyn-rescue:Ferrite_Bead L2
U 1 1 5A39957D
P 1300 6800
F 0 "L2" V 1150 6825 50  0000 C CNN
F 1 "Ferrite_Bead" V 1450 6800 50  0000 C CNN
F 2 "Inductor_SMD:L_0805_2012Metric" V 1230 6800 50  0001 C CNN
F 3 "" H 1300 6800 50  0001 C CNN
	1    1300 6800
	1    0    0    -1  
$EndComp
$Comp
L lynsyn-rescue:Ferrite_Bead L3
U 1 1 5A3995E0
P 1900 6200
F 0 "L3" V 1750 6225 50  0000 C CNN
F 1 "Ferrite_Bead" V 2050 6200 50  0000 C CNN
F 2 "Inductor_SMD:L_0805_2012Metric" V 1830 6200 50  0001 C CNN
F 3 "" H 1900 6200 50  0001 C CNN
	1    1900 6200
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR05
U 1 1 5A3996BF
P 1150 7150
F 0 "#PWR05" H 1150 6900 50  0001 C CNN
F 1 "GND" H 1150 7000 50  0000 C CNN
F 2 "" H 1150 7150 50  0001 C CNN
F 3 "" H 1150 7150 50  0001 C CNN
	1    1150 7150
	1    0    0    -1  
$EndComp
$Comp
L lynsyn-rescue:CP1 C11
U 1 1 5A39C711
P 6000 6725
F 0 "C11" H 6025 6825 50  0000 L CNN
F 1 "10u" H 6025 6625 50  0000 L CNN
F 2 "Capacitor_Tantalum_SMD:CP_EIA-2012-12_Kemet-R" H 6000 6725 50  0001 C CNN
F 3 "" H 6000 6725 50  0001 C CNN
	1    6000 6725
	1    0    0    -1  
$EndComp
$Comp
L lynsyn-rescue:C C?
U 1 1 5A39C824
P 5400 6725
AR Path="/5A39C824" Ref="C?"  Part="1" 
AR Path="/5A30ECDB/5A39C824" Ref="C9"  Part="1" 
F 0 "C9" H 5425 6825 50  0000 L CNN
F 1 "100n" H 5425 6625 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 5438 6575 50  0001 C CNN
F 3 "" H 5400 6725 50  0001 C CNN
	1    5400 6725
	1    0    0    -1  
$EndComp
$Comp
L lynsyn-rescue:C C?
U 1 1 5A39C90B
P 5650 6725
AR Path="/5A39C90B" Ref="C?"  Part="1" 
AR Path="/5A30ECDB/5A39C90B" Ref="C10"  Part="1" 
F 0 "C10" H 5675 6825 50  0000 L CNN
F 1 "100n" H 5675 6625 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 5688 6575 50  0001 C CNN
F 3 "" H 5650 6725 50  0001 C CNN
	1    5650 6725
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR012
U 1 1 5A39D228
P 3400 2050
F 0 "#PWR012" H 3400 1800 50  0001 C CNN
F 1 "GND" H 3400 1900 50  0000 C CNN
F 2 "" H 3400 2050 50  0001 C CNN
F 3 "" H 3400 2050 50  0001 C CNN
	1    3400 2050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR018
U 1 1 5A39D42F
P 6000 6975
F 0 "#PWR018" H 6000 6725 50  0001 C CNN
F 1 "GND" H 6000 6825 50  0000 C CNN
F 2 "" H 6000 6975 50  0001 C CNN
F 3 "" H 6000 6975 50  0001 C CNN
	1    6000 6975
	1    0    0    -1  
$EndComp
$Comp
L lynsyn-rescue:C C?
U 1 1 5A39F2B0
P 5150 6725
AR Path="/5A39F2B0" Ref="C?"  Part="1" 
AR Path="/5A30ECDB/5A39F2B0" Ref="C8"  Part="1" 
F 0 "C8" H 5175 6825 50  0000 L CNN
F 1 "100n" H 5175 6625 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 5188 6575 50  0001 C CNN
F 3 "" H 5150 6725 50  0001 C CNN
	1    5150 6725
	1    0    0    -1  
$EndComp
$Comp
L lynsyn-rescue:C C?
U 1 1 5A39F308
P 4900 6725
AR Path="/5A39F308" Ref="C?"  Part="1" 
AR Path="/5A30ECDB/5A39F308" Ref="C7"  Part="1" 
F 0 "C7" H 4925 6825 50  0000 L CNN
F 1 "100n" H 4925 6625 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 4938 6575 50  0001 C CNN
F 3 "" H 4900 6725 50  0001 C CNN
	1    4900 6725
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR02
U 1 1 5A39FF27
P 900 3050
F 0 "#PWR02" H 900 2800 50  0001 C CNN
F 1 "GND" H 900 2900 50  0000 C CNN
F 2 "" H 900 3050 50  0001 C CNN
F 3 "" H 900 3050 50  0001 C CNN
	1    900  3050
	1    0    0    -1  
$EndComp
$Comp
L lynsyn:IP4220CZ6 U2
U 1 1 5A3A94FC
P 2300 7050
F 0 "U2" H 2300 7550 60  0000 C CNN
F 1 "IP4220CZ6" H 2300 7050 60  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-6" H 2300 7050 60  0001 C CNN
F 3 "" H 2300 7050 60  0001 C CNN
	1    2300 7050
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR03
U 1 1 5A3D3295
P 975 875
F 0 "#PWR03" H 975 725 50  0001 C CNN
F 1 "+3.3V" H 975 1015 50  0000 C CNN
F 2 "" H 975 875 50  0001 C CNN
F 3 "" H 975 875 50  0001 C CNN
	1    975  875 
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR014
U 1 1 5A3D3BAB
P 4250 1925
F 0 "#PWR014" H 4250 1675 50  0001 C CNN
F 1 "GND" H 4250 1775 50  0000 C CNN
F 2 "" H 4250 1925 50  0001 C CNN
F 3 "" H 4250 1925 50  0001 C CNN
	1    4250 1925
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR015
U 1 1 5A3D3F68
P 4300 3000
F 0 "#PWR015" H 4300 2750 50  0001 C CNN
F 1 "GND" H 4300 2850 50  0000 C CNN
F 2 "" H 4300 3000 50  0001 C CNN
F 3 "" H 4300 3000 50  0001 C CNN
	1    4300 3000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR08
U 1 1 5A3D84C9
P 2350 3900
F 0 "#PWR08" H 2350 3650 50  0001 C CNN
F 1 "GND" H 2350 3750 50  0000 C CNN
F 2 "" H 2350 3900 50  0001 C CNN
F 3 "" H 2350 3900 50  0001 C CNN
	1    2350 3900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR09
U 1 1 5A3D8673
P 2350 4750
F 0 "#PWR09" H 2350 4500 50  0001 C CNN
F 1 "GND" H 2350 4600 50  0000 C CNN
F 2 "" H 2350 4750 50  0001 C CNN
F 3 "" H 2350 4750 50  0001 C CNN
	1    2350 4750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR07
U 1 1 5A3D8773
P 2300 5600
F 0 "#PWR07" H 2300 5350 50  0001 C CNN
F 1 "GND" H 2300 5450 50  0000 C CNN
F 2 "" H 2300 5600 50  0001 C CNN
F 3 "" H 2300 5600 50  0001 C CNN
	1    2300 5600
	1    0    0    -1  
$EndComp
$Comp
L lynsyn-rescue:C C?
U 1 1 5A34994A
P 3400 1850
AR Path="/5A34994A" Ref="C?"  Part="1" 
AR Path="/5A30ECDB/5A34994A" Ref="C3"  Part="1" 
F 0 "C3" H 3425 1950 50  0000 L CNN
F 1 "1u" H 3425 1750 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 3438 1700 50  0001 C CNN
F 3 "" H 3400 1850 50  0001 C CNN
	1    3400 1850
	1    0    0    -1  
$EndComp
$Comp
L lynsyn-rescue:C C?
U 1 1 5A349BA1
P 1050 2800
AR Path="/5A349BA1" Ref="C?"  Part="1" 
AR Path="/5A30ECDB/5A349BA1" Ref="C2"  Part="1" 
F 0 "C2" H 1075 2900 50  0000 L CNN
F 1 "1u" H 1075 2700 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 1088 2650 50  0001 C CNN
F 3 "" H 1050 2800 50  0001 C CNN
	1    1050 2800
	1    0    0    -1  
$EndComp
$Comp
L lynsyn-rescue:C C?
U 1 1 5A349CF7
P 750 2550
AR Path="/5A349CF7" Ref="C?"  Part="1" 
AR Path="/5A30ECDB/5A349CF7" Ref="C1"  Part="1" 
F 0 "C1" H 775 2650 50  0000 L CNN
F 1 "4.7u" H 775 2450 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 788 2400 50  0001 C CNN
F 3 "" H 750 2550 50  0001 C CNN
	1    750  2550
	1    0    0    -1  
$EndComp
Text GLabel 2150 6200 2    60   Input ~ 0
USB_VBUS
Text GLabel 1150 2000 0    60   Input ~ 0
USB_VBUS
$Comp
L power:GND #PWR011
U 1 1 5A34FDBB
P 3150 3525
F 0 "#PWR011" H 3150 3275 50  0001 C CNN
F 1 "GND" H 3150 3375 50  0000 C CNN
F 2 "" H 3150 3525 50  0001 C CNN
F 3 "" H 3150 3525 50  0001 C CNN
	1    3150 3525
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR010
U 1 1 5A34FF76
P 3150 2250
F 0 "#PWR010" H 3150 2100 50  0001 C CNN
F 1 "+3.3V" H 3150 2390 50  0000 C CNN
F 2 "" H 3150 2250 50  0001 C CNN
F 3 "" H 3150 2250 50  0001 C CNN
	1    3150 2250
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR017
U 1 1 5A350653
P 4900 6425
F 0 "#PWR017" H 4900 6275 50  0001 C CNN
F 1 "+3.3V" H 4900 6565 50  0000 C CNN
F 2 "" H 4900 6425 50  0001 C CNN
F 3 "" H 4900 6425 50  0001 C CNN
	1    4900 6425
	1    0    0    -1  
$EndComp
$Comp
L lynsyn-rescue:CP1 C6
U 1 1 5A350D3B
P 4525 6725
F 0 "C6" H 4550 6825 50  0000 L CNN
F 1 "10u" H 4550 6625 50  0000 L CNN
F 2 "Capacitor_Tantalum_SMD:CP_EIA-2012-12_Kemet-R" H 4525 6725 50  0001 C CNN
F 3 "" H 4525 6725 50  0001 C CNN
	1    4525 6725
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR016
U 1 1 5A350D4D
P 4525 6975
F 0 "#PWR016" H 4525 6725 50  0001 C CNN
F 1 "GND" H 4525 6825 50  0000 C CNN
F 2 "" H 4525 6975 50  0001 C CNN
F 3 "" H 4525 6975 50  0001 C CNN
	1    4525 6975
	1    0    0    -1  
$EndComp
$Comp
L lynsyn-rescue:C C?
U 1 1 5A350D53
P 4200 6725
AR Path="/5A350D53" Ref="C?"  Part="1" 
AR Path="/5A30ECDB/5A350D53" Ref="C5"  Part="1" 
F 0 "C5" H 4225 6825 50  0000 L CNN
F 1 "100n" H 4225 6625 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 4238 6575 50  0001 C CNN
F 3 "" H 4200 6725 50  0001 C CNN
	1    4200 6725
	1    0    0    -1  
$EndComp
$Comp
L lynsyn-rescue:C C?
U 1 1 5A350D59
P 3950 6725
AR Path="/5A350D59" Ref="C?"  Part="1" 
AR Path="/5A30ECDB/5A350D59" Ref="C4"  Part="1" 
F 0 "C4" H 3975 6825 50  0000 L CNN
F 1 "100n" H 3975 6625 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 3988 6575 50  0001 C CNN
F 3 "" H 3950 6725 50  0001 C CNN
	1    3950 6725
	1    0    0    -1  
$EndComp
$Comp
L energymicro-efm32:EFM32GG332F1024 U1
U 4 1 5A323537
P 1550 1600
F 0 "U1" H 2250 1900 60  0000 L BNN
F 1 "EFM32GG332F1024" H 1925 1775 60  0000 L BNN
F 2 "Package_QFP:TQFP-64_10x10mm_P0.5mm" H 1550 1600 60  0001 C CNN
F 3 "" H 1550 1600 60  0001 C CNN
	4    1550 1600
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3VA #PWR06
U 1 1 5A3512F7
P 1550 2600
F 0 "#PWR06" H 1550 2450 50  0001 C CNN
F 1 "+3.3VA" H 1550 2740 50  0000 C CNN
F 2 "" H 1550 2600 50  0001 C CNN
F 3 "" H 1550 2600 50  0001 C CNN
	1    1550 2600
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3VA #PWR013
U 1 1 5A3513D1
P 3950 6425
F 0 "#PWR013" H 3950 6275 50  0001 C CNN
F 1 "+3.3VA" H 3950 6565 50  0000 C CNN
F 2 "" H 3950 6425 50  0001 C CNN
F 3 "" H 3950 6425 50  0001 C CNN
	1    3950 6425
	1    0    0    -1  
$EndComp
NoConn ~ 1850 6700
NoConn ~ 1850 6900
NoConn ~ 1450 5950
$Comp
L power:PWR_FLAG #FLG02
U 1 1 5A348ED4
P 1500 5450
F 0 "#FLG02" H 1500 5525 50  0001 C CNN
F 1 "PWR_FLAG" H 1500 5600 50  0000 C CNN
F 2 "" H 1500 5450 50  0001 C CNN
F 3 "" H 1500 5450 50  0001 C CNN
	1    1500 5450
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG01
U 1 1 5A3497D1
P 1350 6250
F 0 "#FLG01" H 1350 6325 50  0001 C CNN
F 1 "PWR_FLAG" H 1350 6400 50  0000 C CNN
F 2 "" H 1350 6250 50  0001 C CNN
F 3 "" H 1350 6250 50  0001 C CNN
	1    1350 6250
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG03
U 1 1 5A34AA76
P 2100 6100
F 0 "#FLG03" H 2100 6175 50  0001 C CNN
F 1 "PWR_FLAG" H 2100 6250 50  0000 C CNN
F 2 "" H 2100 6100 50  0001 C CNN
F 3 "" H 2100 6100 50  0001 C CNN
	1    2100 6100
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG04
U 1 1 5A34B0AA
P 3225 1550
F 0 "#FLG04" H 3225 1625 50  0001 C CNN
F 1 "PWR_FLAG" H 3225 1700 50  0000 C CNN
F 2 "" H 3225 1550 50  0001 C CNN
F 3 "" H 3225 1550 50  0001 C CNN
	1    3225 1550
	1    0    0    -1  
$EndComp
$Comp
L lynsyn-rescue:C C?
U 1 1 5A34E926
P 7550 5750
AR Path="/5A34E926" Ref="C?"  Part="1" 
AR Path="/5A30ECDB/5A34E926" Ref="C12"  Part="1" 
F 0 "C12" H 7575 5850 50  0000 L CNN
F 1 "12p" H 7575 5650 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 7588 5600 50  0001 C CNN
F 3 "" H 7550 5750 50  0001 C CNN
	1    7550 5750
	1    0    0    -1  
$EndComp
$Comp
L lynsyn-rescue:C C?
U 1 1 5A34EA28
P 8300 5750
AR Path="/5A34EA28" Ref="C?"  Part="1" 
AR Path="/5A30ECDB/5A34EA28" Ref="C13"  Part="1" 
F 0 "C13" H 8325 5850 50  0000 L CNN
F 1 "12p" H 8325 5650 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 8338 5600 50  0001 C CNN
F 3 "" H 8300 5750 50  0001 C CNN
	1    8300 5750
	1    0    0    -1  
$EndComp
$Comp
L lynsyn-rescue:Crystal_GND24 Y1
U 1 1 5A34EA8A
P 7925 5550
F 0 "Y1" H 7800 5350 50  0000 L CNN
F 1 "48MHz" H 7650 5750 50  0000 L CNN
F 2 "Crystal:Crystal_SMD_3225-4Pin_3.2x2.5mm" H 7925 5550 50  0001 C CNN
F 3 "" H 7925 5550 50  0001 C CNN
	1    7925 5550
	-1   0    0    -1  
$EndComp
$Comp
L energymicro-efm32:EFM32GG332F1024 U1
U 1 1 5A32334C
P 7650 3450
F 0 "U1" H 7980 3630 60  0000 L BNN
F 1 "EFM32GG332F1024" H 8280 3630 60  0000 L BNN
F 2 "Package_QFP:TQFP-64_10x10mm_P0.5mm" H 7650 3450 60  0001 C CNN
F 3 "" H 7650 3450 60  0001 C CNN
	1    7650 3450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR021
U 1 1 5A34F38A
P 7925 6000
F 0 "#PWR021" H 7925 5750 50  0001 C CNN
F 1 "GND" H 7925 5850 50  0000 C CNN
F 2 "" H 7925 6000 50  0001 C CNN
F 3 "" H 7925 6000 50  0001 C CNN
	1    7925 6000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR019
U 1 1 5A34F634
P 7450 4675
F 0 "#PWR019" H 7450 4425 50  0001 C CNN
F 1 "GND" H 7450 4525 50  0000 C CNN
F 2 "" H 7450 4675 50  0001 C CNN
F 3 "" H 7450 4675 50  0001 C CNN
	1    7450 4675
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR020
U 1 1 5A34FFC0
P 7700 5225
F 0 "#PWR020" H 7700 4975 50  0001 C CNN
F 1 "GND" H 7700 5075 50  0000 C CNN
F 2 "" H 7700 5225 50  0001 C CNN
F 3 "" H 7700 5225 50  0001 C CNN
	1    7700 5225
	1    0    0    -1  
$EndComp
Text GLabel 3850 2800 0    60   Input ~ 0
AREF
$Comp
L lynsyn-rescue:LED_ALT D2
U 1 1 5A397CBC
P 1275 1000
F 0 "D2" H 1275 1100 50  0000 C CNN
F 1 "Green" H 1100 1075 50  0000 C CNN
F 2 "LED_SMD:LED_0805_2012Metric" H 1275 1000 50  0001 C CNN
F 3 "" H 1275 1000 50  0001 C CNN
	1    1275 1000
	-1   0    0    1   
$EndComp
$Comp
L lynsyn-rescue:R R1
U 1 1 5A397ECD
P 1675 1000
F 0 "R1" V 1755 1000 50  0000 C CNN
F 1 "91" V 1675 1000 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 1605 1000 50  0001 C CNN
F 3 "" H 1675 1000 50  0001 C CNN
	1    1675 1000
	0    1    1    0   
$EndComp
Text Label 2925 5975 0    60   ~ 0
USB_D+
Text Label 2925 6075 0    60   ~ 0
USB_D-
Text Label 1525 5100 2    60   ~ 0
MCU_D-
Text Label 1525 5200 2    60   ~ 0
MCU_D+
Wire Wire Line
	4250 2200 4350 2200
Wire Wire Line
	4250 2300 4350 2300
Wire Wire Line
	4250 2400 4350 2400
Wire Wire Line
	4250 2500 4350 2500
Wire Wire Line
	3850 2800 4350 2800
Wire Wire Line
	1400 1600 1550 1600
Wire Wire Line
	2400 5400 2425 5400
Wire Wire Line
	2050 5500 2075 5500
Wire Wire Line
	2100 5400 1675 5400
Wire Wire Line
	1675 5400 1675 5850
Wire Wire Line
	1450 5850 1675 5850
Wire Wire Line
	1450 5750 1725 5750
Wire Wire Line
	1050 6600 1050 7050
Wire Wire Line
	1050 7050 1150 7050
Wire Wire Line
	1150 7050 1150 7150
Wire Wire Line
	1300 7050 1300 6950
Connection ~ 1150 7050
Wire Wire Line
	1300 6650 1300 6300
Wire Wire Line
	1150 6300 1300 6300
Wire Wire Line
	1150 6300 1150 6150
Wire Wire Line
	1050 6300 1050 6150
Wire Wire Line
	1450 5550 1500 5550
Wire Wire Line
	1600 5550 1600 6200
Wire Wire Line
	1600 6200 1750 6200
Wire Wire Line
	2050 6200 2100 6200
Wire Wire Line
	1150 2000 1500 2000
Connection ~ 5650 6875
Wire Wire Line
	3150 1600 3225 1600
Wire Wire Line
	3150 2250 3150 2400
Connection ~ 3150 2700
Connection ~ 3150 2600
Wire Wire Line
	3400 2050 3400 2000
Wire Wire Line
	3150 3200 3150 3300
Connection ~ 3150 3300
Connection ~ 5400 6875
Connection ~ 3150 3400
Wire Wire Line
	6000 6875 6000 6975
Connection ~ 5150 6875
Wire Wire Line
	4900 6575 5150 6575
Connection ~ 5650 6575
Connection ~ 5400 6575
Connection ~ 5150 6575
Wire Wire Line
	1550 2600 1550 2700
Connection ~ 1550 2700
Wire Wire Line
	1550 2300 1050 2300
Wire Wire Line
	1050 2300 1050 2650
Wire Wire Line
	750  2200 1500 2200
Wire Wire Line
	750  2200 750  2400
Wire Wire Line
	1500 2000 1500 2200
Connection ~ 1500 2200
Connection ~ 1500 2000
Wire Wire Line
	750  2700 750  3000
Wire Wire Line
	750  3000 900  3000
Wire Wire Line
	900  3000 900  3050
Wire Wire Line
	1050 3000 1050 2950
Connection ~ 900  3000
Wire Wire Line
	1650 7050 1650 6800
Wire Wire Line
	1650 6800 1850 6800
Connection ~ 1300 7050
Wire Wire Line
	2750 6800 3000 6800
Wire Wire Line
	3000 6800 3000 6450
Wire Wire Line
	3000 6450 1600 6450
Connection ~ 1600 6200
Wire Wire Line
	2750 5850 2750 6075
Connection ~ 1675 5850
Wire Wire Line
	2800 6900 2750 6900
Wire Wire Line
	2800 5800 2800 5975
Wire Wire Line
	2800 5800 2050 5800
Wire Wire Line
	2050 5800 2050 5750
Connection ~ 1725 5750
Wire Wire Line
	2250 4300 2500 4300
Wire Wire Line
	2250 4400 2500 4400
Wire Wire Line
	2500 4500 2250 4500
Wire Wire Line
	2500 4600 2250 4600
Connection ~ 4350 1600
Connection ~ 4350 1700
Connection ~ 4350 1800
Connection ~ 4350 1900
Wire Wire Line
	4350 1900 4250 1900
Wire Wire Line
	4300 3000 4350 3000
Wire Wire Line
	7650 3850 7650 3950
Connection ~ 7650 3950
Connection ~ 7650 4050
Wire Wire Line
	2500 3850 2500 4100
Wire Wire Line
	2500 3850 2350 3850
Wire Wire Line
	2350 3850 2350 3900
Connection ~ 2500 4100
Wire Wire Line
	2300 5600 2475 5600
Wire Wire Line
	2500 5300 2475 5300
Wire Wire Line
	2475 5300 2475 5600
Connection ~ 2475 5600
Connection ~ 3150 2400
Wire Wire Line
	4900 6875 5150 6875
Wire Wire Line
	4900 6575 4900 6425
Wire Wire Line
	4525 6875 4525 6975
Connection ~ 4200 6875
Wire Wire Line
	3950 6575 4200 6575
Connection ~ 4200 6575
Wire Wire Line
	3950 6875 4200 6875
Wire Wire Line
	3950 6575 3950 6425
Wire Wire Line
	1500 5450 1500 5550
Connection ~ 1500 5550
Wire Wire Line
	1350 6300 1350 6250
Connection ~ 1300 6300
Wire Wire Line
	2100 6100 2100 6200
Connection ~ 2100 6200
Wire Wire Line
	3225 1600 3225 1550
Connection ~ 3225 1600
Wire Wire Line
	7550 4750 7550 5550
Wire Wire Line
	7550 5550 7775 5550
Wire Wire Line
	8075 5550 8300 5550
Wire Wire Line
	8300 5125 8300 5550
Wire Wire Line
	7550 5900 7550 5950
Wire Wire Line
	8300 5950 8300 5900
Wire Wire Line
	7925 5750 7925 5950
Connection ~ 7925 5950
Wire Wire Line
	7450 4675 7450 4650
Wire Wire Line
	7450 4650 7650 4650
Connection ~ 7650 4550
Connection ~ 7650 4450
Connection ~ 7650 4250
Wire Wire Line
	7550 4750 7650 4750
Connection ~ 7550 5550
Wire Wire Line
	7650 4850 7650 5125
Wire Wire Line
	7650 5125 8300 5125
Connection ~ 8300 5550
Wire Wire Line
	7550 5950 7925 5950
Wire Wire Line
	7700 5225 7700 5200
Wire Wire Line
	7700 5200 7925 5200
Wire Wire Line
	7925 5200 7925 5350
Wire Wire Line
	975  1000 1125 1000
Wire Wire Line
	1525 1000 1425 1000
Wire Wire Line
	3400 1600 3400 1700
Wire Wire Line
	1825 1000 4350 1000
Wire Wire Line
	2500 4750 2350 4750
Connection ~ 7650 4150
Wire Wire Line
	2925 5975 2800 5975
Connection ~ 2800 5975
Wire Wire Line
	2925 6075 2750 6075
Connection ~ 2750 6075
Wire Wire Line
	1525 5100 1750 5100
Wire Wire Line
	1750 5100 1750 5300
Wire Wire Line
	1750 5300 2425 5300
Wire Wire Line
	1525 5200 1700 5200
Wire Wire Line
	1700 5200 1700 5350
Wire Wire Line
	1700 5350 2075 5350
Wire Wire Line
	1725 5750 1725 5500
Wire Wire Line
	1725 5500 1750 5500
Wire Wire Line
	2425 5300 2425 5400
Connection ~ 2425 5400
Wire Wire Line
	2075 5350 2075 5500
Connection ~ 2075 5500
Connection ~ 4350 1500
Connection ~ 4350 1400
Wire Wire Line
	1150 7050 1300 7050
Wire Wire Line
	5650 6875 6000 6875
Wire Wire Line
	3150 2700 3150 2800
Wire Wire Line
	3150 2600 3150 2700
Wire Wire Line
	3150 3300 3150 3400
Wire Wire Line
	5400 6875 5650 6875
Wire Wire Line
	3150 3400 3150 3525
Wire Wire Line
	5150 6875 5400 6875
Wire Wire Line
	5650 6575 6000 6575
Wire Wire Line
	5400 6575 5650 6575
Wire Wire Line
	5150 6575 5400 6575
Wire Wire Line
	1550 2700 1550 2800
Wire Wire Line
	1500 2200 1550 2200
Wire Wire Line
	1500 2000 1550 2000
Wire Wire Line
	900  3000 1050 3000
Wire Wire Line
	1300 7050 1650 7050
Wire Wire Line
	1600 6200 1600 6450
Wire Wire Line
	1675 5850 2750 5850
Wire Wire Line
	1725 5750 2050 5750
Wire Wire Line
	4350 1600 4350 1700
Wire Wire Line
	4350 1700 4350 1800
Wire Wire Line
	4350 1800 4350 1900
Wire Wire Line
	4350 1900 4350 2000
Wire Wire Line
	7650 3950 7650 4050
Wire Wire Line
	7650 4050 7650 4150
Wire Wire Line
	2500 4100 2500 4200
Wire Wire Line
	2475 5600 2500 5600
Wire Wire Line
	3150 2400 3150 2600
Wire Wire Line
	4200 6875 4525 6875
Wire Wire Line
	4200 6575 4525 6575
Wire Wire Line
	1500 5550 1600 5550
Wire Wire Line
	1300 6300 1350 6300
Wire Wire Line
	2100 6200 2150 6200
Wire Wire Line
	3225 1600 3400 1600
Wire Wire Line
	7925 5950 7925 6000
Wire Wire Line
	7925 5950 8300 5950
Wire Wire Line
	7650 4550 7650 4650
Wire Wire Line
	7650 4450 7650 4550
Wire Wire Line
	7650 4250 7650 4450
Wire Wire Line
	7550 5550 7550 5600
Wire Wire Line
	8300 5550 8300 5600
Wire Wire Line
	7650 4150 7650 4250
Wire Wire Line
	2800 5975 2800 6900
Wire Wire Line
	2750 6075 2750 6700
Wire Wire Line
	2425 5400 2500 5400
Wire Wire Line
	2075 5500 2500 5500
Wire Wire Line
	4350 1300 4350 1400
Wire Wire Line
	4350 1500 4350 1600
Wire Wire Line
	4350 1400 4350 1500
Text HLabel 4250 2300 0    60   Input ~ 0
ADC1
Wire Wire Line
	7650 3850 7650 3750
Connection ~ 7650 3850
Wire Wire Line
	7650 3750 7650 3650
Connection ~ 7650 3750
Wire Wire Line
	7650 3650 7650 3550
Connection ~ 7650 3650
Wire Wire Line
	7650 3550 7650 3450
Connection ~ 7650 3550
Wire Wire Line
	2500 4700 2500 4750
Wire Wire Line
	975  875  975  1000
Text HLabel 4225 900  0    60   Output ~ 0
TX1
Wire Wire Line
	4350 900  4225 900 
Connection ~ 2500 4750
Wire Wire Line
	2500 4750 2500 4800
Wire Wire Line
	4350 2900 4350 3000
Connection ~ 4350 3000
$Comp
L Connector:Conn_ARM_JTAG_SWD_10 J5
U 1 1 5D168E86
P 975 4150
F 0 "J5" H 1175 3575 50  0000 R CNN
F 1 "Cortex Debug" H 1600 3500 50  0000 R CNN
F 2 "Connector_PinHeader_1.27mm:PinHeader_2x05_P1.27mm_Vertical_SMD" H 1025 3600 50  0001 L TNN
F 3 "http://infocenter.arm.com/help/topic/com.arm.doc.faqs/attached/13634/cortex_debug_connectors.pdf" V 625 2900 50  0001 C CNN
	1    975  4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	1600 3100 1400 3100
Wire Wire Line
	1400 3100 1400 1600
Wire Wire Line
	2500 5000 1900 5000
Wire Wire Line
	2500 5100 1850 5100
Wire Wire Line
	2500 5200 1800 5200
Wire Wire Line
	875  4750 975  4750
$Comp
L power:GND #PWR01
U 1 1 5D1B6CF9
P 875 4825
F 0 "#PWR01" H 875 4575 50  0001 C CNN
F 1 "GND" H 875 4675 50  0000 C CNN
F 2 "" H 875 4825 50  0001 C CNN
F 3 "" H 875 4825 50  0001 C CNN
	1    875  4825
	1    0    0    -1  
$EndComp
Wire Wire Line
	875  4825 875  4750
Connection ~ 875  4750
Wire Wire Line
	975  3550 975  3475
Wire Wire Line
	1475 3850 1600 3850
Wire Wire Line
	1600 3100 1600 3850
Wire Wire Line
	1900 5000 1900 4050
Wire Wire Line
	1900 4050 1475 4050
Wire Wire Line
	1850 5100 1850 4150
Wire Wire Line
	1850 4150 1475 4150
Wire Wire Line
	1800 5200 1800 4250
Wire Wire Line
	1800 4250 1475 4250
NoConn ~ 1475 4350
Wire Wire Line
	4225 1100 4350 1100
Wire Wire Line
	4050 1100 4125 1100
Wire Wire Line
	4250 1925 4250 1900
Text HLabel 4250 2600 0    60   Input ~ 0
ADC4
Text HLabel 4250 2700 0    60   Input ~ 0
ADC5
Wire Wire Line
	4250 2600 4350 2600
Wire Wire Line
	4250 2700 4350 2700
$Comp
L Connector_Generic:Conn_02x02_Odd_Even J4
U 1 1 5D7CBC23
P 3925 1100
F 0 "J4" H 3975 900 50  0000 C CNN
F 1 "Sync" H 3975 825 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x02_P2.54mm_Vertical_SMD" H 3925 1100 50  0001 C CNN
F 3 "~" H 3925 1100 50  0001 C CNN
	1    3925 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	4350 1300 4350 1200
Connection ~ 4350 1300
Wire Wire Line
	4225 1200 4225 1100
Connection ~ 4225 1100
Wire Wire Line
	3725 1100 3725 1200
$Comp
L power:GND #PWR051
U 1 1 5D7F1AAC
P 3725 1250
F 0 "#PWR051" H 3725 1000 50  0001 C CNN
F 1 "GND" H 3725 1100 50  0000 C CNN
F 2 "" H 3725 1250 50  0001 C CNN
F 3 "" H 3725 1250 50  0001 C CNN
	1    3725 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3725 1250 3725 1200
Connection ~ 3725 1200
$EndSCHEMATC
