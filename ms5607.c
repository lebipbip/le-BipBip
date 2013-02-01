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
#include "oscillator.h"



typedef struct
{
	uint16_t Reserved;
	uint16_t C1;  // Pressure sensitivity  SENS
	uint16_t C2;  // Pressure offset  OFF
	uint16_t C3;  // Temperature coefficient of pressure sensitivity  TCS
	uint16_t C4;  // Temperature coefficient of pressure offset  TCO
	uint16_t C5;  // Reference temperature
	uint16_t C6;  // Temperature coefficient of the temperature
	uint16_t CRC;
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
#ifdef 	SENSOR_EOC_INTERRUPT
		__disable_interrupt();
		B_UNSET(MS5607_EOC_IFG, MS5607_EOC_PIN);
		B_SET(MS5607_EOC_IE, MS5607_EOC_PIN);
		__enable_interrupt();
		PowerEnterLowPower();
		__disable_interrupt();
		B_UNSET(MS5607_EOC_IE, MS5607_EOC_PIN);
		__enable_interrupt();
#else
		__no_operation();
#endif // SENSOR_EOC_INTERRUPT
	}
}

		
void MS5607Stop(void)
{
	SPIStop();
	MS5607_CS_DISABLE;
}
#ifdef 	SENSOR_EOC_INTERRUPT
interrupt(SENSOR_PORT_VECTOR) Sensor_ISR(void)
{
	if (B_IS_SET(MS5607_EOC_IFG, MS5607_EOC_PIN))	//End Of Conversion Interrupt
	{
		B_UNSET(MS5607_EOC_IFG, MS5607_EOC_PIN); 	// clear IFG
		PowerExitLowPower();
	}
}
#endif // SENSOR_EOC_INTERRUPT



unsigned char crc4(unsigned int n_prom[])
{
int cnt;
// simple counter
unsigned int n_rem;
// crc reminder
unsigned int crc_read;
// original value of the crc
unsigned char n_bit;
n_rem = 0x00;
crc_read=n_prom[7];
//save read CRC
n_prom[7]=(0xFF00 & (n_prom[7]));
//CRC byte is replaced by 0
for (cnt = 0; cnt < 16; cnt++)
// operation is performed on bytes
{
// choose LSB or MSB
if (cnt%2==1) n_rem ^= (unsigned short) ((n_prom[cnt>>1]) & 0x00FF);
else n_rem ^= (unsigned short) (n_prom[cnt>>1]>>8);
for (n_bit = 8; n_bit > 0; n_bit--)
{
if (n_rem & (0x8000))
{
n_rem = (n_rem << 1) ^ 0x3000;
}
else
{
n_rem = (n_rem << 1);
}
}
}
n_rem= (0x000F & (n_rem >> 12)); // // final 4-bit reminder is CRC code
n_prom[7]=crc_read;
// restore the crc_read to its original place
return (n_rem ^ 0x00);
}



