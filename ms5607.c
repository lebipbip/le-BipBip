/*
 * MS5607.c
 *
 *  Created on: 21 janv. 2012
 *      Author: timothee
 */

#include <stdint.h>
#include <stdio.h>
#include <msp430.h>
#include <legacymsp430.h>
#include "target.h"
#include "bithacks.h"
#include "power.h"
#include "spi.h"
#include "uart.h"
#include "ms5607.h"


enum
{
	RESET = 0x1E,

	CONV_C1_OSR_256 = 0x40,
	CONV_C1_OSR_512 = 0x42,
	CONV_C1_OSR_1024 = 0x4,
	CONV_C1_OSR_2048 = 0x46,
	CONV_C1_OSR_4096 = 0x48,

	CONV_C2_OSR_256 = 0x50,
	CONV_C2_OSR_512 = 0x52,
	CONV_C2_OSR_1024 = 0x54,
	CONV_C2_OSR_2048 = 0x56,
	CONV_C2_OSR_4096 = 0x58,

	ADC_READ = 0,

	READ_C1 = 0xA2,
	READ_C2 = 0xA4,
	READ_C3 = 0xA6,
	READ_C4 = 0xA8,
	READ_C5 = 0xAA,
	READ_C6 = 0xAC,
	READ_CRC = 0xAE

};

typedef struct
{
	uint16_t C1;  // Pressure sensitivity  SENS
	uint16_t C2;  // Pressure offset  OFF
	uint16_t C3;  // Temperature coefficient of pressure sensitivity  TCS
	uint16_t C4;  // Temperature coefficient of pressure offset  TCO
	uint16_t C5;  // Reference temperature
	uint16_t C6;  // Temperature coefficient of the temperature

	int32_t dT;   // Difference between actual and reference temperature
}MS5607_t;

MS5607_t MS5607;

#define MS5607_I2C_ADDR	0xEC>>1

#define MS5607_CS_ENABLE	B_UNSET(MS5607_CS_OUT, MS5607_CS_BIT)
#define MS5607_CS_DISABLE	B_SET(	MS5607_CS_OUT, MS5607_CS_BIT)


void MS5607WaitEOC(void)
{
	while(!B_IS_SET(MS5607_EOC_IN, MS5607_EOC_PIN))	// PIN low: conversion not over
	{
		__disable_interrupt();
		B_UNSET(MS5607_EOC_IFG, MS5607_EOC_PIN);
		B_SET(MS5607_EOC_IE, MS5607_EOC_PIN);
		__enable_interrupt();
		PowerEnterLowPower();
		__disable_interrupt();
		B_UNSET(MS5607_EOC_IE, MS5607_EOC_PIN);
		__enable_interrupt();
	}
}

		
void MS5607Stop(void)
{
	SPIStop();
	MS5607_CS_DISABLE;
}

interrupt(SENSOR_PORT_VECTOR) Sensor_ISR(void)
{
	if (B_IS_SET(MS5607_EOC_IFG, MS5607_EOC_PIN))	//End Of Conversion Interrupt
	{
		B_UNSET(MS5607_EOC_IFG, MS5607_EOC_PIN); 	// clear IFG
		PowerExitLowPower();
	}
}

