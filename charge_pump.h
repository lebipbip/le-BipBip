/*
 * booster.h
 *
 *  Created on: 8 oct. 2011
 *      Author: timothee
 */

#ifndef CHARGE_PUMP_H
#define CHARGE_PUMP_H

enum
{
	POWER_VOLUME_LOW,
	POWER_VOLUME_MED,
	POWER_VOLUME_HIG,
};

void ChargePumpSetPower(int power);
void ChargePumpStart();
void ChargePumpStop();

void ChargePumpDemo(void);

#endif /* CHARGE_PUMP_H */