void MS5607Start(void)
{

	char printf_buff[140];
	char printf_len = 0;


	B_UNSET(MS5607_EOC_DIR, MS5607_EOC_PIN);
	B_UNSET(MS5607_EOC_SEL, MS5607_EOC_PIN);
	B_UNSET(MS5607_EOC_SEL2, MS5607_EOC_PIN);

#ifdef 	SENSOR_EOC_INTERRUPT
	B_UNSET( MS5607_EOC_IES, MS5607_EOC_PIN);		// rising edge
#endif // SENSOR_EOC_INTERRUPT

	B_UNSET(MS5607_CS_SEL, 	MS5607_CS_BIT);
	B_UNSET(MS5607_CS_SEL2,	MS5607_CS_BIT);
	B_SET(MS5607_CS_DIR, 	MS5607_CS_BIT);
	MS5607_CS_DISABLE;
	
	SPIStart();

	MS5607_CS_ENABLE;
	SPIReadWriteByte(RESET);
	MS5607_CS_DISABLE;
	delay_ms(10);


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
	
	MS5607_CS_ENABLE;
	MS5607.CRC = SPIReadInt( READ_CRC );
	MS5607_CS_DISABLE;



	#ifdef SENSOR_DEBUG
	printf_len = snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len,
			"MS5607.C1: %u\n\r"
			"MS5607.C2: %u\n\r"
			"MS5607.C3: %u\n\r"
			"MS5607.C4: %u\n\r"
			"MS5607.C5: %u\n\r"
			"MS5607.C6: %u\n\r"
			"MS5607.CRC: %d\n\r"
			, MS5607.C1
			, MS5607.C2
			, MS5607.C3
			, MS5607.C4
			, MS5607.C5
			, MS5607.C6
			, MS5607.CRC&0x000F
			);
	UartXmitString(printf_buff);


	/*unsigned int n_prom[] = {0x3132,0x3334,0x3536,0x3738,0x3940,0x4142,0x4344,0x450B}
	snprintf(printf_buff, sizeof(printf_buff), "CRC: %d\n\r", n_prom[7]&0x000F);
	UartXmitString(printf_buff);
	snprintf(printf_buff, sizeof(printf_buff), "Check CRC: %d\n\r", crc4(n_prom));
	UartXmitString(printf_buff);


	//MS5607.CRC= (0xFF00 & (MS5607.CRC ));
	unsigned int prom[8];
	prom[0] = MS5607.Reserved;
	prom[1] = MS5607.C1;
	prom[2] = MS5607.C2;
	prom[3] = MS5607.C3;
	prom[4] = MS5607.C4;
	prom[5] = MS5607.C5;
	prom[6] = MS5607.C6;
	prom[7] = MS5607.CRC;
	snprintf(printf_buff, sizeof(printf_buff), "Check CRC: %d\n\r", crc4(prom));
	UartXmitString(printf_buff);
*/
	#endif // SENSOR_DEBUG

	printf_len = snprintf(printf_buff, sizeof(printf_buff), "Sensor ROM (CRC=%d): %s", MS5607.CRC&0x000F,ICT_TAB_STRING);
	UartXmitString(printf_buff);

	if ((MS5607.C1 == 0)
	||(MS5607.C2 == 0)
	||(MS5607.C3 == 0)
	||(MS5607.C4 == 0)
	||(MS5607.C5 == 0)
	||(MS5607.C6 == 0)
	||(MS5607.CRC == 0))
	{
		snprintf(printf_buff, sizeof(printf_buff), "FAIL \n\r");
		UartXmitString(printf_buff);
	}
	else
	{
		snprintf(printf_buff, sizeof(printf_buff), "OK \n\r");
		UartXmitString(printf_buff);
	}


	MS5607SensorStartTemperatureSampling();
	uint32_t temperature_un = MS5607ReadTemperatureWhenAvailable();

	MS5607StartPressureSampling();
	uint32_t pressure_un = MS5607ReadPressureWhenAvailable();

	int32_t temperature = MS5607CompensateTemperature(temperature_un);
	uint32_t pressure = MS5607CompensatePressure(pressure_un);

	printf_len = 0;
	printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "Pressure: ");
	printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "%luPa %s", pressure, ICT_TAB_STRING);
	if (pressure > 1000 && pressure < 120000 )
		printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "OK\n\r");
	else
		printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "FAIL\n\r");
	UartXmitString(printf_buff);

	printf_len = 0;
	printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "Temperature: ");
	printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "%ld.%lu°C %s", temperature/100,(uint32_t)temperature%100, ICT_TAB_STRING);
	if (temperature > -4000 && temperature < 8500 )
		printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "OK\n\r");
	else
		printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "FAIL\n\r");
	UartXmitString(printf_buff);
	/*
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
	SPIReadWriteByte( SENSOR_TEMPERATURE_OSR );
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
	SPIReadWriteByte( SENSOR_PRESSURE_OSR );
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
