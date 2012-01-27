/*
 * timer.h
 *
 *  Created on: 10 oct. 2011
 *      Author: timothee
 */

#ifndef TIMER_H_
#define TIMER_H_


void TimerTA0Start();
void TimerTA1Start();
void TimerTA0Set(int ccr0, int ccr1, int ccr2);
void TimerTA1Set(int ccr0, int ccr1, int ccr2);
void TimerTA1Stop();
void TimerTA0Stop();


#endif /* TIMER_H_ */
