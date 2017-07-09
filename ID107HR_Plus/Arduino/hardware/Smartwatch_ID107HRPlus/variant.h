/*
  Copyright (c) 2014-2015 Arduino LLC.  All right reserved.
  Copyright (c) 2016 Sandeep Mistry All right reserved.
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _VARIANT_GENERIC_
#define _VARIANT_GENERIC_

/** Master clock frequency */
#ifdef NRF52
#define VARIANT_MCK       (64000000ul)
#else
#define VARIANT_MCK       (16000000ul)
#endif

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "WVariant.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

// Number of pins defined in PinDescription array
#define PINS_COUNT           (32u)
#define NUM_DIGITAL_PINS     (32u)
#define NUM_ANALOG_INPUTS    (4u)
#define NUM_ANALOG_OUTPUTS   (0u)

/*
 * Analog pins
 */
#define PIN_A0               (28)
#define PIN_A1               (29)
#define PIN_A2               (30)
#define PIN_A3               (31)

static const uint8_t A0  = PIN_A0 ;
static const uint8_t A1  = PIN_A1 ;
static const uint8_t A2  = PIN_A2 ;
static const uint8_t A3  = PIN_A3 ;
#define ADC_RESOLUTION    14
/*
 * Serial interfaces
 */
// Serial
#define PIN_SERIAL_RX       (15)
#define PIN_SERIAL_TX       (16)

/*
 * SPI Interfaces
 */
#define SPI_INTERFACES_COUNT 2

#define PIN_SPI_MISO         (27u)
#define PIN_SPI_MOSI         (31u)
#define PIN_SPI_SCK          (30u)

static const uint8_t SS   = 5u ;
static const uint8_t MOSI = PIN_SPI_MOSI ;
static const uint8_t MISO = PIN_SPI_MISO ;
static const uint8_t SCK  = PIN_SPI_SCK ;

#define PIN_SPI1_MISO         (4u)
#define PIN_SPI1_MOSI         (3u)
#define PIN_SPI1_SCK          (5u)

static const uint8_t SS1   = 7u ;
static const uint8_t MOSI1 = PIN_SPI1_MOSI ;
static const uint8_t MISO1 = PIN_SPI1_MISO ;
static const uint8_t SCK1  = PIN_SPI1_SCK ; 

/*
 * Periphery
 */
 
#define SPI_OLED			(SPI)
#define PIN_OLED_DC			(22u)
#define PIN_OLED_CS			(19u)
#define PIN_OLED_RESET		(20u)
#define PIN_OLED_MOSI		(31u)
#define PIN_OLED_MISO		(NRF_SPI_PIN_NOT_CONNECTED)
#define PIN_OLED_CLK		(20u)
#define PIN_OLED_PWR		(26u)


#define PIN_FLASH_CS		(28u)
#define FLASH_SIZE			(8388608ul)	//bits

#define SPI_KX022			(SPI1)
#define PIN_KX022_CS		(7u)
#define PIN_KX022_INT		(6u)

/*
 * Wire Interfaces
 */
#define WIRE_INTERFACES_COUNT 1

/*
#define  PIN_SI1143_SCL               15u
#define  PIN_SI1143_SDA               16u
#define  PIN_SI1143_INT                8u
#define  PIN_SI1143_LED                9u

#define PIN_WIRE_SDA         (PIN_SI1143_SDA)
#define PIN_WIRE_SCL         (PIN_SI1143_SCL) 
*/

#define  PIN_S263_SCL               (15u)
#define  PIN_S263_SDA               (16u)
#define  PIN_S263_RDY               (17u)

#define PIN_WIRE_SDA         (PIN_S263_SDA)
#define PIN_WIRE_SCL         (PIN_S263_SCL) 

#define I2C_ADDRESS_S263			(0x44u)

#ifdef __cplusplus
}
#endif

#endif
