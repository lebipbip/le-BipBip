/*
 * MS5607.h
 *
 *  Created on: 21 janv. 2012
 *      Author: timothee
 */

#ifndef MS5607_H_
#define MS5607_H_

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
#define 		SENSOR_TEMPERATURE_OSR			CONV_C2_OSR_4096
#define 		SENSOR_PRESSURE_OSR				CONV_C1_OSR_4096

void MS5607Start(void);
void MS5607Stop(void);
void MS5607SensorStartTemperatureSampling(void);
uint32_t MS5607ReadTemperatureWhenAvailable(void);
void MS5607StartPressureSampling (void);
uint32_t MS5607ReadPressureWhenAvailable(void);
int32_t MS5607CompensateTemperature(uint32_t D2);
uint32_t MS5607CompensatePressure(uint32_t D1);




#endif /* MS5607_H_ */
