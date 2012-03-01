/*
 * vario.c
 *
 *  Created on: 17 sept. 2011
 *      Author: timothee
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "vario.h"
#include "buzzer.h"
#include "sink.h"
#include "target.h"

#define VARIO_POSITIVE_FREQ_6 		2400
#define VARIO_POSITIVE_FREQ_COEF_6	9
#define VARIO_POSITIVE_BPS_COEF_6	6

#define VARIO_POSITIVE_LIMIT_6  	200

#define VARIO_POSITIVE_FREQ_5 		2200
#define VARIO_POSITIVE_FREQ_COEF_5	9
#define VARIO_POSITIVE_BPS_COEF_5	6

#define VARIO_POSITIVE_LIMIT_5  	100

#define VARIO_POSITIVE_FREQ_4 		2000
#define VARIO_POSITIVE_FREQ_COEF_4	9
#define VARIO_POSITIVE_BPS_COEF_4	6

#define VARIO_POSITIVE_LIMIT_4  	50

#define VARIO_POSITIVE_FREQ_3 		1500
#define VARIO_POSITIVE_FREQ_COEF_3	9
#define VARIO_POSITIVE_BPS_COEF_3	6

#define VARIO_POSITIVE_LIMIT_3  	30

#define VARIO_POSITIVE_FREQ_2 		1000
#define VARIO_POSITIVE_FREQ_COEF_2	9
#define VARIO_POSITIVE_BPS_COEF_2	6

#define VARIO_POSITIVE_LIMIT_2  	10

#define VARIO_POSITIVE_FREQ_1 		600
#define VARIO_POSITIVE_FREQ_COEF_1	1
#define VARIO_POSITIVE_BPS_COEF_1	5

#define VARIO_POSITIVE_LIMIT_1  	5


#define VARIO_STATIONARY_UPPER_LIMIT  	4
		//beeps within theses limits for stationary
#define VARIO_STATIONARY_LOWER_LIMIT  	-2

#define VARIO_STATIONARY_LOWER_RANGE  	VARIO_STATIONARY_UPPER_LIMIT + abs(VARIO_STATIONARY_LOWER_LIMIT)
#define VARIO_STATIONARY_FREQ_COEF		1
#define VARIO_STATIONARY_BASE_FREQ 		10
#define VARIO_STATIONARY_BASE_LEN 		40
#define VARIO_STATIONARY_UNDERSAMPLE_COEF	4

#define VARIO_NEGATIVE_UPPER_LIMIT  	-10
		//beeps within theses limits for sink
#define VARIO_NEGATIVE_LOWER_LIMIT  	-50

#define VARIO_NEGATIVE_BASE_FREQ 		120
#define VARIO_NEGATIVE_FREQ_COEF		2


#define VARIO_AUTO_OFF_UP_DOWN_LIMIT	10

static int32_t Vario_UnderSample = 0;
static bool Vario_Mode_Stationary = true;


void VarioToggleStationaryMode()
{
	if (Vario_Mode_Stationary)
	{
		BuzzerDemoSoundTacTacOff();
		Vario_Mode_Stationary = false;
	}
	else
	{
		BuzzerDemoSoundTacTacOn();
		Vario_Mode_Stationary = true;
	}

	BuzzerSetQueue( 0, 100);
}

void VarioDisableFor(int32_t time_ms)
{
	Vario_UnderSample = time_ms;
}


bool Vario(int32_t PressureDerivated)
{
	if (Vario_UnderSample < TASK_PERIOD_MS )
	{
		int vario_tone;
		// POSITIVE: LIFT
		if(PressureDerivated >= VARIO_POSITIVE_LIMIT_1)
		{

			if(PressureDerivated >= VARIO_POSITIVE_LIMIT_6)
			{
				PressureDerivated = VARIO_POSITIVE_LIMIT_6;			// caps positive value
			}
			if(PressureDerivated < VARIO_POSITIVE_LIMIT_2)
			{
				vario_tone = PressureDerivated*VARIO_POSITIVE_FREQ_COEF_1+VARIO_POSITIVE_FREQ_COEF_1;
				BuzzerSetQueue( vario_tone, 100);
				Vario_UnderSample = 600 - PressureDerivated*VARIO_POSITIVE_BPS_COEF_1;
			}
			else if(PressureDerivated < VARIO_POSITIVE_LIMIT_3)
			{
				vario_tone = PressureDerivated*VARIO_POSITIVE_FREQ_COEF_2+VARIO_POSITIVE_FREQ_COEF_2;
				BuzzerSetQueue( vario_tone, 20);
				Vario_UnderSample = 600 - PressureDerivated*VARIO_POSITIVE_BPS_COEF_2;
			}
			else if(PressureDerivated < VARIO_POSITIVE_LIMIT_4)
			{
				vario_tone = PressureDerivated*VARIO_POSITIVE_FREQ_COEF_3+VARIO_POSITIVE_FREQ_COEF_3;
				BuzzerSetQueue( vario_tone, 20);
				Vario_UnderSample = 600 - PressureDerivated*VARIO_POSITIVE_BPS_COEF_3;
			}
			else if(PressureDerivated < VARIO_POSITIVE_LIMIT_5)
			{
				vario_tone = PressureDerivated*VARIO_POSITIVE_FREQ_COEF_4+VARIO_POSITIVE_FREQ_COEF_4;
				BuzzerSetQueue( vario_tone, 20);
				Vario_UnderSample = 600 - PressureDerivated*VARIO_POSITIVE_BPS_COEF_4;
			}
			else if(PressureDerivated < VARIO_POSITIVE_LIMIT_6)
			{
				vario_tone = PressureDerivated*VARIO_POSITIVE_FREQ_COEF_5+VARIO_POSITIVE_FREQ_COEF_5;
				BuzzerSetQueue( vario_tone, 20);
				Vario_UnderSample = 600 - PressureDerivated*VARIO_POSITIVE_BPS_COEF_5;
			}
			else
			{
				vario_tone = PressureDerivated*VARIO_POSITIVE_FREQ_COEF_5+VARIO_POSITIVE_FREQ_COEF_6;
				BuzzerSetQueue( vario_tone, 20);
				Vario_UnderSample = 600 - PressureDerivated*VARIO_POSITIVE_BPS_COEF_6;
			}
			if (Vario_UnderSample > 600)
				Vario_UnderSample = 600;


		}
		// ZERO STATIONARY
		if(Vario_Mode_Stationary)
		{
			if((PressureDerivated >= VARIO_STATIONARY_LOWER_LIMIT)&&(PressureDerivated <= VARIO_STATIONARY_UPPER_LIMIT))
			{
				vario_tone = 20;
				BuzzerSetQueue( vario_tone, VARIO_STATIONARY_BASE_LEN);
				Vario_UnderSample = 400 - PressureDerivated * 100;
				if ((Vario_UnderSample< 100)|| (Vario_UnderSample> 1000))
					Vario_UnderSample= 100;
				//Vario_UnderSample = 1000;
			}
		}
		// NEGATIVE: SINK
		if(SinkAlarmValue != SINK_ALARM_DISABLE)
		{
			int16_t sinkthresvalue = (int16_t)SinkAlarmValue;
			sinkthresvalue = sinkthresvalue*-16;
			if((PressureDerivated <= sinkthresvalue))
			{
				if (PressureDerivated <= VARIO_NEGATIVE_UPPER_LIMIT)	// cap negative value
					PressureDerivated = VARIO_NEGATIVE_UPPER_LIMIT;
				vario_tone = VARIO_NEGATIVE_BASE_FREQ + PressureDerivated*VARIO_NEGATIVE_FREQ_COEF;
				BuzzerSetQueue(  vario_tone, 200);
				Vario_UnderSample = 500;
			}
		}
	}
	else
	{
		Vario_UnderSample-=TASK_PERIOD_MS;
	}

	if( abs(PressureDerivated) >= VARIO_AUTO_OFF_UP_DOWN_LIMIT )
		return 1;
	else
		return 0; // no pressure change, return 0 for auto-power-off

}




