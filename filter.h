/*
 * filter.h
 *
 *  Created on: 23 sept. 2011
 *      Author: timothee
 */

#ifndef FILTER_H_
#define FILTER_H_

int32_t LP_Filter(int32_t x);
int32_t Filter(uint32_t x);
void FilterReset(void);

typedef struct
{
    int32_t temperature_ut;
	int32_t temperature;
	int32_t pressure_ut;
	int32_t pressure;
}SensorValues;

extern SensorValues sensor;
void VarioInitialiseFilter(SensorValues * sensor);

#endif /* FILTER_H_ */
