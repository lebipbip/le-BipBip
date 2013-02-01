/*
 * sensor.h
 *
 *  Created on: 17 sept. 2011
 *      Author: timothee
 */

#ifndef SENSOR_H_
#define SENSOR_H_

#include "target.h"



#ifdef SENSOR_MS5607
	#include "ms5607.h"
	#define SensorStart 								MS5607Start
	#define SensorCompensatePressure					MS5607CompensatePressure
	#define SensorCompensateTemperature					MS5607CompensateTemperature
	#define SensorStop 									MS5607Stop
	#define SensorStartPressureSampling					MS5607StartPressureSampling
	#define SensorReadPressureWhenAvailable				MS5607ReadPressureWhenAvailable
	#define SensorStartTemperatureSampling				MS5607SensorStartTemperatureSampling
	#define SensorReadTemperatureWhenAvailable			MS5607ReadTemperatureWhenAvailable
#endif //SENSOR_BMP085


#ifdef SENSOR_SIMULATED
	#define SensorStart 								SensorSimuStart
	#define SensorCompensatePressure					SensorSimuCompensatePressure
	#define SensorCompensateTemperature					SensorSimuCompensateTemperature
	#define SensorStop 									SensorSimuStop
	#define SensorStartPressureSampling					SensorSimuStartPressureSampling
	#define SensorReadPressureWhenAvailable				SensorReadPressureWhenAvailable
	#define SensorStartTemperatureSampling				SensorStartTemperatureSampling
	#define SensorReadTemperatureWhenAvailable			SensorReadTemperatureWhenAvailable
	#include <stdint.h>
	void SensorSimuStart(void);
	void SensorSimuStop(void);
	void SensorStartTemperatureSampling(void);
	uint32_t SensorReadTemperatureWhenAvailable(void);
	void SensorSimuStartPressureSampling (void);
	uint32_t SensorReadPressureWhenAvailable(void);
	int32_t SensorSimuCompensateTemperature(uint32_t D2);
	uint32_t SensorSimuCompensatePressure(uint32_t D1);
#endif //SENSOR_SIMULATED


#endif /* SENSOR_H_ */


