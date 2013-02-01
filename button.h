/*
 * button.h
 *
 *  Created on: 1 sept. 2011
 *      Author: timothee
 */

#ifndef BUTTON_H_
#define BUTTON_H_


#define BUTTON_LONG_TIMEOUT_MS	1000
#define BUTTON_LONG_TIMEOUT		BUTTON_LONG_TIMEOUT_MS/TASK_PERIOD_MS

enum BUTTONSTATES
{
	BUTTON_NONE,
	BUTTON_SHORT,
	BUTTON_LONG,
	BUTTON_DOUBLE,
	BUTTON_PRESSED,
};



uint8_t ButtonState();
void ButtonInit();
uint8_t Button();
void ButtonEnableInterrupt();
void ButtonDisableInterrupt();

#endif /* BUTTON_H_ */
