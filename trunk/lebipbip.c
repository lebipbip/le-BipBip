/*
//               MS5607           VCC  VCC
//                 or             /|\  /|\
//               BMP085           10k  10k     MSP430G2xx3
//               sensor            |    |      microcontroller
//             -----------------   |    |   -----------------
//            |              SDA|<-|----+->|P1.6/UCB0SDA  XIN|-
//            |                 |  |       |                 |
//            |                 |  |       |             XOUT|-
//            |              SCL|<-+------>|P1.7/UCB0SCL     |
//            |                 |          |                 |
//            |                 |          |                 |
//            |                 |          |                 |
//            |                 |          |                 |
//            |                 |          |                 |
//            |                 |          |                 |
//             -----------------            -----------------
*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <msp430.h>
#include <legacymsp430.h>
#include "TI_USCI_I2C_master.h"
#include "button.h"
#include "uart.h"
#include "buzzer.h"
#include "bithacks.h"
#include "power.h"
#include "sensor.h"
#include "vario.h"
#include "oscillator.h"
#include "filter.h"
#include "data.h"
#include "booster.h"
#include "target.h"
#include "sink.h"
#include "bluetooth.h"

/*
typedef struct
{
  int ON_delay_ms;
  int OFF_delay_ms;
  unsigned char volatile * Led_DIR;
  unsigned char volatile * Led_OUT;
  char Led_PIN;
  int ON_delay_ms_current;
  int OFF_delay_ms_current;
}Led_Blink;

#define MAX_LED	8
static Led_Blink Led_Blink_Array[MAX_LED] =
{ 	//ON_delay_ms, OFF_delay_ms, Led_DIR, Led_OUT, Led_PIN, dummy, dummy
  { 100, 2000, &P1DIR, &P1OUT, 0, 0, 0},
  { 100, 1000, &P1DIR, &P1OUT, 1, 0, 0},
  { 100, 10000, &P1DIR, &P1OUT, 2, 0, 0},
  { 100, 30000, &P1DIR, &P1OUT, 3, 0, 0},
  { 500, 1000, &P1DIR, &P1OUT, 4, 0, 0},
  { 500, 60000, &P1DIR, &P1OUT, 5, 0, 0},
  { 1  , 0   , &P1DIR, &P1OUT, 6, 0, 0},
  { 1  , 0   , &P1DIR, &P1OUT, 7, 0, 0}
};


void Led_Init()
{
	int i ;
  P1SEL = 0;
  P1SEL2 = 0;
	for(i = 0; i < MAX_LED; i ++)
  {
    Led_Blink_Array[i].ON_delay_ms_current = Led_Blink_Array[i].ON_delay_ms;
    Led_Blink_Array[i].OFF_delay_ms_current = Led_Blink_Array[i].OFF_delay_ms;
    B_SET(*(Led_Blink_Array[i].Led_DIR), Led_Blink_Array[i].Led_PIN);
    B_UNSET(*(Led_Blink_Array[i].Led_OUT), Led_Blink_Array[i].Led_PIN);
  }
}

void Led_Process()
{
	int i ;
  for(i = 0; i < MAX_LED; i ++)
  {
    // ON STATE
    if (Led_Blink_Array[i].ON_delay_ms_current)
    {
      Led_Blink_Array[i].ON_delay_ms_current--;
      B_SET(*(Led_Blink_Array[i].Led_OUT), Led_Blink_Array[i].Led_PIN);
    }
    // OFF STATE
    else if (Led_Blink_Array[i].OFF_delay_ms_current)
    {
      Led_Blink_Array[i].OFF_delay_ms_current--;
      B_UNSET(*(Led_Blink_Array[i].Led_OUT), Led_Blink_Array[i].Led_PIN);
    }
    // Reload
    else
    {
      Led_Blink_Array[i].ON_delay_ms_current = Led_Blink_Array[i].ON_delay_ms;
      Led_Blink_Array[i].OFF_delay_ms_current = Led_Blink_Array[i].OFF_delay_ms;
    }
  }
}
*/

void SerialStartOutput(void)
{
	#ifdef DEBUG

	UartXmitString("\r\n\tle BipBip on MSP430G2553 started\r\n");
	#endif //DEBUG
}

