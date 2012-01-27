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
//#define DISABLE_VARIO
#define SIMULTATED_DATA

#define SENSOR_BMP085
//#define SENSOR_MS5607

#define TASK_PERIOD_MS	22
#define AUTOPOWER_OFF_TIME_S	20*60


#define TA0_1_PIN				6
#define TA0_1_DIR 				P2DIR
#define TA0_1_OUT				P2OUT
#define TA0_1_SEL				P2SEL
#define TA0_1_SEL2				P2SEL2

#define TA1_1_PIN				1
#define TA1_1_DIR 				P2DIR
#define TA1_1_OUT				P2OUT
#define TA1_1_SEL				P2SEL
#define TA1_1_SEL2				P2SEL2

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


#define WAIT_FOR_EOC_INTERRUPT_MODE
#ifdef WAIT_FOR_EOC_INTERRUPT_MODE
	#define SENSOR_PORT_VECTOR	PORT2_VECTOR
#endif //WAIT_FOR_EOC_INTERRUPT_MODE
#define SENSOR_EOC_PORT_DIR		P2DIR
#define SENSOR_EOC_PORT_IN		P2IN
#define SENSOR_EOC_PORT_IFG 	P2IFG
#define SENSOR_EOC_PORT_IES		P2IES
#define SENSOR_EOC_PORT_IE	 	P2IE
#define SENSOR_EOC_BIT 			0


#endif /* TARGET_H_ */
