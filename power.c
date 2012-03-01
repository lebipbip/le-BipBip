#include <stdint.h>
#include <stdio.h>
#include <msp430.h>
#include "bithacks.h"
#include <legacymsp430.h>
#include "power.h"
#include "oscillator.h"
#include "target.h"
#include "uart.h"

static int AutoPowerOffCountDown = AUTOPOWER_OFF_TIME_S*TASK_PERIOD_MS;



unsigned char AutoPowerOffCheck(void)
{
	if(--AutoPowerOffCountDown)
		return 0;
	else
		return 1;
}
void AutoPowerOffReset(void)
{
	AutoPowerOffCountDown = AUTOPOWER_OFF_TIME_S*TASK_PERIOD_MS;
}

void BatteryCheckInit(void)
{
#ifndef POWER_DISABLE_ADC
	  ADC10CTL0 = SREF_1 + ADC10SHT_2 + REFON + ADC10ON;
	  ADC10CTL1 = INCH_11;                   //(Vcc-Vss)/2
	  delay_ms(10);							// time for ref to settle
#endif //POWER_DISABLE_ADC
}


void BatteryCheckStop(void)
{
#ifndef POWER_DISABLE_ADC
	ADC10CTL0 &= ~ENC;                        // ADC10 disabled
	ADC10CTL0 = 0;                            // ADC10, Vref disabled completely
#endif //POWER_DISABLE_ADC
}
#define VREF			3300
#define ADCRANGE		1024
//#define VLOWBATT 		1000		// 3000 mV
//#define VMEDIUMBATT	993		// 3200 mV
#define VMEDIUMBATT		810
#define VLOWBATT 		800

static char BatteryState = 0;

bool BatteryIsLow(void)
{
	if (BatteryState== 1)
		return 1;
	return 0;
}



void BatteryCheck(void)
{
#ifndef POWER_DISABLE_ADC
	int adc_val;
	ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
	while (ADC10CTL1 & ADC10BUSY)// ADC10BUSY?
		;	// wait
	adc_val = ADC10MEM;
	#ifdef ADC_DEBUG_OUTPUT
	char printf_buff[100];
	char printf_len = 0;
	unsigned long BattVal = adc_val;
	printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "Battery: %ld\n\r", BattVal);
	printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len, "Battery: %ld V\n\r", (BattVal*VREF)/ADCRANGE);
	UartXmitString(printf_buff);
	#endif //ADC_DEBUG_OUTPUT

	if ( adc_val < VLOWBATT )
		BatteryState = 1;                      // low batt
	else if (adc_val > VMEDIUMBATT)
		BatteryState =  0;                      // batt ok
	else
		BatteryState =  2;                      // medium batt
#else //POWER_DISABLE_ADC

#endif //POWER_DISABLE_ADC

}


