/*
 * booster.h
 *
 *  Created on: 8 oct. 2011
 *      Author: timothee
 */

#ifndef BOOSTER_H_
#define BOOSTER_H_

#define POWER_VOLUME_LOW 	1
#define POWER_VOLUME_HIG 	2


void BoosterSetPower(int power);
void BoosterStart();
void BoosterStop();

void booster(void);

#endif /* BOOSTER_H_ */
