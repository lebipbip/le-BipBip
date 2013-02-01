/*
//
//
//                   VCC(3.3V)         VCC
//                          |                 |
//             -----------------            -----------------  Vbat (3~4V)       Dickson capacitive charge pump
//            |               DO|<---------|P1.6/MOSI        | |                                                    (16V)
//            |               DI|----+---->|    /MISO        | +-|>|-+--|>|--+-|>|---+--|>|--+--|>|--+----------------+
//            |                 |    |---->|     INT         |       |       |       |       |       |                |
//            |              SCL|<---------|P1.7/UCB0SCL     |     100nF   100nF   100nF   100nF    2nF               |
//            |                 |          |                 |       |       |       |       |       |                |
//            |                 |          |    TimerA0      |-------+-------|-------+       |      GND               |
//            |                 |          |                 |---------------+---------------+                        |   H full bridge (BTL)
//            |                 |          |                 |                                                  +-----+------+
//            |    MS5607       |          |    TimerA1      |--------------------------------------------->  PMos          PMos
//            |                 |          |                 |                                                  +---Piezzo---+
//            |                 |          |                 |--------------------------------------------->  NMos          NMos
//            |                 |          | MSP430G2xx3     |                                                  +-----+------+
//            |                 |          |         P1.3    |                                                        |
//             -----------------            ----------+------                                                        GND
//                                                    |
//													BUTTON
*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include <msp430.h>
#include <legacymsp430.h>
#include "bithacks.h"
#include "target.h"
#include "button.h"
#include "uart.h"
#include "buzzer.h"
#include "power.h"
#include "sensor.h"
#include "vario.h"
#include "oscillator.h"
#include "filter.h"
#include "charge_pump.h"
#include "sink.h"
#include "bluetooth.h"
#include "timer.h"
#include "vario_settings.h"




void PlayVersionNumber(void)
{
	char printf_buff[100];
	char printf_len = 0;
	printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "\n\r\tle BipBip version %d.%d \n\r", VERSION_MAJOR, VERSION_MINOR);
	printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "Buzzer test...\n\r");
	UartXmitString(printf_buff);
	uint8_t version = VERSION_MAJOR;
	while(version)
	{
		version--;
		BuzzerSetBlocking(SINK_SOUND_FREQ, SINK_SOUND_BIP_ON_LEN);
		delay_ms(SINK_SOUND_BIP_OFF_LEN);
	}
	delay_ms(SINK_SOUND_BIP_OFF_LEN*2);
	version = VERSION_MINOR;
	while(version)
	{
		version--;
		BuzzerSetBlocking(SINK_SOUND_FREQ, SINK_SOUND_BIP_ON_LEN);
		delay_ms(SINK_SOUND_BIP_OFF_LEN);
	}
	delay_ms(SINK_SOUND_BIP_OFF_LEN*4);
}
void SerialPressureOutput(long pressure, long long int filter_output)
{
	#ifdef NMEA_OUTPUT_ENABLE
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
	#endif //NMEA_OUTPUT_ENABLE
}





void DisableAllGPIO();
SensorValues sensor;
int main(void)
{
	int32_t filter_output = 0;
	uint8_t button_state;
	bool ExitLowPower;
    WDTCTL = WDTPW + WDTHOLD; // Stop WDT
    OscillatorStart();
	//DisableAllGPIO();
	UartInit();
	ButtonInit();
	BatteryCheckInit();
	BatteryCheck();
	ChargePumpStop();
	SensorStop();
	BTStop();
	while (BatteryIsLow())
	{
		BatteryCheckStop();
		ButtonEnableInterrupt();
		PowerEnterUltraLowPower();
		ButtonDisableInterrupt();
		BatteryCheckInit();
		BatteryCheck();
	}
	BTConfigureAndSaveFlash( 38400, "leBlueBip", BT_Slave_Mode);
	ChargePumpStart();
	PlayVersionNumber();
	BatteryPrintVoltage();
	BuzzerStartSound();
	SensorStart();
	VarioInitialiseFilter(&sensor);
	while(1)
	{
		SensorStartTemperatureSampling();													// Temperature sampling request
		filter_output = Filter(sensor.pressure);											//
		BatteryCheck();																		//			8ms available here
		sensor.temperature = SensorCompensateTemperature(sensor.temperature_ut);			//
		sensor.pressure = SensorCompensatePressure(sensor.pressure_ut);						//
		sensor.temperature_ut = SensorReadTemperatureWhenAvailable();						// TEMPERATURE SAMPLE READ

		SensorStartPressureSampling();														// PRESSURE sampling request
		button_state = Button();
		BuzzerTask();
		if (Vario(filter_output))															//
			AutoPowerOffReset();															//			8ms available here
		SerialPressureOutput(sensor.pressure, filter_output);								//
		sensor.pressure_ut = SensorReadPressureWhenAvailable();								// TEMPERATURE SAMPLE READ


		BuzzerSetVolume(button_state);
		if (button_state != BUTTON_NONE)
			VarioDisableFor(VARIO_DISABLE_SOUND_TIME_MS);
		if(button_state == BUTTON_SHORT)
			AutoPowerOffReset();
		else if(button_state == BUTTON_DOUBLE)
			VarioToggleStationaryMode();
		else if((button_state == BUTTON_LONG)||BatteryIsLow()||AutoPowerOffTrue())			// Shutdown mode
		{
			BuzzerStopSound();

			int timeout = VARIO_SETTING_MENU_TIMEOUT_MS;
			while(ButtonState() && timeout--)
			{
				delay_ms(1);
			}
			if ( ButtonState() ) 			// timeout = enter vario setting
			{
				VarioSettingMenu();
			}
			do //  shutdown
			{
				ExitLowPower = false;
				SensorStop();
				BTStop();
				do
				{
					ChargePumpStop();
					BatteryCheckStop();
					ButtonEnableInterrupt();
					PowerEnterUltraLowPower();// Button pressed here (interrupt wakes up)
					ChargePumpStart();
					BuzzerSetBlocking(600,120);
					ButtonDisableInterrupt();
					BatteryCheckInit();
					BatteryCheck();
				}while (BatteryIsLow());
				BTStart();
				SensorStart();
				VarioInitialiseFilter(&sensor);
				int timeout = SINK_ENTER_SINK_SETTING_TIMEOUT_MS;
				while(ButtonState() && timeout--)
					delay_ms(1);
				if ( !ButtonState() ) 			// button release, regular startup
				{
					ButtonDisableInterrupt();
					ExitLowPower = true;
				}
				else// timeout = enter sink setting
				{
					SinkSetting();// then return to low power
				}
			}while (!ExitLowPower);
			BuzzerStartSound();
			AutoPowerOffReset();
		}																				// Shutdown mode
	}
	// end of main loop
}

void DisableAllGPIO()
{

    B_UNSET(P1SEL, 0); B_UNSET(P1SEL2, 0); B_SET(P1DIR, 0); B_UNSET(P1OUT, 0);		// BT Power
    B_UNSET(P1SEL, 1); B_UNSET(P1SEL2, 1); B_UNSET(P1DIR, 1); B_UNSET(P1OUT, 1);	// RX pin
    B_UNSET(P1SEL, 2); B_UNSET(P1SEL2, 2); B_UNSET(P1DIR, 2); B_UNSET(P1OUT, 2);	// TX pin
    B_UNSET(P1SEL, 3); B_UNSET(P1SEL2, 3); B_UNSET(P1DIR, 3); B_UNSET(P1OUT, 3);	// Button
    B_UNSET(P1SEL, 4); B_UNSET(P1SEL2, 4); B_SET(P1DIR, 4); B_UNSET(P1OUT, 4);		// unused
    B_UNSET(P1SEL, 5); B_UNSET(P1SEL2, 5); B_SET(P1DIR, 5); B_SET(P1OUT, 5);		// SPI CLK
    B_UNSET(P1SEL, 6); B_UNSET(P1SEL2, 6); B_SET(P1DIR, 6); B_SET(P1OUT, 6);		// SPI MOSI
    B_UNSET(P1SEL, 7); B_UNSET(P1SEL2, 7); B_SET(P1DIR, 7); B_SET(P1OUT, 7);		// SPI MISO

    B_UNSET(P2SEL, 0); B_UNSET(P2SEL2, 0); B_SET(P2DIR, 0); B_SET(P2OUT, 0);		// SPI CS
    B_UNSET(P2SEL, 1); B_UNSET(P2SEL2, 1); B_SET(P2DIR, 1); B_UNSET(P2OUT, 1);		// unused
    B_UNSET(P2SEL, 2); B_UNSET(P2SEL2, 2); B_SET(P2DIR, 2); B_UNSET(P2OUT, 2);		// unused
    B_UNSET(P2SEL, 3); B_UNSET(P2SEL2, 3); B_SET(P2DIR, 3); B_UNSET(P2OUT, 3);		// unused
    B_UNSET(P2SEL, 4); B_UNSET(P2SEL2, 4); B_SET(P2DIR, 4); B_UNSET(P2OUT, 4);		// H bridge
    B_UNSET(P2SEL, 5); B_UNSET(P2SEL2, 5); B_UNSET(P2DIR, 5); B_UNSET(P2OUT, 5);	// MISO interrupt
    B_UNSET(P2SEL, 6); B_UNSET(P2SEL2, 6); B_SET(P2DIR, 6); B_UNSET(P2OUT, 6);		// unused
    B_UNSET(P2SEL, 7); B_UNSET(P2SEL2, 7); B_SET(P2DIR, 7); B_UNSET(P2OUT, 7);		// unused

    B_UNSET(P3SEL, 0); B_UNSET(P3SEL2, 0); B_SET(P3DIR, 0); B_UNSET(P3OUT, 0);		// unused
    B_UNSET(P3SEL, 1); B_UNSET(P3SEL2, 1); B_SET(P3DIR, 1); B_UNSET(P3OUT, 1);		// unused
    B_UNSET(P3SEL, 2); B_UNSET(P3SEL2, 2); B_SET(P3DIR, 2); B_UNSET(P3OUT, 2);		// H bridge
    B_UNSET(P3SEL, 3); B_UNSET(P3SEL2, 3); B_SET(P3DIR, 3); B_UNSET(P3OUT, 3);		// unused
    B_UNSET(P3SEL, 4); B_UNSET(P3SEL2, 4); B_UNSET(P3DIR, 4); B_UNSET(P3OUT, 4);	// BT AT mode
    B_UNSET(P3SEL, 5); B_UNSET(P3SEL2, 5); B_UNSET(P3DIR, 5); B_UNSET(P3OUT, 5);	// Charge pump
    B_UNSET(P3SEL, 6); B_UNSET(P3SEL2, 6); B_UNSET(P3DIR, 6); B_UNSET(P3OUT, 6);	// Charge pump
    B_UNSET(P3SEL, 7); B_UNSET(P3SEL2, 7); B_SET(P3DIR, 7); B_UNSET(P3OUT, 7);		// unused

    PowerEnterUltraLowPower();
}
