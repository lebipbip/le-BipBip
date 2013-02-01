/*
 * sink.h
 *
 *  Created on: 19 nov. 2011
 *      Author: timothee
 */

#ifndef SINK_H_
#define SINK_H_


#define SINK_ENTER_SINK_SETTING_TIMEOUT_MS			2500

#define SINK_ALARM_MIN 					2
#define SINK_ALARM_MAX 					8
#define SINK_ALARM_DISABLE 				SINK_ALARM_MAX
#define SINK_ALARM_DEFAULT 				SINK_ALARM_DISABLE

#define SINK_MENU_LOOP_MS        		TASK_PERIOD_MS        //ms
#define SINK_MENU_TIMEOUT        		30*1000/SINK_MENU_LOOP_MS

#define SINK_ALARM_PLAY_DELAY_MS		1000 //ms
#define SINK_ALARM_PLAY_DELAY 			SINK_ALARM_PLAY_DELAY_MS/SINK_MENU_LOOP_MS
#define SINK_SETTING_PLAY_DELAY			5
#define SINK_SOUND_FREQ					170
#define SINK_SOUND_DISABLE_FREQ			100
#define SINK_SOUND_DISABLE_LEN			750
#define SINK_SOUND_BIP_ON_LEN			250
#define SINK_SOUND_BIP_OFF_LEN			250
#define SINK_SETTING_PLAY_PAUSE			((SINK_SOUND_BIP_ON_LEN+SINK_SOUND_BIP_OFF_LEN)/SINK_MENU_LOOP_MS)


int16_t SinkGetAlarmValue(void);
void SinkSettingEnterSound(void);
void SinkSettingExitSound(void);
void SinkSettingPlay(void);
void SinkSettingButtonPushed(void);
void SinkSetting(void);


#endif /* SINK_H_ */
