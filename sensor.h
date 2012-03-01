/*
 * sensor.h
 *
 *  Created on: 17 sept. 2011
 *      Author: timothee
 */

#ifndef SENSOR_H_
#define SENSOR_H_

#include "target.h"
#include "ms5607.h"

#define SensorStart 								MS5607Start
#define SensorCompensatePressure					MS5607CompensatePressure
#define SensorCompensateTemperature					MS5607CompensateTemperature
#define SensorStop 									MS5607Stop
#define SensorStartPressureSampling					MS5607StartPressureSampling
#define SensorReadPressureWhenAvailable				MS5607ReadPressureWhenAvailable
#define SensorStartTemperatureSampling				MS5607SensorStartTemperatureSampling
#define SensorReadTemperatureWhenAvailable			MS5607ReadTemperatureWhenAvailable

#endif /* SENSOR_H_ */


