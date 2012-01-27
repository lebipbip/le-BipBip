################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../I2C.c \
../TI_USCI_I2C_master.c \
../bluetooth.c \
../bmp085.c \
../booster.c \
../button.c \
../buzzer.c \
../filter.c \
../lebipbip.c \
../ms5607.c \
../oscillator.c \
../power.c \
../sink.c \
../timer.c \
../uart.c \
../vario.c 

OBJS += \
./I2C.o \
./TI_USCI_I2C_master.o \
./bluetooth.o \
./bmp085.o \
./booster.o \
./button.o \
./buzzer.o \
./filter.o \
./lebipbip.o \
./ms5607.o \
./oscillator.o \
./power.o \
./sink.o \
./timer.o \
./uart.o \
./vario.o 

C_DEPS += \
./I2C.d \
./TI_USCI_I2C_master.d \
./bluetooth.d \
./bmp085.d \
./booster.d \
./button.d \
./buzzer.d \
./filter.d \
./lebipbip.d \
./ms5607.d \
./oscillator.d \
./power.d \
./sink.d \
./timer.d \
./uart.d \
./vario.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	/usr/local/mspgcc/bin/msp430-gcc -mmcu=msp430g2553 -I/usr/local/mspgcc/msp430/include -I/usr/local/include -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


