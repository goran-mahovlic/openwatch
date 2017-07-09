/*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
* BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*
*  KX022_SPI Accelerometer library
*
*  @author  Goran Mahovlić
*  @version 1.0
*  @date    26-May-2017
*
*  Library for "KX022_SPI Accelerometer library" from Kionix a Rohm group
*    http://www.kionix.com/product/KX022_SPI-1020
*
*/


#include "KX022_SPI.h"

KX022_SPI::KX022_SPI(int cs):_spi(SPI),_cs(cs)
{
   // init();
}
 
KX022_SPI::KX022_SPI(SPIClass &spi,int cs):_spi(spi),_cs(cs)
{
    //init();
} 
 
KX022_SPI::~KX022_SPI()
{
}
 
int KX022_SPI::init()
{
	int retVal;
	pinMode(_cs,OUTPUT);
	digitalWrite(_cs,HIGH);

    _spi.begin();
	
	retVal=getByte(KX022_Accel_WHO_AM_I);
	if (retVal!=0x14)
	{
		return -1;// Did not find device
	}
	else
	{
		writeTwoBytes(KX022_Accel_CNTL1_1,KX022_Accel_CNTL1_2);
		writeTwoBytes(KX022_Accel_ODCNTL_1,KX022_Accel_ODCNTL_2);
		writeTwoBytes(KX022_Accel_CNTL3_1,KX022_Accel_CNTL3_2);
		writeTwoBytes(KX022_Accel_TILT_TIMER_1,KX022_Accel_TILT_TIMER_2);
		writeTwoBytes(KX022_Accel_CNTL2_1,KX022_Accel_CNTL2_2);
		return 0;
	}
}

void KX022_SPI::writeTwoBytes (int address, int data)
{
	digitalWrite(_cs,LOW);
	_buf[0]=address;
	_buf[1]=data;
	_spi.transfer(_buf,2);
	digitalWrite(_cs,HIGH);
}

int KX022_SPI::getByte (int address)
{
	digitalWrite(_cs,LOW);	
	_buf[0]=address | 0x80;// Or-ed with "1" for read bit
	_buf[1]=0;// many not need to do this
	_spi.transfer(_buf,2);

	return _buf[1];
}

float KX022_SPI::getAccel(int channelNum)
{
  return ((int16_t)((getByte(DATA_OUT_BASE+1 + 2*channelNum)<<8) | (getByte(DATA_OUT_BASE + 2*channelNum)))) ;/// 16384.0;  
}
