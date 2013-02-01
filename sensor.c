/*
 * sensor.c
 *
 *  Created on: 31 mars 2012
 *      Author: timothee
 */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <msp430.h>
#include <legacymsp430.h>
#include "target.h"
#include "sensor.h"
#include "data.h"
#include "buzzer.h"
#include "oscillator.h"




#ifdef SENSOR_SIMULATED

void SensorSimuStart(void)
{
}


void SensorSimuStop(void)
{
}



void SensorStartTemperatureSampling(void)
{

}
uint32_t SensorReadTemperatureWhenAvailable(void)
{
	delay_ms(8);
	return 0;
}

void SensorSimuStartPressureSampling (void)
{

}

uint32_t SensorReadPressureWhenAvailable(void)
{
	delay_ms(8);
	return 0;
}


int32_t SensorSimuCompensateTemperature(uint32_t D2)	 // Compensate temperature
{

	return 0;
}

uint32_t SensorSimuCompensatePressure(uint32_t D1)	 // Compensate pressure
{

#ifdef DEMO
	static uint32_t pressure = 990000;
	static uint8_t demoState = 10;
	static uint16_t demoStateTime = 0;
	static bool div2 = false;
	static uint8_t div16 = 0;
	static uint8_t div10 = 0;
	switch (demoState)
	{
		case 10:								// +0m/s
			demoStateTime = (10000)/TASK_PERIOD_MS;
			demoState = 20;
		break;
		case 20:
			if (div2)
				pressure++;
			div2 = !div2;
			demoStateTime--;
				demoState = 31;
		break;
		case 31:								// -0.15m/s
			demoStateTime = (1500)/TASK_PERIOD_MS;
			demoState = 41;
		break;
		case 41:
			if (!div16)
			{
				pressure++;
				div16 = 30;
			}
			else
				div16--;
			demoStateTime--;
			if (!demoStateTime)
				demoState = 30;
		break;
		case 30:								// +0.15m/s
			demoStateTime = (1500)/TASK_PERIOD_MS;
			demoState = 40;
		break;
		case 40:
			if (!div16)
			{
				pressure--;
				div16 = 15;
			}
			else
				div16--;
			demoStateTime--;
			if (!demoStateTime)
				demoState = 50;
		break;
		case 50:								// +0.35m/s
			demoStateTime = (1500)/TASK_PERIOD_MS;
			demoState = 60;
		break;
		case 60:
			if (!div10)
			{
				pressure--;
				div10 = 8;
			}
			else
				div10--;
			demoStateTime--;
			if (!demoStateTime)
				demoState = 70;
		break;
		case 70:								// +1.5m/s
			demoStateTime = (1500)/TASK_PERIOD_MS;
			demoState = 80;
		break;
		case 80:
			if (div2)
				pressure--;
			div2 = !div2;
			demoStateTime--;
			if (!demoStateTime)
				demoState = 90;
		break;
		case 90:						// -6m/s
			demoStateTime = (2000)/TASK_PERIOD_MS;
			demoState = 100;
		break;
		case 100:
			pressure = pressure+2;
			demoStateTime--;
			if (!demoStateTime)
				demoState = 110;
		break;
		case 110:								// +3m/s
			demoStateTime = (2000)/TASK_PERIOD_MS;
			demoState = 120;
		break;
		case 120:
			pressure = pressure-1;
			demoStateTime--;
			if (!demoStateTime)
				demoState = 130;
		break;
		case 130:			 					// +6m/s
			demoStateTime = (1500)/TASK_PERIOD_MS;
			demoState = 140;
		break;
		case 140:
			pressure = pressure-2;
			demoStateTime--;
			if (!demoStateTime)
				demoState = 150;
		break;
		case 150:								// +9m/s
			demoStateTime = (1500)/TASK_PERIOD_MS;
			demoState = 160;
		break;
		case 160:
			pressure = pressure-3;
			demoStateTime--;
			if (!demoStateTime)
				demoState = 10;
		break;
		default:
			demoState = 10;
			break;

	}
	return pressure;

#else
	static int index = 0;
	if ((index++) == sizeof(DataSampleElevator)/sizeof(DataSampleElevator[0])-1)
	{
		index = 0;
		BuzzerSetBlocking(2000,1000);
	}
	return  DataSampleElevator[index];


	#endif //DEMO
}
#endif // SENSOR_SIMULATED
