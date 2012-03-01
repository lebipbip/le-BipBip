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

#define SINK_MENU_LOOP_MS        		20        //ms
#define SINK_MENU_TIMEOUT        		30*1000/SINK_MENU_LOOP_MS

#define SINK_ALARM_PLAY_DELAY_MS		1500 //ms
#define SINK_ALARM_PLAY_DELAY 			SINK_ALARM_PLAY_DELAY_MS/SINK_MENU_LOOP_MS
#define SINK_SETTING_PLAY_DELAY			5
#define SINK_SOUND_FREQ					200
#define SINK_SOUND_DISABLE_FREQ			100
#define SINK_SOUND_DISABLE_LEN			1000
#define SINK_SOUND_BIP_ON_LEN			200
#define SINK_SOUND_BIP_OFF_LEN			200
#define SINK_SETTING_PLAY_PAUSE			(SINK_SOUND_BIP_ON_LEN+SINK_SOUND_BIP_OFF_LEN)/SINK_MENU_LOOP_MS


char SinkAlarmValue = SINK_ALARM_DEFAULT; // x-0.5m/s (5 = -2.5m/s)
static char SinkSettingPlayState = 0;
static int SinkSettingPlayDelay;
static char SinkSettingPlayLoop = 0;
static char SinkSettingPlayPause = SINK_SETTING_PLAY_PAUSE;



#undef DEBUG

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
				SinkSettingPlayLoop = 	SinkAlarmValue;
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
					SinkSettingPlayPause = SINK_SETTING_PLAY_PAUSE*10;
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

	SinkAlarmValue++;
	if ( SinkAlarmValue > SINK_ALARM_MAX)
		SinkAlarmValue = SINK_ALARM_MIN;
	SinkSettingPlayState = SINK_STATE_IDDLE;
	SinkSettingPlayDelay = SINK_SETTING_PLAY_DELAY;
}

void SinkSetting(void)
{
	uint8_t button_state;
	int timeout = SINK_MENU_TIMEOUT;

	SinkSettingEnterSound();
	while(ButtonState())		// wait for button to be released
		delay_ms(1);
	SinkSettingPlayDelay = SINK_SETTING_PLAY_DELAY;
	SinkSettingPlayState = SINK_STATE_IDDLE;
	do
	{
		delay_ms(SINK_MENU_LOOP_MS);
		button_state = Button();
		if (button_state == BUTTON_SHORT)
		{
			timeout = SINK_MENU_TIMEOUT;
			SinkSettingButtonPushed();
		}
		SinkSettingPlay();
		BuzzerTask();
	}while( (button_state != BUTTON_LONG) && timeout-- );
	SinkSettingExitSound(); 	// exit sink settings

}
