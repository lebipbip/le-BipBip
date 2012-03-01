/*
 * sink.h
 *
 *  Created on: 19 nov. 2011
 *      Author: timothee
 */

#ifndef SINK_H_
#define SINK_H_


#define SINK_ALARM_DEFAULT 				5
#define SINK_ALARM_MIN 					3
#define SINK_ALARM_MAX 					8
#define SINK_ALARM_DISABLE 				SINK_ALARM_MAX


extern char SinkAlarmValue;
void SinkSettingEnterSound(void);
void SinkSettingExitSound(void);
void SinkSettingPlay(void);
void SinkSettingButtonPushed(void);
void SinkSetting(void);


#endif /* SINK_H_ */
