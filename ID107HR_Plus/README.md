ID107HR Plus
============

This "Smart Band" is made by  	Shenzhen DO Intelligent Technology Co., Ltd.
http://www.idoosmart.com/

http://www.idoosmart.com/ID107PlusHR/

Main processor nRF58232QFAA

KX022.	Accelerometer is KX022  ( http://kionixfs.kionix.com/en/datasheet/KX022-1020%20Specifications%20Rev%2012.0.pdf )
This is probably connected via SPI, as the NCS pin is connected to the nRF52, if the hardware was designed to 
communicate via I2C this would be pulled high all the time.

Si1143.	Heart rate sensing uses a Si1143 optical sensor. https://www.silabs.com/documents/public/data-sheets/Si114x.pdf
This device is I2C only.

Macronix MX25L. SPI flash memory 

Azeteq S263 capacitive touch controller

OLED display (128x64) - appears to use SH1106 or SH1107 controller protocol
Pin 26 of the MCU seems to control whether the display is turned on.
Commands can be written to the display if 26 is LOW, but they will not be visible until 26 is set to HIGH.
Pin 26 seems to control pins 2,3 and 6 on the ribbon cable to the display


Processor pin mapping seems to be as follows.
Note some pins may not be connected, or their use has yet to be determined.

0	
1
2
3	KX022_SDA
4	KX022_ADDR
5	KX022_SCL
6	KX022_INT
7	KX022_NCS
8	SI1143_INT
9	SI1143_LED
10	SI1143_SDA
11
12
13	
14
15	S263_SCL
16	S263_SDA
17	S263_RDY
18	SI1143_SCL
19	OLED_CS
20	OLED_RST
21	
22	OLED_DC
23	TEST_PAD_1 
24	TEST_PAD_2
25	VIBRATION (HIGH = ON)
26	OLED_POWER_CONTROL (HIGH = ON)
27	MX25_SO
28	MX25_CE
29
30	MX25_SCK
31	MX25_SI