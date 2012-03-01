/*
 * spi.c
 *
 *  Created on: 9 févr. 2012
 *      Author: timothee
 */
#include <stdint.h>
#include <msp430.h>
#include "target.h"
#include "spi.h"
#include "bithacks.h"

#define SPI_MOSI_BIT	7
#define SPI_MOSI_SEL	P1SEL
#define SPI_MOSI_SEL2	P1SEL2

#define SPI_MISO_BIT	6
#define SPI_MISO_IN		P1IN
#define SPI_MISO_DIR	P1DIR
#define SPI_MISO_SEL	P1SEL
#define SPI_MISO_SEL2	P1SEL2

#define SPI_CLK_BIT		5
#define SPI_CLK_SEL		P1SEL
#define SPI_CLK_SEL2	P1SEL2

void SPIStart(void)
{
	B_SET(SPI_MOSI_SEL, SPI_MOSI_BIT);
	B_SET(SPI_MOSI_SEL2, SPI_MOSI_BIT);

	B_SET(SPI_MISO_SEL, SPI_MISO_BIT);
	B_SET(SPI_MISO_SEL2, SPI_MISO_BIT);

	B_SET(SPI_CLK_SEL, SPI_CLK_BIT);
	B_SET(SPI_CLK_SEL2, SPI_CLK_BIT);

	UCB0CTL0 = UCCKPH + UCMSB + UCMST + UCSYNC;  // 3-pin, 8-bit SPI master
	UCB0CTL1 = UCSSEL_2;                     // SMCLK
	UCB0BR0 = 0x01;                          // Fsys
	UCB0BR1 = 0;                              //
	//UCB0MCTL = 0;                             // No modulation
	UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
}

void SPIStop(void)
{

}

uint8_t SPIReadWriteByte(uint8_t value)
{
	uint8_t ret;
	UCB0TXBUF = value;          	// Send value
	while ((UCB0STAT & UCBUSY ));   	// USCI_A0 TX ok
	ret = UCB0RXBUF;
	return ret;
}

uint16_t SPIReadInt( uint8_t addr )
{
	uint16_t value;
	SPIReadWriteByte(addr);
	value = SPIReadWriteByte(0);
	value = value << 8;
	value += SPIReadWriteByte(0);
	return value;
}

uint32_t SPIRead24( uint8_t addr )
{
	uint32_t value = 0;
	SPIReadWriteByte(addr);
	value = SPIReadWriteByte(0);
	value = value << 8;
	value |= SPIReadWriteByte(0);
	value = value << 8;
	value |= SPIReadWriteByte(0);
	return value;
}
