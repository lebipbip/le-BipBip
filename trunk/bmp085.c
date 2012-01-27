#include <stdint.h>
#include <stdio.h>
#include <legacymsp430.h>
#include "target.h"
#include "bithacks.h"
#include "power.h"
#include "bmp085.h"
#include "TI_USCI_I2C_master.h"
#include "uart.h"
#include "oscillator.h"


// Calibration values
static int ac1;
static int ac2;
static int ac3;
static unsigned int ac4;
static unsigned int ac5;
static unsigned int ac6;
static int b1;
static int b2;
static int mb;
static int mc;
static int md;
static long b5;


// Request uncompensated pressure value
void BMP085StartPressureSampling(void)
{
	unsigned char Write_Command[] = {CONTROL_REGISTER_ADDR, PRESSURE_LOW_POWER_CR + (OSS << 6)};	// Write 0x34+(OSS<<6) into register 0xF4
	bmp085WriteBuffer( Write_Command, sizeof(Write_Command));      // Request a pressure reading w/ oversampling setting
}

unsigned long BMP085ReadPressureWhenAvailable(void)
{
	unsigned char buffer[3]= {0,0,0};
	bmp085_Wait_For_OEC();	// Wait at least 4.5ms to 25ms in OSS 3
	bmp085ReadBuffer(VALUE_REGISTER_ADDR, buffer, sizeof(buffer));
	return (((((unsigned long) buffer[0]) << 16) | (((unsigned long) buffer[1]) << 8) | (unsigned long) buffer[2]) >> (8-OSS));
}

// Request uncompensated temperature value
void BMP085SensorStartTemperatureSampling(void)
{
	unsigned char Write_Command[] = {CONTROL_REGISTER_ADDR, TEMPERATURE_CR};	// Read two bytes from registers 0xF6 and 0xF7
	bmp085WriteBuffer( Write_Command, sizeof(Write_Command));      // Request a pressure reading w/ oversampling setting
}

unsigned long BMP085ReadTemperatureWhenAvailable(void)
{
	uint8_t buffer[2];
	bmp085_Wait_For_OEC();	// Wait at least 25ms in OSS 3
	bmp085ReadBuffer(0xF6, buffer, sizeof(buffer));
	return ((uint16_t) buffer[0] << 8) + (uint16_t) buffer[1];
}


#ifdef WAIT_FOR_EOC_INTERRUPT_MODE
interrupt(SENSOR_PORT_VECTOR) Sensor_ISR(void)
{
	if (B_IS_SET(SENSOR_EOC_PORT_IFG, SENSOR_EOC_BIT))	//End Of Conversion Interrupt
	{
		B_UNSET(SENSOR_EOC_PORT_IFG, SENSOR_EOC_BIT); 	// clear IFG
		if (!bmp085OEC())							 	// if EOC pin set for long enough and not bounce from entering low power
			PowerExitLowPower();

	}
}
#endif //WAIT_FOR_EOC_INTERRUPT_MODE


void BMP085Stop(void)
{
	B_UNSET( SENSOR_EOC_PORT_IE, SENSOR_EOC_BIT);		// EOC interrupt disabled

	B_UNSET( P1DIR, 7);									// MSP430G2553 UCB0SDA pin
	B_UNSET( P1SEL, 7);									// MSP430G2553 UCB0SDA pin
	B_UNSET( P1SEL2, 7);								// MSP430G2553 UCB0SDA pin

	B_UNSET( P1DIR, 6);									// MSP430G2553 UCB0SCL pin
	B_UNSET( P1SEL, 6);									// MSP430G2553 UCB0SDA pin
	B_UNSET( P1SEL2, 6);								// MSP430G2553 UCB0SDA pin*/
}

void BMP085Start(void )
{
	B_UNSET( SENSOR_EOC_PORT_DIR, SENSOR_EOC_BIT);     	// input for EOC input
	delay_ms(300);

#ifdef WAIT_FOR_EOC_INTERRUPT_MODE
	B_UNSET( SENSOR_EOC_PORT_IFG, SENSOR_EOC_BIT);		// clear IFG
	B_UNSET( SENSOR_EOC_PORT_IES, SENSOR_EOC_BIT);		// rising edge
	B_SET( SENSOR_EOC_PORT_IE, SENSOR_EOC_BIT);			// interrupt enabled

#endif // WAIT_FOR_EOC_INTERRUPT_MODE

	__enable_interrupt();
	bmp085Calibration();
}

