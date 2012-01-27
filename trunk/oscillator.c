/*
 * oscillator.c
 *
 *  Created on: 20 sept. 2011
 *      Author: timothee
 */

#include <msp430.h>
#include "oscillator.h"

void OscillatorStart(void)
{
	#if	(CPU_FREQUENCY == 16000000)
		BCSCTL1 = CALBC1_16MHZ; // set DCO clock for MCLK and SMCLK
		DCOCTL = CALDCO_16MHZ;
	#endif
	#if	(CPU_FREQUENCY == 1000000)
		BCSCTL1 = CALBC1_1MHZ; // set DCO clock for MCLK and SMCLK
		DCOCTL = CALDCO_1MHZ;
	#endif
	#if	(CPU_FREQUENCY == 2000000)
		BCSCTL1 = CALBC1_2MHZ; // set DCO clock for MCLK and SMCLK
		DCOCTL = CALDCO_2MHZ;
	#endif
}
/*
void OscillatorStop(void)
{
	#if	(CPU_FREQUENCY == 16000000)
		BCSCTL1 = CALBC1_16MHZ; // set DCO clock for MCLK and SMCLK
		DCOCTL = CALDCO_16MHZ;
	#endif
	#if	(CPU_FREQUENCY == 1000000)
		BCSCTL1 = CALBC1_1MHZ; // set DCO clock for MCLK and SMCLK
		DCOCTL = CALDCO_1MHZ;
	#endif
}*/

void delay_ms(unsigned int delay)
{
    while (delay--)
    {
        __delay_cycles(CPU_FREQUENCY/1000);
    }
}
