#ifndef BMP085_H_INCLUDED
#define BMP085_H_INCLUDED



#define CONTROL_REGISTER_ADDR       0xF4

#define TEMPERATURE_CR              0x2E
#define PRESSURE_LOW_POWER_CR       0x34
#define PRESSURE_STANDARD_CR        0x74
#define PRESSURE_HIGH_RES_CR        0xB4
#define PRESSURE_ULTRA_HIGH_RES_CR  0xF4

#define VALUE_REGISTER_ADDR         0xF6

#define OSS  3				  // Oversampling Setting

#define BMP085_ADDRESS 0xEE>>1  // I2C address of BMP085


void bmp085_Wait_For_OEC();
short BMP085CompensateTemperature(long ut);
long BMP085CompensatePressure(long up);
void bmp085Calibration();
unsigned long BMP085ReadPressureWhenAvailable(void);
unsigned long BMP085ReadTemperatureWhenAvailable(void);
void BMP085StartPressureSampling(void);
void BMP085SensorStartTemperatureSampling(void);


int8_t bmp085Read(uint8_t address);
int16_t bmp085ReadInt(uint8_t address);
uint8_t bmp085OEC();
/*
unsigned int bmp085ReadUT(void);
long bmp085ReadUP(void);
*/
int16_t bmp085ReadBuffer(uint8_t reg_address, uint8_t * buffer, uint8_t len);
int16_t bmp085WriteBuffer(uint8_t * buffer, uint8_t len);
void BMP085Start(void );
void BMP085Stop(void);

#endif //BMP085_H_INCLUDED
