/*
 * encoder.c
 *
 *  Created on: Mar 27, 2022
 *      Author: Marko
 */
#include "encoder.h"

static uint8_t stanje_enkodera_tim4 = 0;
static uint8_t stanje_enkodera_tim3 = 0;

static const float rastojanje_pasivnih = 266;
static uint16_t rezolucija_enc_pasivnog = 2048;

static int32_t brzina_levog_pasivnog = 0.0;
static int32_t brzina_desnog_pasivnog = 0.0;
static int32_t levi = 0.0;
static int32_t desni = 0.0;

static uint16_t inc_levog_pasivnog = 0;
static int16_t pre_inc_levog_pasivnog = 0;

static uint16_t inc_desnog_pasivnog = 0;
static int16_t pre_inc_desnog_pasivnog = 0;

double volatile omega = 0.0;
double volatile brzina = 0.0;

volatile double teta = 0.0;
double volatile x = 0.0;
double volatile y = 0.0;


uint32_t counter1 = 0.0;
int16_t count1 = 0.0;
int16_t position1 = 0.0;

uint32_t counter2 = 0.0;
int16_t count2 = 0.0;
int16_t position2 = 0.0;

void HAL_TIM_IC_CaptureCallback1(TIM_HandleTypeDef *htim) {
	counter1 = __HAL_TIM_GET_COUNTER(htim);
	count1 = (int16_t) counter1;
	position1 = count1 / 4;
}

void HAL_TIM_IC_CaptureCallback2(TIM_HandleTypeDef *htim) {
	counter2 = __HAL_TIM_GET_COUNTER(htim);
	count2 = (int16_t) counter2;
	position2 = count2 / 4;
}

bool encoder_tim3_dir(void) {
	if ((TIM3->CR1 & (0b1 << 4)) == (0b1 << 4)) {
		return true;
	} else {
		return false;
	}
}

bool encoder_tim4_dir(void) {
	if ((TIM4->CR1 & (0b1 << 4)) == (0b1 << 4)) {
		return true;
	} else {
		return false;
	}
}

uint16_t read_cnt_tim4(void) {
	return TIM4->CNT;
}
uint16_t read_cnt_tim3(void) {
	return TIM3->CNT;
}

uint8_t read_stanje_enkodera_tim4(void) {
	uint8_t temp;

	temp = stanje_enkodera_tim4;
	return stanje_enkodera_tim4;

	return temp;
}

uint8_t read_stanje_enkodera_tim3(void) {
	uint8_t temp2;

	temp2 = stanje_enkodera_tim3;
	return stanje_enkodera_tim3;

	return temp2;
}
/*
static float koverzija_inkremenata_u_milimetre(int32_t increments)
{
	float obim_pasivnog = 204.1; //mm
	float n = (1000/obim_pasivnog);
	//inkrementi po metru
	float inc_po_metru = n * rezolucija_enc_pasivnog*4 ;  // ovde je bilo i *4
	float konverziona_konstanta = 1000 / inc_po_metru;
	return (float)increments * konverziona_konstanta;
}

 if (encoder_tim3_dir () == 0)
 {
 // Up-counting
 if (encoder1_cnt >= encoder1_prev_cnt)
 {
 encoder1_velocity_inc = encoder1_cnt - encoder1_prev_cnt;
 }
 else
 {
 encoder1_velocity_inc = (0xFFFF - encoder1_prev_cnt) + encoder1_cnt;
 }
 }
 else
 {
 // Down-counting
 if (encoder1_cnt <= encoder1_prev_cnt)
 {
 encoder1_velocity_inc = encoder1_cnt - encoder1_prev_cnt;
 }
 else
 {
 encoder1_velocity_inc =
 -((0xFFFF - encoder1_cnt) + encoder1_prev_cnt);
 }
 }
 */
