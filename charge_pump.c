/*
 * charge_pump.c
 *
 *  Created on: 8 oct. 2011
 *      Author: timothee
 */
#include <msp430.h>
#include "charge_pump.h"
#include "buzzer.h"
#include "oscillator.h"
#include "bithacks.h"
#include "timer.h"
#include "target.h"
static char BoosterPowerValue = POWER_VOLUME_HIG;


void ChargePumpSetPower(int power)
{
	BoosterPowerValue = power;
	ChargePumpStart();
}

#define CHARGE_PUMP_CYCLE_ON		4
#define CHARGE_PUMP_PERIOD			10

void ChargePumpStart()
{
	switch(BoosterPowerValue)
	{
		case POWER_VOLUME_LOW:
			ChargePumpStop();
		break;
		case POWER_VOLUME_MED:
			TimerTA0Set(CHARGE_PUMP_PERIOD, 0, CHARGE_PUMP_PERIOD-CHARGE_PUMP_CYCLE_ON);
		break;
		case POWER_VOLUME_HIG:
			TimerTA0Set(CHARGE_PUMP_PERIOD, CHARGE_PUMP_CYCLE_ON, CHARGE_PUMP_PERIOD-CHARGE_PUMP_CYCLE_ON);
		break;
	}
}

void ChargePumpStop()
{
	TimerTA0Stop();
}


void ChargePumpDemo(void)
{
#ifdef CHARGE_PUMP_DEMO
	ChargePumpSetPower(POWER_VOLUME_HIG);
	int freq = 800;
	BuzzerSetBlocking( freq, 4000 );
	while( freq < 1800 )
	{
		BuzzerSetBlocking( freq, 10 );
		freq +=3;
	}
#endif //CHARGE_PUMP_DEMO
}

