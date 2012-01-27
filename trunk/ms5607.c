/*
 * MS5607.c
 *
 *  Created on: 21 janv. 2012
 *      Author: timothee
 */

#include <stdint.h>
#include <stdio.h>
#include <msp430.h>
#include "target.h"
#include "bithacks.h"
#include "power.h"
#include "I2C.h"
#include "TI_USCI_I2C_master.h"
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




void MS5607Start(void)
{
	char printf_buff[140];
	char printf_len = 0;
	I2CWriteByte(MS5607_I2C_ADDR, RESET);
	delay_ms(100);
	MS5607.C1 = I2CReadInt(MS5607_I2C_ADDR, READ_C1);
	MS5607.C2 = I2CReadInt(MS5607_I2C_ADDR, READ_C2);
	MS5607.C3 = I2CReadInt(MS5607_I2C_ADDR, READ_C3);
	MS5607.C4 = I2CReadInt(MS5607_I2C_ADDR, READ_C4);
	MS5607.C5 = I2CReadInt(MS5607_I2C_ADDR, READ_C5);
	MS5607.C6 = I2CReadInt(MS5607_I2C_ADDR, READ_C6);
	/*
	int crc = I2CReadInt(MS5607_I2C_ADDR, READ_CRC);

	MS5607.C1
	MS5607.C2
	MS5607.C3
	MS5607.C4
	MS5607.C5
	MS5607.C6*/

	#ifdef DEBUG
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
	/*
	while(1)
	{
		uint32_t temperature_un = MS5607_Read_Uncompensated_Temperature();
		uint32_t pressure_un = MS5607_Read_Uncompensated_Pressure();

		//printf_len = 0;
		//printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "%ld", temperature_un);
		//printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, ",%ld\r\n", pressure_un);
		//UartXmitString(printf_buff);

		int32_t temperature = MS5607_Compensate_Temperature(temperature_un);
		uint32_t pressure = MS5607_Compensate_Pressure(pressure_un);


		printf_len = 0;
//		printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "%lu,", pressure_un);
//		printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "%lu,", temperature_un);
		printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "%lu,", pressure);
		printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "%ld\r\n", temperature);
		UartXmitString(printf_buff);
	}
*/

}

void MS5607Stop(void)
{

}

void MS5607SensorStartTemperatureSampling(void)
{
	I2CWriteByte( MS5607_I2C_ADDR, CONV_C2_OSR_4096);
}
uint32_t MS5607ReadTemperatureWhenAvailable(void)
{
	uint32_t D2 = 0;
	delay_ms(8);
	while (D2 == 0)
	{
		I2CWriteByte( MS5607_I2C_ADDR, ADC_READ);
		D2 = I2CRead32(MS5607_I2C_ADDR);
		if (D2 == 0)
		{
			MS5607SensorStartTemperatureSampling();
			delay_ms(8);
		}
	}
	return D2;
}

void MS5607StartPressureSampling (void)
{
	I2CWriteByte(MS5607_I2C_ADDR, CONV_C1_OSR_4096);
}

uint32_t MS5607ReadPressureWhenAvailable(void)
{
	uint32_t D1 = 0;
#ifdef DEBUG
	delay_ms(1);
#else
	delay_ms(8);
#endif // DEBUG
	while (D1 == 0)
	{
		I2CWriteByte( MS5607_I2C_ADDR, ADC_READ);
		D1 = I2CRead32(MS5607_I2C_ADDR);
		if (D1 == 0)
		{
			MS5607StartPressureSampling();
			delay_ms(8);
		}
	}
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
