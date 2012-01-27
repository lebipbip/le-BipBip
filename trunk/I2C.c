/*
 * I2C.c
 *
 *  Created on: 21 janv. 2012
 *      Author: timothee
 */

#include <stdint.h>
#include <stdio.h>
#include <msp430.h>
#include "target.h"
#include "TI_USCI_I2C_master.h"
#include "uart.h"
#include "I2C.h"

int16_t I2CReadInt(uint8_t device_addr, uint8_t reg_address)
{
	uint8_t data[2];
	I2CWriteByte(device_addr, reg_address);
	I2CReadBuffer(device_addr, data, sizeof(data));
	return (int)( (data[0]<<8) | data[1]);// First byte will be from 'address'
										// Second byte will be from 'address'+1
}
unsigned long long I2CRead32(uint8_t device_addr)
{
	uint8_t buffer[3];
	unsigned long long read = 0;
	TI_USCI_I2C_receiveinit( device_addr, I2C_PRESCALER);
	while ( TI_USCI_I2C_notready() );         // wait for bus to be free
	TI_USCI_I2C_receive( sizeof(buffer), buffer);
	while ( TI_USCI_I2C_notready() );         // wait for bus to be free
	read = ((unsigned long long) buffer[0] << 16) | ((unsigned long long) buffer[1] << 8) | ((unsigned long long) buffer[2]) ;
	return read;
}
void I2CReadBuffer(uint8_t device_addr, uint8_t * buffer, uint8_t len)
{
	TI_USCI_I2C_receiveinit( device_addr, I2C_PRESCALER);
	while ( TI_USCI_I2C_notready() );         // wait for bus to be free
	TI_USCI_I2C_receive( len, buffer);
	while ( TI_USCI_I2C_notready() );         // wait for bus to be free
}
void I2CWriteByte(uint8_t device_addr, uint8_t byte)
{
	TI_USCI_I2C_transmitinit( device_addr, I2C_PRESCALER); 			//write device adress on bus
	while ( TI_USCI_I2C_notready() );         							// wait for bus to be free
	TI_USCI_I2C_transmit( 1, &byte);
	while ( TI_USCI_I2C_notready() );         							// wait for bus to be free
}
void I2CWriteBuffer(uint8_t device_addr, uint8_t * buffer, uint8_t len)
{
	TI_USCI_I2C_transmitinit( device_addr, I2C_PRESCALER); 			//write device adress on bus
	while ( TI_USCI_I2C_notready() );         							// wait for bus to be free
	TI_USCI_I2C_transmit( len, buffer);
	while ( TI_USCI_I2C_notready() );         							// wait for bus to be free
}
