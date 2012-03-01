/*
 * button.c
 *
 *  Created on: 1 sept. 2011
 *      Author: timothee
 */
#include <msp430.h>
#include <legacymsp430.h>
#include <stdint.h>
#include "target.h"
#include "power.h"
#include "bithacks.h"
#include "button.h"
#include "oscillator.h"
#include "buzzer.h"
#include "sink.h"
#include "uart.h"
#include <stdio.h>

void ButtonInit()
{
	B_UNSET(BUTTON_PORT_DIR, BUTTON_PIN_BIT);		// set as input
	B_UNSET( BUTTON_PORT_IFG, BUTTON_PIN_BIT);		// IFG cleared
	B_SET( BUTTON_PORT_IES, BUTTON_PIN_BIT);		// high-to-low transition
	__enable_interrupt();
}

void ButtonEnableInterrupt()
{
	B_UNSET(BUTTON_PORT_IFG, BUTTON_PIN_BIT);		// clear IFG
	B_SET(BUTTON_PORT_IE, BUTTON_PIN_BIT);			// interrupt enabled
}

void ButtonDisableInterrupt()
{
	B_UNSET(BUTTON_PORT_IE, BUTTON_PIN_BIT);			// interrupt enabled
}


uint8_t ButtonState()
{
	return !B_IS_SET(BUTTON_PORT_IN, BUTTON_PIN_BIT);
}

#define BUTTON_DOUBLE_TIMEOUT	(400/TASK_PERIOD_MS)



uint8_t Button()
{
	uint8_t button_state = BUTTON_NONE;

	static uint8_t button_last_state = 0;
	static uint8_t button_tempo = 0;
	static int16_t ButtonDoubleTimeout = 0;
	static bool		ButtonDoubleOk = false;

	if(!B_IS_SET(BUTTON_PORT_IN, BUTTON_PIN_BIT))
	{

		if((ButtonDoubleTimeout <= 0)||(ButtonDoubleTimeout == BUTTON_DOUBLE_TIMEOUT))
		{
			ButtonDoubleTimeout = BUTTON_DOUBLE_TIMEOUT;
		}
		else
		{
			ButtonDoubleOk = true;
		}
		button_tempo++;
		if (button_tempo >= BUTTON_LONG_TEMPO)
		{
			button_state = BUTTON_LONG;
			ButtonDoubleTimeout = -1;
			button_last_state = button_state;
		}
		else
			button_state = BUTTON_PRESSED;


	}
	else if (ButtonDoubleOk)
	{
		ButtonDoubleOk = false;
		ButtonDoubleTimeout = -1;
		button_state = BUTTON_DOUBLE;
	}
	else
	{
		if (ButtonDoubleTimeout >= 0 )
		{
			ButtonDoubleTimeout--;
		}
		if (ButtonDoubleTimeout == 0)
		{
			button_state = BUTTON_SHORT;
			ButtonDoubleTimeout = -1;
		}
		button_tempo = 0;
	}

#ifdef DEBUG_BUTTON
	char printf_buff[30];
	char printf_len = 0;
	if(button_state == BUTTON_SHORT)
		printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "BUTTON_SHORT\n\r");
	else if(button_state == BUTTON_DOUBLE)
		printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "BUTTON_DOUBLE\n\r");
	else if(button_state == BUTTON_LONG)
		printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "BUTTON_LONG\n\r");
	if(button_state != BUTTON_NONE )
		UartXmitString(printf_buff);
#endif //DEBUG_BUTTON
	return button_state;
}


interrupt(BUTTON_PORT_VECTOR) Button_ISR(void)
{
	if (B_IS_SET(BUTTON_PORT_IFG, BUTTON_PIN_BIT))		// Button pressed interrupt
	{
		B_UNSET(BUTTON_PORT_IFG, BUTTON_PIN_BIT);		// clear IFG
		delay_ms(75);
		BuzzerSetBlocking(600,120);
		int timeout = 2000;
		while(ButtonState() && timeout--)
		{
			delay_ms(1);
		}
		if ( !ButtonState() ) 			// button release, regular startup
		{
			ButtonDisableInterrupt();
			PowerExitUltraLowPower();		// resume system main loop
		}
		else// timeout = enter sink setting
		{
			SinkSetting();
			// return to low power
			ButtonDisableInterrupt();
			ButtonEnableInterrupt();

		}

	}
}
