#include <stdint.h>
#include <stdio.h>
#include <msp430.h>
#include "bithacks.h"
#include <legacymsp430.h>
#include "power.h"
#include "oscillator.h"
#include "target.h"
#include "uart.h"


static uint64_t AutoPowerOffCountDownCycle = AUTOPOWER_OFF_TIME_S;
static uint8_t  AutoPowerOffSecondCounter	= 0;

#define ADCRANGE					1024
#define VREF						2500
#define VCC_INPUT_COEF				2 		//INCH_11 = (Vcc-Vss)/2
#define LOW_BATTERY_VOLTAGE			3000	// mV, before Over- discharge detection voltage (2700mV)



bool AutoPowerOffSecond(void)
{
	if(AutoPowerOffSecondCounter)
	{
		AutoPowerOffSecondCounter--;
		return false;
	}
	else
	{
		AutoPowerOffSecondCounter = 1000/TASK_PERIOD_MS;
		return true;
	}
}

bool AutoPowerOffTrue(void)
{
	if(AutoPowerOffSecond())
	{
		if(AutoPowerOffCountDownCycle == 0)
			return true;
		else
			AutoPowerOffCountDownCycle--;
			return false;
	}
	else
		return false;
}
void AutoPowerOffReset(void)
{
	AutoPowerOffCountDownCycle = AUTOPOWER_OFF_TIME_S;
}
uint16_t BatteryGetVoltage(void)
{
	uint32_t adc_val;
	ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
	while (ADC10CTL1 & ADC10BUSY);	// wait
	adc_val = ADC10MEM;
	return (uint16_t)((VREF*adc_val*2)/ADCRANGE);
}

void BatteryCheckInit(void)
{
	ADC10CTL0 = SREF_1 + ADC10SHT_2 + REFON + REF2_5V + ADC10ON;
	ADC10CTL1 = INCH_11;                   	//(Vcc-Vss)/2
	delay_ms(1);							// time for ref to settle
}

void BatteryPrintVoltage(void)
{
	uint16_t BatteryVoltage = BatteryGetVoltage();
	char printf_buff[100];
	char printf_len = 0;
	printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "MCU VCC: %dmV %s", BatteryVoltage, ICT_TAB_STRING);
	if (BatteryVoltage > 3000 && BatteryVoltage < 3600 )
		printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "OK\n\r");
	else
		printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "FAIL\n\r");
	UartXmitString(printf_buff);
}


void BatteryCheckStop(void)
{
	ADC10CTL0 &= ~ENC;                        // ADC10 disabled
	ADC10CTL0 = 0;                            // ADC10, Vref disabled completely
}


static char BatteryState = 1;

bool BatteryIsLow(void)
{
	if (BatteryState == 0)
		return true;
	else
		return false;
}

void BatteryCheck(void)
{
#ifndef POWER_DISABLE_ADC
	if ( BatteryGetVoltage() < LOW_BATTERY_VOLTAGE )
		BatteryState = 0;                      // low batt
	else
		BatteryState = 1;                      // batt ok
#endif //POWER_DISABLE_ADC
}





