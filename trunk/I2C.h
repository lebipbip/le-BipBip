/*
 * I2C.h
 *
 *  Created on: 21 janv. 2012
 *      Author: timothee
 */

#ifndef I2C_H_
#define I2C_H_
#include <stdint.h>
int16_t I2CReadInt(uint8_t device_addr, uint8_t reg_address);
void I2CReadBuffer(uint8_t device_addr, uint8_t * buffer, uint8_t len);
void I2CWriteByte(uint8_t device_addr, uint8_t byte);
void I2CWriteBuffer(uint8_t device_addr, uint8_t * buffer, uint8_t len);
unsigned long long I2CRead32(uint8_t device_addr);

#endif /* I2C_H_ */
