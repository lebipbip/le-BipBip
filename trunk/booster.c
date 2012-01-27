/*
 * booster.c
 *
 *  Created on: 8 oct. 2011
 *      Author: timothee
 */
#include <msp430.h>
#include "booster.h"
#include "buzzer.h"
#include "oscillator.h"
#include "bithacks.h"
#include "booster.h"
#include "timer.h"

static char BoosterPowerValue = POWER_VOLUME_HIG;


void BoosterSetPower(int power)
{
	BoosterPowerValue = power;
}


void BoosterStart()
{
	if (BoosterPowerValue == POWER_VOLUME_HIG)
		TimerTA0Set(8, 7, 0);
	else
		TimerTA0Set(3, 2 , 0);
}

void BoosterStop()
{
	TimerTA0Stop();
}


void booster(void)
{

	BuzzerDemoSound();
	BoosterSetPower(POWER_VOLUME_LOW);
	int freq = 2000;
	BuzzerSetFrequency( freq );
	while(1)
	{
		//delay_ms(100);
		BuzzerSetBlocking( freq, 1000 );
		freq += 100;
		if (freq > 2800 )
			freq = 2000;
	}

}

