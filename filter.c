/*
 * filter.c
 *
 *  Created on: 23 sept. 2011
 *      Author: timothee
 */

#include <stdio.h>
#include <stdint.h>
#include "uart.h"

#define LP_A_0 	1
#define LP_B_0  1

static int64_t  LP_y_1 = 0;

// low pass filter for temperature
int32_t LP_Filter(int32_t x)
{
	long long y = LP_A_0*LP_y_1 + LP_B_0*x;

	LP_y_1 = y/(LP_A_0+LP_B_0);

	return LP_y_1;
}


#define FILTER_INTEGER_FACTOR	2048

// feedforward coefficients
#define B_0  	251
#define B_1  	0
#define B_2  	-251
// feedback coefficients,
#define A_0 	2048
#define A_1  	-3576
#define A_2  	1547

static long long int x_1 = 0;
static long long int x_2 = 0;
static long long int y_1 = 0;
static long long int y_2 = 0;

//butterworth band pass filter: remove noise (integer:low pass) and derivate signal (high pass)
int32_t Filter(uint32_t x)
{
	int64_t y;
	x = x * FILTER_INTEGER_FACTOR;
	y = ((long long int)B_0*(long long int)x) + ((long long int)B_1*x_1) + ((long long int)B_2*x_2)
			- ((long long int)A_1*y_1) - ((long long int)A_2*y_2);
#ifdef DEBUG
	char printf_buff[128];
	char printf_len = 0;

	printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len,
			"x: %ld ", x);
	printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len,
			"x_1: %lld ", x_1);
	printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len,
			"x_2: %lld ", x_2);
	printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len,
			"y_1: %lld\t ", y_1);
	printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len,
			"y_2: %lld ", y_2);
	printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len,
			"y: %lld\n\r", y);
	UartXmitString(printf_buff);
#endif // DEBUG
	x_2 = x_1;
	x_1 = x;

	y_2 = y_1;
	y_1 = y/A_0;

	return -y_1/FILTER_INTEGER_FACTOR;
}