void SerialPressureOutput(long pressure, long long int filter_output)
{
	#ifdef DEBUG
	char printf_buff[100];
	char printf_len = 0;

	//printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, " %ld", pressure_ut);
	//printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, " TCo: %d.%d°C\t", (temperature/10), (temperature%10));
	printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "%ld", (long int)pressure);
	printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, ",%ld", (long int)filter_output);
	//printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, " Vz: %d.%02dm/s\t",(vario_diff/100), (vario_diff%100));*/
	printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, " \n\r");
	UartXmitString(printf_buff);
	printf_len = 0;
	#endif //DEBUG
}
#define TEMPERATURE_UNDERSAMPLE	64
int main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop WDT
    OscillatorStart();

/*
    B_UNSET(P1SEL, 0); B_UNSET(P1SEL2, 0); B_SET(P1DIR, 0); B_UNSET(P1OUT, 0);		// BT Command
    B_UNSET(P1SEL, 1); B_UNSET(P1SEL2, 1); B_UNSET(P1DIR, 1); B_UNSET(P1OUT, 1);	// RX pin
    B_UNSET(P1SEL, 2); B_UNSET(P1SEL2, 2); B_UNSET(P1DIR, 2); B_UNSET(P1OUT, 2);	// TX pin
    B_UNSET(P1SEL, 3); B_UNSET(P1SEL2, 3); B_UNSET(P1DIR, 3); B_UNSET(P1OUT, 3);	// unused
    B_UNSET(P1SEL, 4); B_UNSET(P1SEL2, 4); B_UNSET(P1DIR, 4); B_UNSET(P1OUT, 4);	// unused	(I2C SDA fix)
    B_UNSET(P1SEL, 5); B_UNSET(P1SEL2, 5); B_UNSET(P1DIR, 5); B_UNSET(P1OUT, 5);	// unused	(I2C SDA fix)
    B_UNSET(P1SEL, 6); B_UNSET(P1SEL2, 6); B_UNSET(P1DIR, 6); B_UNSET(P1OUT, 6);	// SCL
    B_UNSET(P1SEL, 7); B_UNSET(P1SEL2, 7); B_UNSET(P1DIR, 7); B_UNSET(P1OUT, 7);	// SDA

    B_UNSET(P2SEL, 0); B_UNSET(P2SEL2, 0); B_UNSET(P2DIR, 0); B_UNSET(P2OUT, 0);	// EOC
    B_UNSET(P2SEL, 1); B_UNSET(P2SEL2, 1); B_SET(P2DIR, 1); B_UNSET(P2OUT, 1);		// H bridge
    B_UNSET(P2SEL, 2); B_UNSET(P2SEL2, 2); B_SET(P2DIR, 2); B_SET(P2OUT, 2);		// BT AT
    B_UNSET(P2SEL, 3); B_UNSET(P2SEL2, 3); B_SET(P2DIR, 3); B_UNSET(P2OUT, 3);		// unused
    B_UNSET(P2SEL, 4); B_UNSET(P2SEL2, 4); B_SET(P2DIR, 4); B_UNSET(P2OUT, 4);		// H bridge
    B_UNSET(P2SEL, 5); B_UNSET(P2SEL2, 5); B_UNSET(P2DIR, 5); B_UNSET(P2OUT, 5);	// unused	(I2C PWD fix)
    B_UNSET(P2SEL, 6); B_UNSET(P2SEL2, 6); B_SET(P2DIR, 6); B_UNSET(P2OUT, 6);		// Boost
    B_UNSET(P2SEL, 7); B_UNSET(P2SEL2, 7); B_SET(P2DIR, 7); B_UNSET(P2OUT, 7);		// unused

    //PowerEnterUltraLowPower();
*/


    int32_t temperature_ut = 0;
	int32_t temperature;
	int32_t pressure_ut = 0;
	int32_t pressure = 0;
	int32_t filter_output = 0;
	uint8_t button_state;
	#ifdef SIMULTATED_DATA
	int index = 0;
	#endif //SIMULTATED_DATA

	ButtonInit();
	UartInit();

	BatteryCheckInit();
	BatteryCheck();
	while (BatteryIsLow())
	{
		BatteryCheckStop();
		ButtonEnableInterrupt();
		PowerEnterUltraLowPower();
		ButtonDisableInterrupt();
		BatteryCheckInit();
		BatteryCheck();
	}
	SerialStartOutput();
	booster();
	//SinkSetting();
	//BTConfigureAndSaveFlash( 38400, "le BlueBip", BT_Slave_Mode);
	BuzzerResetSound();
	SensorStart();
