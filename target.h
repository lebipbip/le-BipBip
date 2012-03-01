/*
 * target.h
 *
 *  Created on: 17 sept. 2011
 *      Author: timothee
 */

#ifndef TARGET_H_
#define TARGET_H_
#include "uart.h"


#define DEBUG
#define UART_BAUDRATE UART_BAUDRATE_38400
//#define POWER_DISABLE_ADC
//#define DISABLE_VARIO
//#define CHARGE_PUMP_DEMO
//#define SENSOR_SIMULATED
#define SENSOR_MS5607
#define SENSOR_EOC_INTERRUPT

#define MS5607_CS_BIT	0
#define MS5607_CS_DIR	P2DIR
#define MS5607_CS_OUT	P2OUT
#define MS5607_CS_SEL	P2SEL
#define MS5607_CS_SEL2	P2SEL2

#define MS5607_EOC_PIN	5
#define MS5607_EOC_DIR	P2DIR
#define MS5607_EOC_IN	P2IN
#define MS5607_EOC_IFG	P2IFG
#define MS5607_EOC_IE	P2IE
#define MS5607_EOC_IES	P2IES
#define MS5607_EOC_SEL	P2SEL
#define MS5607_EOC_SEL2	P2SEL2
#define SENSOR_PORT_VECTOR	PORT2_VECTOR

#define TASK_PERIOD_MS	22
#define AUTOPOWER_OFF_TIME_S	20*60

// Capacitive charge pump pin (PWM with dead time)
#define TA0_1_PIN				5
#define TA0_1_DIR 				P3DIR
#define TA0_1_OUT				P3OUT
#define TA0_1_SEL				P3SEL
#define TA0_1_SEL2				P3SEL2

#define TA0_2_PIN				6
#define TA0_2_DIR 				P3DIR
#define TA0_2_OUT				P3OUT
#define TA0_2_SEL				P3SEL
#define TA0_2_SEL2				P3SEL2

// Full H bridge (PWM with dead time)
#define TA1_1_PIN				2
#define TA1_1_DIR 				P3DIR
#define TA1_1_OUT				P3OUT
#define TA1_1_SEL				P3SEL
#define TA1_1_SEL2				P3SEL2

#define TA1_2_PIN				4
#define TA1_2_DIR 				P2DIR
#define TA1_2_OUT				P2OUT
#define TA1_2_SEL				P2SEL
#define TA1_2_SEL2				P2SEL2


#define BUTTON_PORT_DIR			P1DIR
#define BUTTON_PORT_IN			P1IN
#define BUTTON_PORT_IFG 		P1IFG
#define BUTTON_PORT_IES 		P1IES
#define BUTTON_PORT_IE 			P1IE
#define BUTTON_PORT_VECTOR		PORT1_VECTOR
#define BUTTON_PIN_BIT			3

/*
#define WAIT_FOR_EOC_INTERRUPT_MODE
#ifdef WAIT_FOR_EOC_INTERRUPT_MODE
	#define SENSOR_EOC_PORT_IFG 	P2IFG
	#define SENSOR_EOC_PORT_IES		P2IES
	#define SENSOR_EOC_PORT_IE	 	P2IE
	#define SENSOR_PORT_VECTOR	PORT2_VECTOR
#endif //WAIT_FOR_EOC_INTERRUPT_MODE
#define SENSOR_EOC_PORT_DIR		P2DIR
#define SENSOR_EOC_PORT_IN		P2IN

#define SENSOR_EOC_BIT 			5
*/

#endif /* TARGET_H_ */