//Funkcija za ocitavanje stanja robota - x,y,teta
void sens_robot(void) {
	//LEVI
	//Ocitati inkremenata sa enkodera - pasivni tockovi


	inc_levog_pasivnog = (int32_t)read_cnt_tim4();
	inc_desnog_pasivnog = (int32_t)read_cnt_tim3();

	TIM4->CNT=0;
	TIM3->CNT=0;

	//levi = (int32_t)read_cnt_tim4();
	//desni = (int32_t)read_cnt_tim3();

	pre_inc_levog_pasivnog += inc_levog_pasivnog;
	pre_inc_desnog_pasivnog += inc_desnog_pasivnog;


	brzina_levog_pasivnog = 0;
	brzina_desnog_pasivnog = 0;
	/*if (encoder_tim4_dir() == 0) {
		if (inc_levog_pasivnog >= pre_inc_levog_pasivnog) { //UPCOUNTING
			brzina_levog_pasivnog = inc_levog_pasivnog - pre_inc_levog_pasivnog;
		} else {
			brzina_levog_pasivnog = (0xFFFF - pre_inc_levog_pasivnog)
					+ inc_levog_pasivnog;
		}
	} else {
		// Down-counting
		if (inc_levog_pasivnog <= pre_inc_levog_pasivnog) {
			brzina_levog_pasivnog = inc_levog_pasivnog - pre_inc_levog_pasivnog;
		} else {
			brzina_levog_pasivnog = -((0xFFFF - inc_levog_pasivnog)
					+ pre_inc_levog_pasivnog);
		}
	}

	if (encoder_tim3_dir() == 0) {
			if (inc_desnog_pasivnog >= pre_inc_desnog_pasivnog) { //UPCOUNTING
				brzina_desnog_pasivnog = inc_desnog_pasivnog - pre_inc_desnog_pasivnog;
			} else {
				brzina_desnog_pasivnog = (0xFFFF - pre_inc_desnog_pasivnog)
						+ inc_desnog_pasivnog;
			}
		} else {
			// Down-counting
			if (inc_desnog_pasivnog <= pre_inc_desnog_pasivnog) {
				brzina_desnog_pasivnog = inc_desnog_pasivnog - pre_inc_desnog_pasivnog;
			} else {
				brzina_desnog_pasivnog = -((0xFFFF - inc_desnog_pasivnog)
						+ pre_inc_desnog_pasivnog);
			}
		}

	pre_inc_levog_pasivnog = inc_levog_pasivnog;
	pre_inc_desnog_pasivnog = inc_desnog_pasivnog;
*/
	brzina_levog_pasivnog=(int16_t)inc_levog_pasivnog;
	brzina_desnog_pasivnog=(int16_t)inc_desnog_pasivnog;

	float brzina_enkodera1 =  0.0255024303*(float)brzina_levog_pasivnog;
	float brzina_enkodera2 =  0.0255024303*(float)brzina_desnog_pasivnog;
	//float brzina_enkodera1 =  koverzija_inkremenata_u_milimetre(inc_levog_pasivnog);
	//float brzina_enkodera2 =  koverzija_inkremenata_u_milimetre(inc_desnog_pasivnog);


	float temp1 = (brzina_enkodera1 + brzina_enkodera2);
	float temp2 = (brzina_enkodera1 - brzina_enkodera2);

	 // R = (temp1/temp2)*d;
	  omega = temp2 / rastojanje_pasivnih;

	  teta += omega; // orijentacija robota

	  if(teta>M_PI)
	  		  teta -=2*M_PI;
	  if(teta<-M_PI)
	  		  teta +=2*M_PI;
	  brzina = temp1 / 2;

	  x += brzina * cos(teta); //x
	  y += brzina * sin(teta); //y
}

//Obnavljanje vrednosti prethodnih inkremenata





float get_brzina_levog_pasivnog(void) {

return brzina_levog_pasivnog;
}

float get_brzina_desnog_pasivnog(void) {

return brzina_desnog_pasivnog;
}

