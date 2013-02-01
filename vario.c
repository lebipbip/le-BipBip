/*
 * vario.c
 *
 *  Created on: 17 sept. 2011
 *      Author: timothee
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "vario.h"
#include "buzzer.h"
#include "sink.h"
#include "target.h"
#include "vario_LUT.h"
#include "vario_settings.h"
#include "filter.h"

#define VARIO_POSITIVE_UPPER_LIMIT 			(sizeof(BeepFreqLUT)/(sizeof(BeepFreqLUT[0]))*VARIO_LUT_STEP)-1
#define VARIO_POSITIVE_LOWER_LIMIT  		VarioSettingGetValue() // cm.s-1


#define VARIO_STATIONARY_UPPER_LIMIT  		VARIO_POSITIVE_LOWER_LIMIT
		//beeps within theses limits for stationary
#define VARIO_STATIONARY_LOWER_LIMIT  		-10

#define VARIO_STATIONARY_LOWER_RANGE  		VARIO_STATIONARY_UPPER_LIMIT + abs(VARIO_STATIONARY_LOWER_LIMIT)
#define VARIO_STATIONARY_FREQ_COEF			1
#define VARIO_STATIONARY_BASE_FREQ 			10
#define VARIO_STATIONARY_BASE_LEN 			40
#define VARIO_STATIONARY_UNDERSAMPLE_COEF	4


#define VARIO_NEGATIVE_BASE_FREQ 			200
#define VARIO_NEGATIVE_FREQ_COEF			(VARIO_SINK_LUT_MAX_ITEM-VARIO_NEGATIVE_UPPER_LIMIT)


#define VARIO_AUTO_OFF_UP_DOWN_LIMIT		120

static int32_t Vario_UnderSample = 0;
static bool Vario_Mode_Stationary = false;




void VarioToggleStationaryMode()
{
	if (Vario_Mode_Stationary)
	{
		UartXmitString("Detector OFF\n\r");
		BuzzerDemoSoundTacTacOff();
		Vario_Mode_Stationary = false;
	}
	else
	{
		UartXmitString("Detector ON\n\r");
		BuzzerDemoSoundTacTacOn();
		Vario_Mode_Stationary = true;
	}
}

void VarioDisableFor(int32_t time_ms)
{
	Vario_UnderSample = time_ms;
}
static uint16_t Vario_Delay_Stationary = 0;
#define VARIO_LUT_LIFTY_OFFSET	30
bool Vario(int32_t Vz)
{
	if (Vario_UnderSample < TASK_PERIOD_MS )
	{
		// POSITIVE: LIFT
		if(Vz >= VARIO_POSITIVE_LOWER_LIMIT)
		{

			if(Vz >= VARIO_POSITIVE_UPPER_LIMIT)
			{
				Vz = VARIO_POSITIVE_UPPER_LIMIT;			// caps positive value
			}
			uint16_t StepLUT = Vz/VARIO_LUT_STEP;
			Vario_UnderSample = RepeatTimeLUT[StepLUT];
			BuzzerSetQueue( BeepFreqLUT[StepLUT], BeepLenLUT[StepLUT]);
			Vario_Delay_Stationary = 200;
		}
		// ZERO : STATIONARY
		else if((Vz >= VARIO_STATIONARY_LOWER_LIMIT)&&(Vz < VARIO_STATIONARY_UPPER_LIMIT))
		{
			if (Vario_Delay_Stationary > TASK_PERIOD_MS)
				Vario_Delay_Stationary -= TASK_PERIOD_MS;
			else if(Vario_Mode_Stationary)
			{
				Vario_UnderSample = (-5*Vz)+200;
				BuzzerSetQueue( (140+Vz)/2, 40);
			}
		}
		// NEGATIVE: SINK
		else if(Vz <= SinkGetAlarmValue())
		{

			if(SinkGetAlarmValue() != 0)
			{
				if( Vz < -1600)
					Vz = -1600;
				uint32_t freq = (364*Vz)/2048+359;
				BuzzerSetNow(  freq, 200);
				Vario_UnderSample = 150;
			}
		}
	}
	else
	{
		Vario_UnderSample-=TASK_PERIOD_MS;
	}

	if( abs(Vz) >= VARIO_AUTO_OFF_UP_DOWN_LIMIT )
		return 1;
	else
		return 0; // no pressure change, return 0 for auto-power-off

}




