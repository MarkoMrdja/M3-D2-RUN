################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Lib/periferije/ENCODER/encoder.c 

OBJS += \
./Core/Lib/periferije/ENCODER/encoder.o 

C_DEPS += \
./Core/Lib/periferije/ENCODER/encoder.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Lib/periferije/ENCODER/%.o Core/Lib/periferije/ENCODER/%.su: ../Core/Lib/periferije/ENCODER/%.c Core/Lib/periferije/ENCODER/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Lib-2f-periferije-2f-ENCODER

clean-Core-2f-Lib-2f-periferije-2f-ENCODER:
	-$(RM) ./Core/Lib/periferije/ENCODER/encoder.d ./Core/Lib/periferije/ENCODER/encoder.o ./Core/Lib/periferije/ENCODER/encoder.su

.PHONY: clean-Core-2f-Lib-2f-periferije-2f-ENCODER

