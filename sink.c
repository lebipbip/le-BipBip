/*
 * sink.c
 *
 *  Created on: 19 nov. 2011
 *      Author: timothee
 */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "sink.h"
#include "buzzer.h"
#include "button.h"
#include "oscillator.h"
#include "uart.h"
#include "target.h"






enum SinkStates
{
	SINK_STATE_IDDLE,
	SINK_STATE_PLAY_START,
	SINK_STATE_PLAYING
};

void SinkSettingEnterSound(void)
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
void SinkSettingExitSound(void)
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




static uint8_t SinkAlarmState = SINK_ALARM_DEFAULT; // x-0.5m/s (5 = -2.5m/s)
static uint8_t SinkSettingPlayState = 0;
static uint16_t SinkSettingPlayDelay;
static uint8_t SinkSettingPlayLoop = 0;
static uint16_t SinkSettingPlayPause = SINK_SETTING_PLAY_PAUSE;
#define SINK_STATE_TO_VALUE_COEF		-50
static int16_t SinkAlarmValue = SINK_ALARM_DEFAULT*SINK_STATE_TO_VALUE_COEF;
#define SINK_VALUE_DISABLE		SINK_ALARM_DISABLE*SINK_STATE_TO_VALUE_COEF

int16_t SinkGetAlarmValue()
{
	if (SinkAlarmValue != SINK_VALUE_DISABLE)
		return SinkAlarmValue;
	else
		return 0;
}

void SinkSetAlarmState(uint8_t state)
{
	SinkAlarmValue = (int16_t)SinkAlarmState*SINK_STATE_TO_VALUE_COEF;
}


void SinkSettingPlay(void)
{
#ifdef DEBUG
	char printf_buff[32];
	printf_buff[0] = 0;
	char printf_len = 0;
#endif // DEBUG
	switch(SinkSettingPlayState)
	{
		case SINK_STATE_IDDLE:
			#ifdef DEBUG
			printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "SINK_STATE_IDDLE\n\r");
			UartXmitString(printf_buff);
			#endif // DEBUG
			if (SinkSettingPlayDelay-- == 0)
			{
				SinkSettingPlayLoop = 	SinkAlarmState;
				SinkSettingPlayState = 	SINK_STATE_PLAY_START;
				SinkSettingPlayDelay = SINK_SETTING_PLAY_DELAY;

			}
		break;
		case SINK_STATE_PLAY_START:
				#ifdef DEBUG
				printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "SINK_STATE_PLAY_START\n\r");
				UartXmitString(printf_buff);
				#endif // DEBUG
				if ( SinkSettingPlayLoop == SINK_ALARM_DISABLE)	// sink alarm disabled, play specific sound
				{
					BuzzerSetQueue(SINK_SOUND_DISABLE_FREQ, SINK_SOUND_DISABLE_LEN);
					SinkSettingPlayPause = SINK_ALARM_PLAY_DELAY+SINK_SOUND_DISABLE_LEN/SINK_MENU_LOOP_MS;
					SinkSettingPlayState = SINK_STATE_PLAYING;

				}
				else if (SinkSettingPlayLoop--)
				{
					SinkSettingPlayPause = SINK_SETTING_PLAY_PAUSE;
					SinkSettingPlayState = SINK_STATE_PLAYING;
					BuzzerSetQueue(SINK_SOUND_FREQ, SINK_SOUND_BIP_ON_LEN);
				}
				else
				{
					SinkSettingPlayDelay = SINK_ALARM_PLAY_DELAY;
					SinkSettingPlayState = SINK_STATE_IDDLE;
				}
		break;
		case SINK_STATE_PLAYING:
#ifdef DEBUG
			printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "SINK_STATE_PLAYING\n\r");
			UartXmitString(printf_buff);
#endif // DEBUG
			if (!SinkSettingPlayPause--)
			{
				SinkSettingPlayState = SINK_STATE_PLAY_START;
			}
		break;
	}
}

void SinkSettingButtonPushed(void)
{

	SinkAlarmState++;
	if ( SinkAlarmState > SINK_ALARM_MAX)
		SinkAlarmState = SINK_ALARM_MIN;
	SinkSetAlarmState(SinkAlarmState);
	SinkSettingPlayState = SINK_STATE_IDDLE;
	SinkSettingPlayDelay = SINK_SETTING_PLAY_DELAY;
}

void SinkSetting(void)
{
	bool button_state, button_prev_state;
	uint16_t timeout = SINK_MENU_TIMEOUT;

	SinkSettingEnterSound();
	while(ButtonState())		// wait for button to be released
		delay_ms(1);
	CheckBuzzerStart();
	SinkSettingPlayDelay = SINK_SETTING_PLAY_DELAY;
	SinkSettingPlayState = SINK_STATE_IDDLE;
	button_prev_state = ButtonState();
	do
	{
		CheckBuzzer(false);
		button_state = ButtonState();
		if ((!button_state)&&(button_prev_state))
		{
			timeout = SINK_MENU_TIMEOUT;
			SinkSettingButtonPushed();
		}
		button_prev_state = button_state;
		SinkSettingPlay();
		BuzzerTask();
	}while( (Button() != BUTTON_LONG) && timeout-- );
	SinkSettingExitSound(); 	// exit sink settings

}