// Stores all of the bmp085's calibration values into global variables
// Calibration values are required to calculate temp and pressure
// This function should be called at the beginning of the program
void bmp085Calibration()
{
  ac1 = bmp085ReadInt(0xAA);
  ac2 = bmp085ReadInt(0xAC);
  ac3 = bmp085ReadInt(0xAE);
  ac4 = bmp085ReadInt(0xB0);
  ac5 = bmp085ReadInt(0xB2);
  ac6 = bmp085ReadInt(0xB4);
  b1 = bmp085ReadInt(0xB6);
  b2 = bmp085ReadInt(0xB8);
  mb = bmp085ReadInt(0xBA);
  mc = bmp085ReadInt(0xBC);
  md = bmp085ReadInt(0xBE);
  /*
#ifdef DEBUG
	char printf_buff[140];
	char printf_len = 0;

	printf_len += snprintf(printf_buff+printf_len, sizeof(printf_buff)-printf_len,
			"ac1: %d\n\r"
			"ac2: %d\n\r"
			"ac3: %d\n\r"
			"ac4: %u\n\r"
			"ac5: %u\n\r"
			"ac6: %u\n\r"
			"b1: %d\n\r"
			"b2: %d\n\r"
			"mb: %d\n\r"
			"mc: %d\n\r"
			"md: %d\n\r"
			, ac1
			, ac2
			, ac3
			, ac4
			, ac5
			, ac6
			, b1
			, b2
			, mb
			, mc
			, md);
	UartXmitString(printf_buff);
#endif //DEBUG
	*/
}

// Calculate temperature given ut.
// Value returned will be in units of 0.1 deg C
short BMP085CompensateTemperature(long ut)
{
  long x1, x2;

  x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;
  x2 = ((long)mc << 11)/(x1 + md);
  b5 = x1 + x2;

  return ((b5 + 8)>>4);
}

// Calculate pressure given up
// calibration values must be known
// b5 is also required so bmp085GetTemperature(...) must be called first.
// Value returned will be pressure in units of Pa.
long BMP085CompensatePressure(long up)
{
  long x1, x2, x3, b3, b6, p;
  unsigned long b4, b7;

  b6 = b5 - 4000;
  // Calculate B3
  x1 = (b2 * (b6 * b6)>>12)>>11;
  x2 = (ac2 * b6)>>11;
  x3 = x1 + x2;
  //b3 = (((((long)ac1)*4 + x3)<>2;
	b3 = (((((long)ac1 )*4 + x3) << OSS) + 2) >> 2;
  // Calculate B4
  x1 = (ac3 * b6)>>13;
  x2 = (b1 * ((b6 * b6)>>12))>>16;
  x3 = ((x1 + x2) + 2)>>2;
  b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;

  b7 = ((unsigned long)(up - b3) * (50000>>OSS));
  if (b7 < 0x80000000)
    p = (b7<<1)/b4;
  else
    p = (b7/b4)<<1;

  x1 = (p>>8) * (p>>8);
  x1 = (x1 * 3038)>>16;
  x2 = (-7357 * p)>>16;
  p += (x1 + x2 + 3791)>>4;

  return p;
}



// Read 2 bytes from the BMP085

int16_t bmp085ReadInt(uint8_t reg_address)
{
	uint8_t data[2];
	bmp085ReadBuffer(reg_address, data, sizeof(data));
	return (int)( (data[0]<<8) | data[1]);// First byte will be from 'address'
										// Second byte will be from 'address'+1
}

int16_t bmp085ReadBuffer(uint8_t reg_address, uint8_t * buffer, uint8_t len)
{
	//while ( TI_USCI_I2C_notready() );
	TI_USCI_I2C_transmitinit( BMP085_ADDRESS, I2C_PRESCALER);  // init transmitting with USCI
	while ( TI_USCI_I2C_notready() );         // wait for bus to be free
	TI_USCI_I2C_transmit( 1, &reg_address);   // start transmitting
	while ( TI_USCI_I2C_notready() );         // wait for bus to be free
	TI_USCI_I2C_receiveinit( BMP085_ADDRESS, I2C_PRESCALER);
	while ( TI_USCI_I2C_notready() );         // wait for bus to be free
	TI_USCI_I2C_receive( len, buffer);
	while ( TI_USCI_I2C_notready() );         // wait for bus to be free
	return 1;
}

int16_t bmp085WriteBuffer(uint8_t * buffer, uint8_t len)
{
	//while ( TI_USCI_I2C_notready() );
	TI_USCI_I2C_transmitinit( BMP085_ADDRESS, I2C_PRESCALER); 			//write device adress on bus
	while ( TI_USCI_I2C_notready() );         							// wait for bus to be free
	TI_USCI_I2C_transmit( len, buffer);
	while ( TI_USCI_I2C_notready() );         							// wait for bus to be free
	return 1;
}


uint8_t bmp085OEC()
{
	return (!B_IS_SET( SENSOR_EOC_PORT_IN, SENSOR_EOC_BIT));     // EOC input state
}


void bmp085_Wait_For_OEC()
{
#ifdef WAIT_FOR_EOC_INTERRUPT_MODE
	if (bmp085OEC())
		PowerEnterLowPower();
#else
	while(bmp085OEC())
		__no_operation();
#endif // WAIT_FOR_EOC_INTERRUPT_MODE
}
