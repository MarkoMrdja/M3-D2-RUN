/*
 * pwm.h
 *
 *  Created on: Mar 27, 2022
 *      Author: Marko
 */

#ifndef LIB_PERIFERIJE_PWM_PWM_H_
#define LIB_PERIFERIJE_PWM_PWM_H_
#include<stdint.h>
#include "stm32f4xx.h"
#include <stdbool.h>
#include <string.h>
#include "../ENCODER/encoder.h"
#include "../Inc/main.h"
#include "../USART1/usart1.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#define M_PI 3.14159265358979323846

void pid_init(void);
void regulacija_ugla(void);
void regulacija(void);
void faktor_ispune_t1_smer1(float upravljanje);
void faktor_ispune_t1_smer2(float upravljanje);
void faktor_ispune_t2_smer2(float upravljanje);
void faktor_ispune_t2_smer1(float upravljanje);
void faktor_ispune_nazad(uint8_t upravljanje);
void faktor_ispune_napred(uint8_t upravljanje);
void medju_stanje(void);
void pokreni_traku(uint8_t upravljanje);
#endif /* LIB_PERIFERIJE_PWM_PWM_H_ */
