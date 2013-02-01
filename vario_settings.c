/*
 * vario_settings.h
 *
 *  Created on: 7 june 2012
 *      Author: timothee
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "vario_settings.h"
#include "buzzer.h"
#include "button.h"
#include "oscillator.h"
#include "uart.h"
#include "target.h"




enum VarioStates
{
	VARIO_STATE_IDDLE,
	VARIO_STATE_PLAY_START,
	VARIO_STATE_PLAYING
};


#define VARIO_SETTING_MODE_TO_VALUE  10

static uint8_t VarioSettingMode = VARIO_SETTING_DEFAULT; // x+0.1m/s (3 = +0.3m/s)
static uint8_t VarioSettingValue = VARIO_SETTING_MODE_TO_VALUE*VARIO_SETTING_DEFAULT+14;

static uint8_t VarioSettingPlayState = 0;
static uint16_t VarioSettingPlayDelay;
static uint8_t VarioSettingPlayLoop = 0;
static uint16_t VarioSettingPlayPause = VARIO_SETTING_PLAY_PAUSE;

void VarioSettingMenu(void)
{
	uint8_t button_state, button_prev_state;
	uint16_t timeout = VARIO_SETTING_MENU_TIMEOUT;

	VarioSettingEnterSound();
	while(ButtonState())		// wait for button to be released
		delay_ms(1);
	CheckBuzzerStart();

	VarioSettingPlayDelay = VARIO_SETTING_PLAY_DELAY/3;
	VarioSettingPlayState = VARIO_STATE_IDDLE;
	button_prev_state = ButtonState();
	do
	{
		CheckBuzzer(true);
		button_state = ButtonState();
		if ((!button_state)&&(button_prev_state))
		{
			timeout = VARIO_SETTING_MENU_TIMEOUT;
			VarioSettingButtonPushed();
		}
		button_prev_state = button_state;
		VarioSettingPlay();
		BuzzerTask();
	}while( (Button() != BUTTON_LONG) && timeout-- );
	VarioSettingExitSound(); 	// exit Vario settings

}

void VarioSettingEnterSound(void)
{
	BuzzerSetBlocking(600,120);
	BuzzerSetBlocking(0,30);
	BuzzerSetBlocking(800,60);
	BuzzerSetBlocking(0,200);
	BuzzerSetBlocking(600,120);
	BuzzerSetBlocking(0,30);
	BuzzerSetBlocking(800,60);
	BuzzerSetBlocking(0,200);
}
void VarioSettingExitSound(void)
{
	BuzzerSetBlocking(600,120);
	BuzzerSetBlocking(0,30);
	BuzzerSetBlocking(800,60);
	BuzzerSetBlocking(0,200);
	BuzzerSetBlocking(600,120);
	BuzzerSetBlocking(0,30);
	BuzzerSetBlocking(800,60);
	BuzzerSetBlocking(0,200);
	BuzzerStopSound();
}



uint8_t VarioSettingGetValue()
{
	return VarioSettingValue;
}

void VarioSettingSetValue(uint8_t value)
{
	VarioSettingValue = value*10+14;
}


#undef DEBUG

void VarioSettingPlay(void)
{
#ifdef DEBUG
	char printf_buff[32];
	printf_buff[0] = 0;
	char printf_len = 0;
#endif // DEBUG
	switch(VarioSettingPlayState)
	{
		case VARIO_STATE_IDDLE:
			#ifdef DEBUG
			printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "VARIO_STATE_IDDLE\n\r");
			UartXmitString(printf_buff);
			#endif // DEBUG
			if (VarioSettingPlayDelay-- == 0)
			{
				VarioSettingPlayLoop = 	VarioSettingMode;
				VarioSettingPlayState = VARIO_STATE_PLAY_START;
				VarioSettingPlayDelay = VARIO_SETTING_PLAY_DELAY;

			}
		break;
		case VARIO_STATE_PLAY_START:
				#ifdef DEBUG
				printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "VARIO_STATE_PLAY_START\n\r");
				UartXmitString(printf_buff);
				#endif // DEBUG
				if (VarioSettingPlayLoop--)
				{
					VarioSettingPlayPause = VARIO_SETTING_PLAY_PAUSE;
					VarioSettingPlayState = VARIO_STATE_PLAYING;
					BuzzerSetQueue(VARIO_SETTING_SOUND_FREQ, VARIO_SETTING_SOUND_BIP_ON_LEN);
				}
				else
				{
					VarioSettingPlayDelay = VARIO_SETTING_PLAY_DELAY;
					VarioSettingPlayState = VARIO_STATE_IDDLE;
				}
		break;
		case VARIO_STATE_PLAYING:
#ifdef DEBUG
			printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "VARIO_STATE_PLAYING\n\r");
			UartXmitString(printf_buff);
#endif // DEBUG
			if (!VarioSettingPlayPause--)
			{
				VarioSettingPlayState = VARIO_STATE_PLAY_START;
			}
		break;
	}
}

void VarioSettingButtonPushed(void)
{
	VarioSettingMode++;
	if ( VarioSettingMode > VARIO_SETTING_MAX)
	{
		VarioSettingMode = VARIO_SETTING_MIN;
	}
	VarioSettingSetValue(VarioSettingMode);
	VarioSettingPlayState = VARIO_STATE_IDDLE;
	VarioSettingPlayDelay = VARIO_SETTING_PLAY_DELAY/3;
}


