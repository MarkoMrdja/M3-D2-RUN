/*
 * encoder.h
 *
 *  Created on: Mar 27, 2022
 *      Author: Marko
 */

#ifndef LIB_PERIFERIJE_ENCODER_ENCODER_H_
#define LIB_PERIFERIJE_ENCODER_ENCODER_H_
#include <stdint.h>
#include "stm32f4xx.h"
#include <string.h>
#include <stdbool.h>
#include <math.h>

uint8_t
read_stanje_enkodera_tim4 (void);

uint16_t
read_cnt_tim4 (void);

uint16_t
read_cnt_tim3 (void);

void
sens_robot (void);

void HAL_TIM_IC_CaptureCallback1(TIM_HandleTypeDef* htim);
void HAL_TIM_IC_CaptureCallback2(TIM_HandleTypeDef* htim);

float get_brzina_levog_pasivnog(void);
float get_brzina_desnog_pasivnog (void);

#endif /* LIB_PERIFERIJE_ENCODER_ENCODER_H_ */
