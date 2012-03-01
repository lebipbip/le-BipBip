/*
 * spi.h
 *
 *  Created on: 9 févr. 2012
 *      Author: timothee
 */

#ifndef SPI_H_
#define SPI_H_
#include <stdint.h>

void SPIStart(void);
void SPIStop(void);
uint8_t SPIReadWriteByte(uint8_t value);
uint16_t SPIReadInt( uint8_t addr );
uint32_t SPIRead24( uint8_t addr );

#endif /* SPI_H_ */

