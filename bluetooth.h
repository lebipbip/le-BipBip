/*
 * bluetooth.h
 *
 *  Created on: 13 janv. 2012
 *      Author: timothee
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#define UART_SEND					UartXmitString
#define UART_GET_CHAR()				{}
#define BT_POWER_COMMAND
//#define BT_RESET_COMMAND

#ifdef BT_POWER_COMMAND
	#define BT_POWER_PIN_BIT     	0
	#define BT_POWER_PIN_DIR        P1DIR
	#define BT_POWER_PIN_OUT     	P1OUT
	#define BT_POWER_ENABLE			B_SET(BT_POWER_PIN_DIR, BT_POWER_PIN_BIT); B_UNSET(BT_POWER_PIN_OUT, BT_POWER_PIN_BIT)
	#define BT_POWER_DISABLE		B_SET(BT_POWER_PIN_DIR, BT_POWER_PIN_BIT); B_SET(BT_POWER_PIN_OUT, BT_POWER_PIN_BIT)
#endif // BT_POWER_COMMAND

#ifdef BT_RESET_COMMAND
	#define BT_RESET_PIN_BIT
	#define BT_RESET_PIN_DIR
	#define BT_RESET_PIN_OUT
	#define BT_RESET_ENABLE
	#define BT_RESET_DISABLE
#endif // BT_RESET_COMMAND

#define BT_AT_MODE_PIN_BIT			4
#define BT_AT_MODE_PIN_DIR        	P3DIR
#define BT_AT_MODE_PIN_OUT        	P3OUT

#define BT_AT_MODE_ENABLE			B_SET(BT_AT_MODE_PIN_DIR, BT_AT_MODE_PIN_BIT); B_SET(BT_AT_MODE_PIN_OUT, BT_AT_MODE_PIN_BIT)
#define BT_AT_MODE_DISABLE			B_SET(BT_AT_MODE_PIN_DIR, BT_AT_MODE_PIN_BIT); B_UNSET(BT_AT_MODE_PIN_OUT, BT_AT_MODE_PIN_BIT)




/* no changes below */
#include <stdbool.h>
enum
{
	BT_Slave_Mode 			= 0,
	BT_Slave_Loopback_Mode 	= 1,
	BT_Master_Mode 			= 2
}e_BTParingMode;


bool BTConfigureAndSaveFlash(unsigned long baudrate, char * devicename, char mode);	// Configure factory parameter, do it only once, as it's saved in flash
void BTStop();
void BTStart();
#endif /* BLUETOOTH_H_ */
