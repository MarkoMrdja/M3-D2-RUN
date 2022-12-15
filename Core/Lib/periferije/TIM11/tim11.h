/*
 * tim11.h
 *
 *  Created on: Mar 27, 2022
 *      Author: Marko
 */

#ifndef LIB_PERIFERIJE_TIM11_TIM11_H_
#define LIB_PERIFERIJE_TIM11_TIM11_H_

#include "stm32f4xx.h"
#include"../../../../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_tim.h"
#include <stdint.h>
#include <stdbool.h>
#include"../USART1/usart1.h"
#include"../ENCODER/encoder.h"
#include"../PWM/pwm.h"

void
tim5_init (void);


#endif /* LIB_PERIFERIJE_TIM11_TIM11_H_ */
