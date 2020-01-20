/*
 * RF24HALChibios.h
 *
 *  Created on: 26 лют. 2018 р.
 *      Author: Bogdan
 */

#ifndef NRF24_RF24HALCHIBIOS_CONFIG_H_
#define NRF24_RF24HALCHIBIOS_CONFIG_H_

#include <hal.h>

#define RFM7x_USE_SPI SPID1
#define RFM7x_USE_SPI_CNF				\
  SPI_CR_DORD_MSB_FIRST     |   /* SPI Data order.                */ \
  SPI_CR_CPOL_CPHA_MODE(0)  |   /* SPI clock polarity and phase.  */ \
  SPI_CR_SCK_FOSC_128,          /* SPI clock.                     */ \
  SPI_SR_SCK_FOSC_2             /* SPI double speed bit.          */ 

#define RFM7x_CSN_PORT IOPORT2
#define RFM7x_CSN_PAD (2)
#define RFM7x_CE_PORT IOPORT2
#define RFM7x_CE_PAD (1)

#define RFM7x_CSN_LOW palClearPad(RFM7x_CSN_PORT, RFM7x_CSN_PAD)
#define RFM7x_CSN_HI  palSetPad(RFM7x_CSN_PORT, RFM7x_CSN_PAD)

#define RFM7x_CE_LOW  palClearPad(RFM7x_CE_PORT, RFM7x_CE_PAD)
#define RFM7x_CE_HI   palSetPad(RFM7x_CE_PORT, RFM7x_CE_PAD)

#endif /* NRF24_RF24HALCHIBIOS_CONFIG_H_ */
