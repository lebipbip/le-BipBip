/*
 * booster.h
 *
 *  Created on: 8 oct. 2011
 *      Author: timothee
 */

#ifndef BOOSTER_H_
#define BOOSTER_H_

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

#endif /* BOOSTER_H_ */
