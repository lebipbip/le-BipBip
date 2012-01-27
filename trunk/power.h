/*
 * power.h
 *
 *  Created on: 2 sept. 2011
 *      Author: timothee
 */

#ifndef POWER_H_
#define POWER_H_

#include <stdbool.h>

void BatteryCheckInit(void);
bool BatteryIsLow(void);
void BatteryCheck(void);
void BatteryCheckStop(void);
unsigned char AutoPowerOffCheck(void);
void AutoPowerOffReset(void);

#define PowerEnterUltraLowPower() (__bis_SR_register(LPM4_bits + GIE))       // Enter LPM4 w/ interrupts enabled
#define PowerExitUltraLowPower() (_BIC_SR_IRQ(LPM4_bits))                 	// Clear LPM4 bits from 0(SR)

#define PowerEnterLowPower() (__bis_SR_register(LPM0_bits + GIE))       // Enter LPM0 (Timer and PWM running) w/ interrupts enabled
#define PowerExitLowPower() (_BIC_SR_IRQ(LPM0_bits))                 	// Clear LPM0 bits from 0(SR)


#endif /* POWER_H_ */
