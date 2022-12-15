################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Lib/periferije/PWM/pwm.c 

OBJS += \
./Core/Lib/periferije/PWM/pwm.o 

C_DEPS += \
./Core/Lib/periferije/PWM/pwm.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Lib/periferije/PWM/%.o Core/Lib/periferije/PWM/%.su: ../Core/Lib/periferije/PWM/%.c Core/Lib/periferije/PWM/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Lib-2f-periferije-2f-PWM

clean-Core-2f-Lib-2f-periferije-2f-PWM:
	-$(RM) ./Core/Lib/periferije/PWM/pwm.d ./Core/Lib/periferije/PWM/pwm.o ./Core/Lib/periferije/PWM/pwm.su

.PHONY: clean-Core-2f-Lib-2f-periferije-2f-PWM

