/*
 * bluetooth.c
 *
 *  Created on: 13 janv. 2012
 *      Author: timothee
 */

/* bluetooth.c */
#include <msp430.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "bluetooth.h"
#include "bithacks.h"
#include "oscillator.h"
#include "uart.h"
#include "target.h"
#define BT_AT_BAUDRATE				384000

#define BT_RESET_TIME_MS			10
#define BT_START_TIME_MS			100

#define BT_EOC        				"\r"
#define BT_BREAK      			 	"\03"
#define	CHECK_CHAR_BEFORE_FAIL		32
#define BT_RESPONSE_TIMEOUT_MS 		10
#define MAX_CMD_LEN					64		// command string in stack for command TX

void BTSetATMode()
{
	BT_POWER_DISABLE;
	BT_AT_MODE_ENABLE;
	delay_ms(BT_RESET_TIME_MS);
	BT_POWER_ENABLE;
	delay_ms(BT_START_TIME_MS);
}

void BTSetDataMode()
{
	BT_POWER_DISABLE;
	BT_AT_MODE_DISABLE;
	delay_ms(BT_RESET_TIME_MS);
	BT_POWER_ENABLE;
	delay_ms(BT_START_TIME_MS);
}

bool BTCheckRestponse(char * resp)
{
	bool ret;
	/*char cmd[MAX_CMD_LEN];
	char uart_rx;
	uart_rx_char = UartGetChar();
	if(!strncmp( &uart_rx, resp))
		ret =  true;
	else
		ret =  false;

	UART_SEND(BT_BREAK);*/
	ret =  false;
	return ret;
}

bool BTCheckRestponseOk(void)
{
	return (BTCheckRestponse("OK"));
}

bool BTIsPresent()
{
	UART_SEND(BT_EOC);
	UART_SEND("AT");
	UART_SEND(BT_EOC);
	return BTCheckRestponseOk();
}

bool BTSetDataBaudrate(unsigned long baudrate)
{
	char cmd[MAX_CMD_LEN];
	UART_SEND(BT_EOC);
	snprintf( cmd, MAX_CMD_LEN, "AT+UART=38400,0,0");
	UART_SEND(cmd);
	UART_SEND(BT_EOC);
	return true;
}

bool BTSetCMode(char mode)
{
	char cmd[MAX_CMD_LEN];
	int cmd_len;
	UART_SEND(BT_EOC);
	cmd_len = snprintf( cmd, MAX_CMD_LEN, "AT+CMODE=%d", mode);
	UART_SEND(cmd);
	UART_SEND(BT_EOC);
	return true;
}
bool BTSetRole(char mode)
{
	char cmd[MAX_CMD_LEN];
	int cmd_len;
	UART_SEND(BT_EOC);
	cmd_len = snprintf( cmd, MAX_CMD_LEN, "AT+ROLE=%d", mode);
	UART_SEND(cmd);
	UART_SEND(BT_EOC);
	return true;
}

void BTStop()
{
#ifdef BLUETOOTH_ENABLE
	BT_POWER_DISABLE;
	BT_AT_MODE_ENABLE;
#else // BLUETOOTH_DISABLE
	BT_POWER_DISABLE;
	BT_AT_MODE_DISABLE;
#endif //BLUETOOTH_ENABLE
}

void BTStart()
{
#ifdef BLUETOOTH_ENABLE
	BTSetDataMode();
#else // BLUETOOTH_DISABLE
	BT_POWER_DISABLE;
	BT_AT_MODE_DISABLE;
#endif //BLUETOOTH_ENABLE

}
bool BTSetName(char * name)
{
	char cmd[MAX_CMD_LEN];
	int cmd_len;
	UART_SEND(BT_EOC);
	cmd_len = snprintf( cmd, MAX_CMD_LEN, "AT+NAME=%s", name);
	UART_SEND(cmd);
	UART_SEND(BT_EOC);
	return true;
}

bool BTConfigureAndSaveFlash(unsigned long baudrate, char * devicename, char role)	// factory parameter, do it only once: saved in BT's module flash
{
#ifdef BLUETOOTH_ENABLE
	BTSetATMode();
	delay_ms(100);
	BTIsPresent();
	delay_ms(100);
	BTSetDataBaudrate(baudrate);
	delay_ms(100);
	BTSetName(devicename);
	delay_ms(100);
	BTSetCMode(1);
	delay_ms(100);
	BTSetRole(role);
	delay_ms(100);
	BTIsPresent();
	BTStart();
	delay_ms(100);
#else // BLUETOOTH_DISABLE


#endif //BLUETOOTH_ENABLE
	return true;

}