/*
	int init_sensor = 10;
	int init_filter = 1000;
	while (init_sensor--)
	{
		SensorStartTemperatureSampling();
		temperature_ut =  SensorReadTemperatureWhenAvailable();
		temperature_ut = LP_Filter(temperature_ut);
		temperature = SensorCompensateTemperature(temperature_ut);
		SensorStartPressureSampling();
		pressure_ut = SensorReadPressureWhenAvailable();
		pressure = SensorCompensatePressure(pressure_ut);
		filter_output = Filter(pressure);
	}
	init_filter = 1000;
	while (init_filter--)
	{
		temperature_ut = LP_Filter(temperature_ut);
		filter_output = Filter(pressure);
	}
	init_sensor = 10;
*/

	while(1)
	{
		//B_SET(P2DIR, 5); 		B_SET(P2OUT, 5);
		// TEMPERATURE
		SensorStartTemperatureSampling();
		// begin of 5 ms sensor sampling time start with SensorRequestTemperatureConversion()
		//B_SET(P2DIR, 3); 		B_SET(P2OUT, 3);
		temperature_ut = LP_Filter(temperature_ut);
		temperature = SensorCompensateTemperature(temperature_ut);
		//B_SET(P2DIR, 3); 		B_UNSET(P2OUT, 3);
		// end of 5 ms sensor sampling time here ends with SensorReadConvertedTemperatureWhenAvailable()
		temperature_ut =  SensorReadTemperatureWhenAvailable();

		// PRESSURE
		SensorStartPressureSampling();
		// begin of 25 ms sensor sampling time here
		//B_SET(P2DIR, 3); 		B_SET(P2OUT, 3);

		pressure = SensorCompensatePressure(pressure_ut);
		#ifdef SIMULTATED_DATA
		pressure = DataSampleElevator[index];
		if ((index++) == sizeof(DataSampleElevator)/sizeof(DataSampleElevator[0]))
		{
			index = 0;
			BuzzerSetBlocking(3000,500);
		}
		#endif //SIMULTATED_DATA
		filter_output = Filter(pressure);
		#ifndef DISABLE_VARIO
		if (Vario(filter_output))
		{
			AutoPowerOffReset();
		}
		BuzzerTask();
		#endif // DISABLE_VARIO
		SerialPressureOutput(pressure, filter_output);
		button_state = Button();
		BuzzerSetVolume(button_state);
		BatteryCheck();


		//B_SET(P2DIR, 3); 		B_UNSET(P2OUT, 3);

		// end of 25 ms sensor sampling time here
		pressure_ut = SensorReadPressureWhenAvailable();
		//B_SET(P2DIR, 5); 		B_UNSET(P2OUT, 5);


		// BUTTON, Low Battery and Auto-Off mangement
		if(button_state == BUTTON_SHORT)
		{
			AutoPowerOffReset();
		}
		else if((button_state == BUTTON_LONG)||BatteryIsLow()||AutoPowerOffCheck())
		{

			BuzzerStopSound();
			SensorStop();
			do
			{
				BatteryCheckStop();
				ButtonEnableInterrupt();

				PowerEnterUltraLowPower();
				// Button pressed here (interrupt wake up)

				ButtonDisableInterrupt();
				BatteryCheckInit();
				BatteryCheck();
			}	while (BatteryIsLow());

			ButtonDisableInterrupt();
			SensorStart();
			BuzzerStartSound();

			/*while (init_sensor--)
			{
				SensorStartTemperatureSampling();
				temperature_ut =  SensorReadTemperatureWhenAvailable();
				temperature_ut = LP_Filter(temperature_ut);
				temperature = SensorCompensateTemperature(temperature_ut);
				SensorStartPressureSampling();
				pressure_ut = SensorReadPressureWhenAvailable();
				pressure = SensorCompensatePressure(pressure_ut);
				filter_output = Filter(pressure);
			}
			init_filter = 1000;
			while (init_filter--)
			{
				temperature_ut = LP_Filter(temperature_ut);
				filter_output = Filter(pressure);
			}
			init_sensor = 10;*/

			AutoPowerOffReset();
		}
	}
}


