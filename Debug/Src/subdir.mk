################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/can.c \
../Src/gpio.c \
../Src/i2c.c \
../Src/imu.c \
../Src/inv_mpu.c \
../Src/main.c \
../Src/stm32f3xx_hal_msp.c \
../Src/stm32f3xx_it.c \
../Src/system_stm32f3xx.c \
../Src/usart.c 

OBJS += \
./Src/can.o \
./Src/gpio.o \
./Src/i2c.o \
./Src/imu.o \
./Src/inv_mpu.o \
./Src/main.o \
./Src/stm32f3xx_hal_msp.o \
./Src/stm32f3xx_it.o \
./Src/system_stm32f3xx.o \
./Src/usart.o 

C_DEPS += \
./Src/can.d \
./Src/gpio.d \
./Src/i2c.d \
./Src/imu.d \
./Src/inv_mpu.d \
./Src/main.d \
./Src/stm32f3xx_hal_msp.d \
./Src/stm32f3xx_it.d \
./Src/system_stm32f3xx.d \
./Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 '-D__weak=__attribute__((weak))' -DEMPL_TARGET_STM32 -DMPU9250 '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F303x8 -I"C:/Users/Francesco Conforte/workspace/nodo-imu/Inc" -I"C:/Users/Francesco Conforte/workspace/nodo-imu/Drivers/STM32F3xx_HAL_Driver/Inc" -I"C:/Users/Francesco Conforte/workspace/nodo-imu/Drivers/STM32F3xx_HAL_Driver/Inc/Legacy" -I"C:/Users/Francesco Conforte/workspace/nodo-imu/Drivers/CMSIS/Device/ST/STM32F3xx/Include" -I"C:/Users/Francesco Conforte/workspace/nodo-imu/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


