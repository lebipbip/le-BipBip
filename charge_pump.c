/*
 * charge_pump.c
 * Dickson capacitive charge pump with 4 branches elevata voltage: Vbat+4*Vcc = 3.6+3.3*4 = 16.8V
 * 100 KHz switching frequency with schottcky diode and 100nF capacitor (2nF for the last)
 * Frequency should be higher than 10KHz and lower than 1MHz as dead time are needed (break before make switching type)
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


static uint8_t BoosterPowerValue = POWER_VOLUME_HIG;

void ChargePumpSetPower(int power)
{
	BoosterPowerValue = power;
	ChargePumpStart();
}

#define CHARGE_PUMP_CYCLE_ON		2
#define CHARGE_PUMP_PERIOD			5

void ChargePumpStart()
{
	switch(BoosterPowerValue)
	{
		case POWER_VOLUME_LOW:
			TimerTA0Set(CHARGE_PUMP_PERIOD, 0, 0);
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
	BuzzerSetFrequency( 1000 );
	while( 1 )
	{
		BuzzerSetBlocking( freq, 10 );
		freq +=3;
	}
#endif //CHARGE_PUMP_DEMO
}

