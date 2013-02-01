/*
 * vario_SETTING.h
 *
 *  Created on: 7 june 2012
 *      Author: timothee
 */
#ifndef VARIO_SETTING_H
#define VARIO_SETTING_H

#define VARIO_SETTING_MENU_TIMEOUT_MS		2500

#define VARIO_SETTING_MIN 					1
#define VARIO_SETTING_MAX 					5
#define VARIO_SETTING_DEFAULT 				2

#define VARIO_SETTING_MENU_LOOP_MS        	TASK_PERIOD_MS        //ms
#define VARIO_SETTING_MENU_TIMEOUT        	30*1000/VARIO_SETTING_MENU_LOOP_MS

#define VARIO_SETTING_PLAY_DELAY_MS			1000 //ms
#define VARIO_SETTING_PLAY_DELAY 			VARIO_SETTING_PLAY_DELAY_MS/VARIO_SETTING_MENU_LOOP_MS
#define VARIO_SETTING_SOUND_FREQ			900
#define VARIO_SETTING_SOUND_BIP_ON_LEN		150
#define VARIO_SETTING_SOUND_BIP_OFF_LEN		150
#define VARIO_SETTING_PLAY_PAUSE			((VARIO_SETTING_SOUND_BIP_ON_LEN+VARIO_SETTING_SOUND_BIP_OFF_LEN)/VARIO_SETTING_MENU_LOOP_MS)


uint8_t VarioSettingGetValue(void);
void VarioSettingEnterSound(void);
void VarioSettingExitSound(void);
void VarioSettingPlay(void);
void VarioSettingButtonPushed(void);
void VarioSettingMenu(void);


#endif //VARIO_SETTING_H