void MS5607Start(void)
{

	B_UNSET(MS5607_EOC_DIR, MS5607_EOC_PIN);
	B_UNSET(MS5607_EOC_SEL, MS5607_EOC_PIN);
	B_UNSET(MS5607_EOC_SEL2, MS5607_EOC_PIN);
	B_UNSET( MS5607_EOC_IES, MS5607_EOC_PIN);		// rising edge


	B_UNSET(MS5607_CS_SEL, 	MS5607_CS_BIT);
	B_UNSET(MS5607_CS_SEL2,	MS5607_CS_BIT);
	B_SET(MS5607_CS_DIR, 	MS5607_CS_BIT);
	MS5607_CS_DISABLE;
	
	SPIStart();

	MS5607_CS_ENABLE;
	SPIReadWriteByte(RESET);
	MS5607_CS_DISABLE;
	delay_ms(3);


	MS5607_CS_ENABLE;	
	MS5607.C1 = SPIReadInt( READ_C1 );
	MS5607_CS_DISABLE;
	__delay_cycles(1);

	MS5607_CS_ENABLE;
	MS5607.C2 = SPIReadInt( READ_C2 );
	MS5607_CS_DISABLE;
	__delay_cycles(1);

	MS5607_CS_ENABLE;
	MS5607.C3 = SPIReadInt( READ_C3 );
	MS5607_CS_DISABLE;
	__delay_cycles(1);

	MS5607_CS_ENABLE;
	MS5607.C4 = SPIReadInt( READ_C4 );
	MS5607_CS_DISABLE;
	__delay_cycles(1);

	MS5607_CS_ENABLE;
	MS5607.C5 = SPIReadInt( READ_C5 );
	MS5607_CS_DISABLE;
	__delay_cycles(1);

	MS5607_CS_ENABLE;
	MS5607.C6 = SPIReadInt( READ_C6 );
	MS5607_CS_DISABLE;
	
	
	/*
	#ifdef DEBUG
	char printf_buff[140];
	char printf_len = 0;
	printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len,
			"MS5607.C1: %u\n\r"
			"MS5607.C2: %u\n\r"
			"MS5607.C3: %u\n\r"
			"MS5607.C4: %u\n\r"
			"MS5607.C5: %u\n\r"
			"MS5607.C6: %u\n\r"
			, MS5607.C1
			, MS5607.C2
			, MS5607.C3
			, MS5607.C4
			, MS5607.C5
			, MS5607.C6
			);
	UartXmitString(printf_buff);
	#endif // DEBUG

	while(1)
	{
		MS5607SensorStartTemperatureSampling();
		uint32_t temperature_un = MS5607ReadTemperatureWhenAvailable();
		MS5607StartPressureSampling();
		uint32_t pressure_un = MS5607ReadPressureWhenAvailable();

		int32_t temperature = MS5607CompensateTemperature(temperature_un);
		uint32_t pressure = MS5607CompensatePressure(pressure_un);


		printf_len = 0;
		printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "%lu,", pressure_un);
		printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "%lu,", temperature_un);
		printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "%lu,", pressure);
		printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "%ld\r\n", temperature);
		UartXmitString(printf_buff);
	}
	*/
}



void MS5607SensorStartTemperatureSampling(void)
{
	MS5607_CS_DISABLE;
	__delay_cycles(1);
	MS5607_CS_ENABLE;
	SPIReadWriteByte( CONV_C2_OSR_4096 );
}
uint32_t MS5607ReadTemperatureWhenAvailable(void)
{
	uint32_t D2;
	MS5607WaitEOC();
	MS5607_CS_DISABLE;
	__delay_cycles(1);
	MS5607_CS_ENABLE;
	D2 = SPIRead24(ADC_READ);
	MS5607_CS_DISABLE;
	return D2;
}

void MS5607StartPressureSampling (void)
{
	MS5607_CS_DISABLE;
	__delay_cycles(1);
	MS5607_CS_ENABLE;
	SPIReadWriteByte( CONV_C1_OSR_4096 );
}

uint32_t MS5607ReadPressureWhenAvailable(void)
{
	uint32_t D1;
	MS5607WaitEOC();
	MS5607_CS_DISABLE;
	__delay_cycles(1);
	MS5607_CS_ENABLE;
	D1 = SPIRead24(ADC_READ);
	MS5607_CS_DISABLE;
	return D1;
}


int32_t MS5607CompensateTemperature(uint32_t D2)	 // Compensate temperature
{
	// Difference between actual and reference temperature
	MS5607.dT = (int32_t)D2 - ((int32_t)MS5607.C5 << 8);
	// Actual temperature (-40 unsigned long long 85°C with 0.01°C resolution)
	int32_t TEMP = 2000 + ((MS5607.dT * (int64_t)MS5607.C6) >> 23);
	return TEMP;
}

uint32_t MS5607CompensatePressure(uint32_t D1)	 // Compensate pressure
{
	// Offset at actual temperature
	int64_t OFF = ((int64_t)MS5607.C2 << 17) + (((int64_t)MS5607.C4 * MS5607.dT) >> 6);
	// Sensitivity at actual temperature
	int64_t SENS = ((int64_t)MS5607.C1 << 16) + (((int64_t)MS5607.C3 * MS5607.dT) >> 7);
	// Temperature compensated pressure (10 to 1200mbar with 0.01mbar resolution)
	uint32_t P = (((D1 * SENS) >> 21) - OFF) >> 15;
	return  P;
}
