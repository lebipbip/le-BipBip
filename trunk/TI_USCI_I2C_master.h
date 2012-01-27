#ifndef USCI_LIB
#define USCI_LIB

#include "oscillator.h"

#define SDA_PIN BIT7                                  // MSP430G2553 UCB0SDA pin
#define SCL_PIN BIT6                                  // MSP430G2553 UCB0SCL pin
#if	(CPU_FREQUENCY == 16000000)
	#define I2C_PRESCALER 8
#endif
#if	(CPU_FREQUENCY == 1000000)
	#define I2C_PRESCALER 2
#endif
#if	(CPU_FREQUENCY == 2000000)
	#define I2C_PRESCALER 2
#endif

void TI_USCI_I2C_receiveinit(unsigned char slave_address, unsigned int prescale);
void TI_USCI_I2C_transmitinit(unsigned char slave_address, unsigned int prescale);


void TI_USCI_I2C_receive(unsigned char byteCount, unsigned char *field);
void TI_USCI_I2C_transmit(unsigned char byteCount, unsigned char *field);


unsigned char TI_USCI_I2C_slave_present(unsigned char slave_address);
unsigned char TI_USCI_I2C_notready();


#endif //USCI_LIB
