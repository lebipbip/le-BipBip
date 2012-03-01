/*
//                                
//                               
//               MS5607                      MSP430G2553
//               sensor                     microcontroller
//             -----------------            -----------------
//            |              	|---MISO-->|P1.6/            |->Serial Tx (3.3V)
//            |                 |<--CLK----|                 |
//            |                 |          |                 |<-Serial Rx (3.3V)
//            |              	|<--MOSI-+-|P1.7             |
//            |                 | |-INT--->|                 |
//            |                 |          |                 |      VCC
//            |                 |          |                 |       |
//            |                 |          |                 |      47K
//            |                 |          |             P1.3|-------|
//            |                 |          |                 |     Button
//             -----------------            -----------------        |
                                                                    GND
*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <msp430.h>
#include <legacymsp430.h>
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
#include "charge_pump.h"
#include "target.h"
#include "sink.h"

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

typedef struct
{
    int32_t temperature_ut;
	int32_t temperature;
	int32_t pressure_ut;
	int32_t pressure;
}SensorValues;

void VarioInitialiseFilter(SensorValues * sensor)
{
	int32_t filter_output;
	int16_t init_filter = 400;
	SensorStartTemperatureSampling();
	sensor->temperature_ut =  SensorReadTemperatureWhenAvailable();
	sensor->temperature = SensorCompensateTemperature(sensor->temperature_ut);
	SensorStartPressureSampling();
	sensor->pressure_ut = SensorReadPressureWhenAvailable();
	sensor->pressure = SensorCompensatePressure(sensor->pressure_ut);
	filter_output = Filter(sensor->pressure);
	while (init_filter--)
	{
		filter_output = Filter(sensor->pressure);
	}
}

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

    SensorValues sensor;

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
	ChargePumpStart();
	BuzzerResetSound();
	ChargePumpDemo();
	//SinkSetting();
	SensorStart();
	VarioInitialiseFilter(&sensor);
	while(1)
	{
		#ifndef SENSOR_SIMULATED
		SensorStartTemperatureSampling();
		filter_output = Filter(sensor.pressure);
		sensor.temperature = SensorCompensateTemperature(sensor.temperature_ut);
		sensor.pressure = SensorCompensatePressure(sensor.pressure_ut);

		sensor.temperature_ut = SensorReadTemperatureWhenAvailable();

		SensorStartPressureSampling();

		#else
		sensor.pressure = DataSampleElevator[index];
		if ((index++) == sizeof(DataSampleElevator)/sizeof(DataSampleElevator[0]))
		{
			index = 0;
			BuzzerSetBlocking(3000,500);
		}
		sleep_ms(20);
		#endif //SIMULTATED_DATA

		#ifndef DISABLE_VARIO
		if (Vario(filter_output))
		{
			AutoPowerOffReset();
		}

		#endif // DISABLE_VARIO
		SerialPressureOutput(sensor.pressure, filter_output);

		sensor.pressure_ut = SensorReadPressureWhenAvailable();
		BatteryCheck();
		button_state = Button();
		// BUTTON, Low Battery and Auto-Off mangement
		BuzzerSetVolume(button_state);
		if (button_state!=BUTTON_NONE)
			VarioDisableFor(1500);
		if(button_state == BUTTON_SHORT)
		{
			AutoPowerOffReset();
		}
		else if(button_state == BUTTON_DOUBLE)
		{
			VarioToggleStationaryMode();
		}
		else if((button_state == BUTTON_LONG)||BatteryIsLow()||AutoPowerOffCheck())
		{

			BuzzerStopSound();
			ChargePumpStop();
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
			ChargePumpStart();
			BuzzerStartSound();
			VarioInitialiseFilter(&sensor);
			AutoPowerOffReset();
		}
		BuzzerTask();
	}
}


