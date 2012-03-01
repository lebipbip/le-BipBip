/*
 * MS5607.h
 *
 *  Created on: 21 janv. 2012
 *      Author: timothee
 */

#ifndef MS5607_H_
#define MS5607_H_



void MS5607Start(void);
void MS5607Stop(void);
void MS5607SensorStartTemperatureSampling(void);
uint32_t MS5607ReadTemperatureWhenAvailable(void);
void MS5607StartPressureSampling (void);
uint32_t MS5607ReadPressureWhenAvailable(void);
int32_t MS5607CompensateTemperature(uint32_t D2);
uint32_t MS5607CompensatePressure(uint32_t D1);




#endif /* MS5607_H_ */
