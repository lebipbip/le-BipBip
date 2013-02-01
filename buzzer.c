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
#include "charge_pump.h"
#include "button.h"
#include "sensor.h"
#include "filter.h"

static int CurrentLen = 0;
static int CurrentFreq = 0;
static int NextLen= 0;
static int NextFreq;


const char * stringLift = "SMB:d=4,o=5,b=100:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,16p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16c7,16p,16c7,16c7,p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16d#6,8p,16d6,8p,16c6";
const char * stringSink = "SW:d=4,o=3,b=110:4g,g,g,8d#,16p,16a#,g,8d#,16p,16a#,2g,d4,d4,d4,8d#4,16p,16a#,f#,8d#,16p,16a#,2g,g4,8g,16p,16g,g4,8f#4,16p,16f4,16e4,16d#4,e4,8g#,c#4,8c4,16p,16b,16a#,16a,a#,8d#,f#,8d#,16p,16a#,4g,8d#,16p,16a#,2g";
int notefreq[12] = {4186, 4434, 4698, 4978, 5274, 5587, 5919, 6271, 6644, 7040, 7458, 7902};

extern SensorValues sensor;

void CheckBuzzerStart(void)
{
	VarioInitialiseFilter(&sensor);
}

void CheckBuzzer(bool mode)
{
	int32_t Vz = 0;
	SensorStartTemperatureSampling();
	sensor.temperature = SensorCompensateTemperature(sensor.temperature_ut);
	sensor.temperature_ut = SensorReadTemperatureWhenAvailable();
	SensorStartPressureSampling();
	sensor.pressure_ut = SensorReadPressureWhenAvailable();
	sensor.pressure = SensorCompensatePressure(sensor.pressure_ut);
	Vz = Filter(sensor.pressure);
	if((Vz < (-10000))&&mode == false)
	{
		play(stringSink);
		VarioInitialiseFilter(&sensor);
	}
	else if((Vz > 10000)&&mode == true)
	{
		play(stringLift);
		VarioInitialiseFilter(&sensor);
	}
}

int note2freq(int note)
{
  return notefreq[note % 12] / (1 << (9 - (note / 12)));
}

void play( const char *song)
{
  char *p = (char *)song;
  int defdur = 4;
  int defscale = 6;
  int bpm = 63;

  // Skip name
  while (*p && *p != ':') p++;
  if (!*p) return;
  p++;

  // Parse defaults
  while (*p)
  {
    char param;
    int value;

    while (*p == ' ') p++;
    if (!*p) return;
    if (*p == ':') break;

    param = *p++;
    if (*p != '=') return;

    p++;
    value = 0;
    while (*p >= '0' && *p <= '9') value = value * 10 + (*p++ - '0');

    switch (param)
    {
      case 'd': defdur = 32 / value; break;
      case 'o': defscale = value; break;
      case 'b': bpm = value; break;
    }

    while (*p == ' ') p++;
    if (*p == ',') p++;
  }
  p++;

  while (*p)
  {
    int note = -1;
    int scale = defscale;
    int dur = defdur;
    int ms;
    int freq;

    // Skip whitespace
    while (*p == ' ') p++;
    if (!*p) return;

    // Parse duration
    if (*p >= '0' && *p <= '9')
    {
      int value = 0;
      while (*p >= '0' && *p <= '9') value = value * 10 + (*p++ - '0');

      dur = 32 / value;
    }

    // Parse note
    switch (*p)
    {
      case 0: return;
      case 'C': case 'c': note = 0; break;
      case 'D': case 'd': note = 2; break;
      case 'E': case 'e': note = 4; break;
      case 'F': case 'f': note = 5; break;
      case 'G': case 'g': note = 7; break;
      case 'A': case 'a': note = 9; break;
      case 'H': case 'h': note = 11; break;
      case 'B': case 'b': note = 11; break;
      case 'P': case 'p': note = -1; break;
    }
    p++;
    if (*p == '#')
    {
      note++;
      p++;
    }
    if (*p == 'b')
    {
      note--;
      p++;
    }

    // Parse special duration
    if (*p == '.')
    {
      dur += dur / 2;
      p++;
    }

    // Parse scale
    if (*p >= '0' && *p <= '9') scale = (*p++ - '0');

    // Parse special duration (again...)
    if (*p == '.')
    {
      dur += dur / 2;
      p++;
    }

    // Skip delimiter
    while (*p == ' ') p++;
    if (*p == ',') p++;

    // Play note
    ms = dur * 60000 / (bpm * 8);
    if (note == -1)
      freq = 0;
    else
      freq = note2freq((scale + 1) * 12 + note);

    if (freq)
    {
      BuzzerSetFrequency(freq);
      delay_ms(ms * 7 / 8);
      BuzzerSetFrequency(0);
      delay_ms(ms / 8);
    }
    else
    	delay_ms(ms);
  }
  BuzzerSetFrequency(0);
}


void BuzzerResetSound()
{
	BuzzerStartSound();
	BuzzerSetBlocking(0, 100);
	BuzzerSetBlocking(600, 120);
	BuzzerSetBlocking(0, 30);
	BuzzerSetBlocking(800, 60);

}

void BuzzerDemoSoundTacTacOn()
{
	BuzzerSetBlocking(0,200);
	BuzzerSetBlocking(20,200);
	BuzzerSetBlocking(0,200);
	BuzzerSetBlocking(30,150);
	BuzzerSetBlocking(0,200);
	BuzzerSetBlocking(40,100);
}

void BuzzerDemoSoundTacTacOff()
{
	BuzzerSetBlocking(0,200);
	BuzzerSetBlocking(40,100);
	BuzzerSetBlocking(0,200);
	BuzzerSetBlocking(30,150);
	BuzzerSetBlocking(0,200);
	BuzzerSetBlocking(20,200);
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

	if (BuzzerSoundMode > 3)
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
			ChargePumpSetPower(POWER_VOLUME_LOW);
			BuzzerDemoSound();
			break;
		case 2:
			ChargePumpSetPower(POWER_VOLUME_MED);
			BuzzerDemoSound();
			break;
		case 3:
			ChargePumpSetPower(POWER_VOLUME_HIG);
			BuzzerDemoSound();
			break;
		default:
			BuzzerSoundMode = 1;
	}
}

#define BUZZER_DEAD_TIME_CYCLE	2

void BuzzerSetFrequency(unsigned int freq)
{
	if (freq)
	{
		int ccr0 = (CPU_FREQUENCY/16)/(freq);
		int ccr0_2 = ccr0/2;
		TimerTA1Set(ccr0, ccr0_2-BUZZER_DEAD_TIME_CYCLE, ccr0-ccr0_2+BUZZER_DEAD_TIME_CYCLE );
	}
	else
	{
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
	if (CurrentFreq != freq)					//avoid constant tone glitch
		BuzzerSetFrequency(freq);				//update only if requiered
	CurrentLen = ms;
	NextLen = 0;
}

void BuzzerSetQueue(unsigned int freq, unsigned int ms)
{
	NextFreq = freq;
	NextLen = ms;
}
