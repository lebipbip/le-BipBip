/*
 * target.h
 *
 *  Created on: 17 sept. 2011
 *      Author: timothee
 */

#ifndef TARGET_H_
#define TARGET_H_
#include "uart.h"

#define VERSION_MAJOR		2
#define VERSION_MINOR		2


//Uart baudrate
#define 	UART_BAUDRATE 					UART_BAUDRATE_38400
// In Circuit Test defines
#define 	ICT_TAB_STRING					"\t\t"
//Energy and power
#define 	AUTOPOWER_OFF_TIME_S			20*60
// Vario define
#define 	VARIO_DISABLE_SOUND_TIME_MS		500		// Disable vario for this time in ms when the button is pressed or on startup
// Filter&Sensor
#define 	FILTER_INIT_THREASHOLD			30		// cm/s
#define 	FILTER_INIT_STABLE_SAMPLE		100  	// when FILTER_INIT_STABLE_SAMPLE samples are below FILTER_INIT_THREASHOLD start main loop
#define 	FILTER_INIT_MAX_SAMPLE			2000 	// if FILTER_INIT_THREASHOLD not reached within this limit, start anyway
#define 	TASK_PERIOD_MS					16 		// time for a main cycle loop in ms


/*******************************************************/
// Dev define, SHOULD NOT be enabled in production
/*******************************************************/
//#define 	DEBUG
//#define 	DEBUG_BUTTON
//#define 	BLUETOOTH_ENABLE
//#define 	DEMO
//#define 	SENSOR_DATA_SIMU
//#define 	SENSOR_SIMULATED
//#define 	VARIO_TEST_RANGE_POSITIVE
//#define 	VARIO_TEST_RANGE_STATIONARY
//#define 	VARIO_TEST_RANGE_NEGATIVE
//#define 	CHARGE_PUMP_DEMO
//#define 	SENSOR_DEBUG
//#define 	NMEA_OUTPUT_ENABLE
//#define 	DISABLE_VARIO
//#define 	POWER_DISABLE_ADC
/*******************************************************/

/*******************************************************/
// DONT CHANGE SETTINGS BELLOW: can damage the board
/*******************************************************/
// Sensor
#ifndef 	SENSOR_SIMULATED
	#define 	SENSOR_MS5607
#endif 		//SENSOR_SIMULATED
#define 	SENSOR_SPI
#define 	SENSOR_EOC_INTERRUPT
#define 	MS5607_CS_BIT			0
#define 	MS5607_CS_DIR			P2DIR
#define 	MS5607_CS_OUT			P2OUT
#define 	MS5607_CS_SEL			P2SEL
#define 	MS5607_CS_SEL2			P2SEL2
#define 	MS5607_EOC_PIN			5
#define 	MS5607_EOC_DIR			P2DIR
#define 	MS5607_EOC_IN			P2IN
#define 	MS5607_EOC_IFG			P2IFG
#define 	MS5607_EOC_IE			P2IE
#define 	MS5607_EOC_IES			P2IES
#define 	MS5607_EOC_SEL			P2SEL
#define 	MS5607_EOC_SEL2			P2SEL2
#define 	SENSOR_PORT_VECTOR	PORT2_VECTOR
// Capacitive charge pump pin (PWM with dead time)
#define 	TA0_1_PIN				5
#define 	TA0_1_DIR 				P3DIR
#define 	TA0_1_OUT				P3OUT
#define 	TA0_1_SEL				P3SEL
#define 	TA0_1_SEL2				P3SEL2
#define 	TA0_2_PIN				6
#define 	TA0_2_DIR 				P3DIR
#define 	TA0_2_OUT				P3OUT
#define 	TA0_2_SEL				P3SEL
#define 	TA0_2_SEL2				P3SEL2
// Full H bridge (BTL bridge) (PWM with dead time)
#define 	TA1_1_PIN				2
#define 	TA1_1_DIR 				P3DIR
#define 	TA1_1_OUT				P3OUT
#define 	TA1_1_SEL				P3SEL
#define 	TA1_1_SEL2				P3SEL2
#define 	TA1_2_PIN				4
#define 	TA1_2_DIR 				P2DIR
#define 	TA1_2_OUT				P2OUT
#define 	TA1_2_SEL				P2SEL
#define 	TA1_2_SEL2				P2SEL2
#define 	BUTTON_PORT_DIR			P1DIR
#define 	BUTTON_PORT_IN			P1IN
#define 	BUTTON_PORT_IFG 		P1IFG
#define 	BUTTON_PORT_IES 		P1IES
#define 	BUTTON_PORT_IE 			P1IE
#define 	BUTTON_PORT_VECTOR		PORT1_VECTOR
#define 	BUTTON_PIN_BIT			3
/*******************************************************/



#endif /* TARGET_H_ */
