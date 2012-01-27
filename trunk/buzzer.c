#include <stdint.h>
#include <stdio.h>
#include <msp430.h>
#include "bithacks.h"
#include <legacymsp430.h>
#include "uart.h"
#include "oscillator.h"
#include "buzzer.h"
#include "target.h"
#include "vario.h"
#include "timer.h"
#include "booster.h"
#include "button.h"
//#define BUZZER_DISABLED

static int CurrentLen = 0;
static int CurrentFreq = 0;
static int NextLen= 0;
static int NextFreq;

void BuzzerResetSound()
{

	BuzzerStartSound();
	BuzzerSetBlocking(0,100);
	BuzzerSetBlocking(600,120);
	BuzzerSetBlocking(0,30);
	BuzzerSetBlocking(800,60);

}

void BuzzerDemoSoundTacTac()
{
	BuzzerSetFrequency(0);
	BuzzerSetBlocking(0,200);
	BuzzerSetBlocking(20,100);
	BuzzerSetBlocking(0,200);
	BuzzerSetBlocking(20,100);
	BuzzerSetBlocking(0,200);
	BuzzerSetBlocking(20,100);
}

void BuzzerDemoSound()
{
	BuzzerSetFrequency(0);
	BuzzerSetBlocking(0,200);
	BuzzerSetBlocking(600,200);
	BuzzerSetBlocking(0,75);
	BuzzerSetBlocking(600,200);
	BuzzerSetBlocking(0,75);
	BuzzerSetBlocking(600,200);
	BuzzerSetQueue(0,1000);
}

void BuzzerStartSound()
{
	BuzzerSetFrequency(0);
	BuzzerSetBlocking(200,150);
	BuzzerSetBlocking(0,50);
	BuzzerSetBlocking(400,120);
	BuzzerSetBlocking(0,40);
	BuzzerSetBlocking(600,100);
	BuzzerSetBlocking(0,30);
	BuzzerSetBlocking(800,60);
}



void BuzzerStopSound()
{
	BuzzerSetFrequency(0);
	BuzzerSetBlocking(600,150);
	BuzzerSetBlocking(0,30);
	BuzzerSetBlocking(400,170);
	BuzzerSetBlocking(0,40);
	BuzzerSetBlocking(200,190);
	BuzzerSetBlocking(0,50);
	BuzzerSetBlocking(150,200);
}


#define DEFAULT_VOLUME_MODE				2
static char BuzzerVolumeValue = 		POWER_VOLUME_HIG;
static char BuzzerSoundMode = 			DEFAULT_VOLUME_MODE;
#define BUZZER_BUTTON_TIMEOUT_MS		4000
#define BUZZER_BUTTON_TIMEOUT			BUZZER_BUTTON_TIMEOUT_MS/TASK_PERIOD_MS

static int BuzzerButtonTemp = 0;

void BuzzerSetVolume(char button)
{
	if (button  == BUTTON_SHORT)
	{
		if (BuzzerButtonTemp == 0)
		{
			BuzzerButtonTemp = BUZZER_BUTTON_TIMEOUT;
		}
		else if (BuzzerButtonTemp > 0)
		{
			BuzzerButtonTemp = BUZZER_BUTTON_TIMEOUT;
			BuzzerSoundMode++;
		}
	}

	if ( BuzzerButtonTemp > 0 )
		BuzzerButtonTemp--;

	if (button != BUTTON_SHORT)
		return;

	if (BuzzerSoundMode > 4)
		BuzzerSoundMode = 1;

#ifdef DEBUG
	char printf_buff[30];
	char printf_len = 0;
	printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "Volume mode %d\n\r", BuzzerSoundMode);
	UartXmitString(printf_buff);
#endif //DEBUG

	switch(BuzzerSoundMode)
	{
		case 1:
			BuzzerVolumeValue = POWER_VOLUME_LOW;
			BoosterSetPower(BuzzerVolumeValue);
			VarioSetStationaryMode(true);
			BuzzerDemoSoundTacTac();
			BuzzerDemoSound();

			break;
		case 2:
			BuzzerVolumeValue = POWER_VOLUME_HIG;
			BoosterSetPower(BuzzerVolumeValue);
			VarioSetStationaryMode(true);
			BuzzerDemoSoundTacTac();
			BuzzerDemoSound();

			break;
		case 3:
			BuzzerVolumeValue = POWER_VOLUME_LOW;
			BoosterSetPower(BuzzerVolumeValue);
			VarioSetStationaryMode(false);
			BuzzerDemoSound();
			break;
		case 4:
			BuzzerVolumeValue = POWER_VOLUME_HIG;
			BoosterSetPower(BuzzerVolumeValue);
			VarioSetStationaryMode(false);
			BuzzerDemoSound();
			break;
	}
}

#define BUZZER_ON_TIME		(ccr0/2)
void BuzzerSetFrequency(unsigned int freq)
{
	if (freq)
	{
		int ccr0 = (CPU_FREQUENCY/4)/(freq);

		TimerTA1Set(ccr0, BUZZER_ON_TIME, ccr0 );
		BoosterStart();
	}
	else
	{
		BoosterStop();
		TimerTA1Stop();
	}
}



void BuzzerTask()
{
	if(CurrentLen>0)
		CurrentLen-=TASK_PERIOD_MS;
	else
	{
		if(NextLen)
		{
			if (CurrentFreq != NextFreq)		//avoid constant tone glitch
				BuzzerSetFrequency(NextFreq);	//update only if requiered
			CurrentLen = NextLen;
			NextLen = 0;
		}
		else
		{
			BuzzerSetFrequency(0);
		}

	}
}


void BuzzerSetBlocking(unsigned int freq, unsigned int ms)
{
	BuzzerSetFrequency(freq);
	delay_ms(ms);
	BuzzerSetFrequency(0);
}

void BuzzerSetNow(unsigned int freq, unsigned int ms)
{
	if (CurrentFreq != freq)		//avoid constant tone glitch
		BuzzerSetFrequency(freq);				//update only if requiered
	CurrentLen = ms;
	NextLen = 0;
}

void BuzzerSetQueue(unsigned int freq, unsigned int ms)
{
	NextFreq = freq;
	NextLen = ms;
}




// Timer A0 interrupt service routine
interrupt(TIMER0_A0_VECTOR) Timer_A0 (void)
{
}

// Timer_A1 Interrupt Vector (TA0IV) handler
interrupt(TIMER0_A1_VECTOR) Timer_A1(void)
{
}
