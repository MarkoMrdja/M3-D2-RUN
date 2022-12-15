/*
 * pwm.c
 *
 *  Created on: Mar 27, 2022
 *      Author: Marko
 */
#include "pwm.h"

volatile float Kpd;
volatile float Kid;
volatile float Kdd;
static volatile float Kp_teta;
static volatile float Ki_teta;
static volatile float Kd_teta;

volatile float Vdl = 0;
volatile float Vl = 0;
volatile float Vdr = 0;
volatile float Vr = 0;

float volatile teta_ei_max;
float volatile teta_ei_min;
static float volatile d_ei_max;
static float volatile d_ei_min;
float volatile teta_d;
static volatile float teta_e = 0.0;
static volatile float teta_de = 0.0;
static volatile float teta_de_p = 0.0;
volatile float teta_min;
volatile float teta_ei;
volatile float d_e, d_ei, d_de_p, d_de;
static volatile float d_e_min = 0.03;

volatile float x_e = 0.0;
volatile float x_el = 0.0;
volatile float y_el = 0.0;
volatile float y_e = 0.0;
volatile float acc = 0.5;
volatile float x_d;
volatile float y_d;
volatile float t_ce;
volatile uint8_t smer = 1;
volatile int kreni = 1;

//externe prom
extern uint8_t prom;
extern uint8_t prom2;
extern double teta;
extern double x;
extern double y;
extern double omega;
extern double brzina;
uint8_t upravljanje_proslot1s1 = 0.0;
uint8_t upravljanje_proslot1s2 = 0.0;
uint8_t upravljanje_proslot2s1 = 0.0;
uint8_t upravljanje_proslot2s2 = 0.0;

volatile uint8_t faza;

int16_t upravljanje_deltat1s1 = 0.0;
int16_t upravljanje_deltat1s2 = 0.0;
int16_t upravljanje_deltat2s1 = 0.0;
int16_t upravljanje_deltat2s2 = 0.0;
void faktor_ispune_t1_smer1(float upravljanje);
void faktor_ispune_t1_smer2(float upravljanje);
void faktor_ispune_t2_smer1(float upravljanje);
void faktor_ispune_t2_smer2(float upravljanje);


void pid_init(void) {

	Kp_teta = 40; //32
	Ki_teta = 0.15; //0.1
	Kd_teta = 23000; //21k

	Kpd = 35; //40
	Kid = 0.13; //	0.15
	Kdd = 22000; //25k

	d_ei_max = 110;
	d_ei_min = -110;


	teta_ei_max = 130; //140
	teta_ei_min = -130;
}

void faktor_ispune_napred(uint8_t upravljanje) {
	if (upravljanje > 100) {
		upravljanje = 100;
	} else if (upravljanje < 0) {
		upravljanje = 0;
	}
	TIM1->CCR1 = 0;
	TIM1->CCR2 = upravljanje;
	TIM1->CCR3 = upravljanje;
	TIM1->CCR4 = 0;
}
void medju_stanje(void) {
	TIM1->CCR1 = 0;
	TIM1->CCR3 = 0;
}
void faktor_ispune_t1_smer1(float upravljanje) {
	TIM1->CCR1 = (42 * upravljanje - 1);
	GPIOB->ODR |= (0b1 << 0);
	GPIOB->ODR &= ~(0b1 << 3);
}
void faktor_ispune_t1_smer2(float upravljanje) {
	TIM1->CCR1 = (42 * upravljanje - 1);
	GPIOB->ODR |= (0b1 << 3);
	GPIOB->ODR &= ~(0b1 << 0);
}
void faktor_ispune_t2_smer2(float upravljanje) {
	TIM1->CCR3 = (42 * upravljanje - 1);
	GPIOB->ODR |= (0b1 << 4);
	GPIOB->ODR &= ~(0b1 << 5);
}

void faktor_ispune_t2_smer1(float upravljanje) {
	TIM1->CCR3 = (42 * upravljanje - 1);
	GPIOB->ODR |= (0b1 << 5);
	GPIOB->ODR &= ~(0b1 << 4);
}
void faktor_ispune_nazad(uint8_t upravljanje) {
	if (upravljanje > 100) {
		upravljanje = 100;
	} else if (upravljanje < 0) {
		upravljanje = 0;
	}
	TIM1->CCR1 = upravljanje;
	TIM1->CCR2 = 0;
	TIM1->CCR3 = 0;
	TIM1->CCR4 = upravljanje;
	;
}

void pokreni_traku(uint8_t upravljanje)
{
	if (abs(upravljanje) > 100)
	{
		if(upravljanje < 100)
		{
			upravljanje = -100;
		}
		else
		{
			upravljanje = 100;
		}
	}
	TIM1->CCR2 = (42*upravljanje - 1);
	TIM1->CCR4 = 0;
}

void regulacija_ugla(void) {


	teta_e = teta_d - teta; //proporcionalno dejstvo
	teta_ei += teta_e; //integralno dejstvo
	teta_de = teta_e - teta_de_p;
	teta_de_p = teta_de;

	if (teta_ei > teta_ei_max) {
		teta_ei = teta_ei_max;
	} else if (teta_ei < teta_ei_min) {
		teta_ei = teta_ei_min;
	}

	if (fabs(teta_e) > teta_min) {

		Vdr = -(Kp_teta * teta_e + Ki_teta * teta_ei + Kd_teta * teta_de);
		Vdl = +(Kp_teta * teta_e + Ki_teta * teta_ei + Kd_teta * teta_de);

		if (fabs(Vdr) > 100) {
				if (Vdr < 0) {
					Vdr = -100;
				} else if (Vdr > 0) {
					Vdr = 100;
				}
			}
			if (fabs(Vdl) > 100) {
				if (Vdl < 0) {
					Vdl = -100;
				} else if (Vdl > 0) {
					Vdl = 100;
				}
			}

			if (Vl < Vdl)
				Vl += 0.5;
			if (Vl > Vdl)
				Vl -= 0.5;
			if (Vr > Vdr)
				Vr -= 0.5;
			if (Vr < Vdr)
				Vr += 0.5;

			if (Vl < 0) // smer 2
					{
				faktor_ispune_t1_smer2(fabs(Vl));
			} else if (Vl > 0) { // smer 1
				faktor_ispune_t1_smer1(fabs(Vl));
			}
			if (Vr < 0) { //smer 2
				faktor_ispune_t2_smer2(fabs(Vr));
			} else if (Vr > 0) { //smer 1
				faktor_ispune_t2_smer1(fabs(Vr));
			}

	}
	else {
		prom2 = 15;
	}
}

void regulacija(void) {
	if(kreni == 1){

	if (faza == 0) {          /////////////////////////faza0

		x_e = x_d - (float)x;  //greska x, zadato - trenutno
		y_e = y_d - (float)y;

		x_el = x_e*cosf((float)teta) + y_e*sinf((float)teta);
		y_el = y_e*cosf((float)teta) - x_e*sinf((float)teta);

		if (smer == 1){
			t_ce = atan2f(y_el, x_el);
		} else if (smer == 2){
			t_ce = atan2f(-y_el, -x_el);
		}

		if (t_ce < -M_PI)
			t_ce += 2 * M_PI;
		if (t_ce > M_PI)
			t_ce -= 2 * M_PI;

		teta_e = t_ce; // te pi/2 pi/4

		teta_ei += teta_e;
		teta_de = teta_e - teta_de_p; //tde pi/2 -pi/4
		teta_de_p = teta_e;

		if (teta_ei > teta_ei_max) {
			teta_ei = teta_ei_max;
		} else if (teta_ei < teta_ei_min) {
			teta_ei = teta_ei_min;
		}

		Vdr = +(Kp_teta * teta_e + Ki_teta * teta_ei + Kd_teta * teta_de);
		Vdl = -(Kp_teta * teta_e + Ki_teta * teta_ei + Kd_teta * teta_de);

		//prosledi_pwm(Vdr, Vdl, Vr, Vl);
		acc = 0.5;

		if (fabs(teta_e) < M_PI/20)
			acc = 1;

		if (fabs(teta_e) < teta_min) {
			acc = 1;
			//medju_stanje();
			faza = 1;
			teta_ei = 0;
			d_ei = 0;
			teta_de = 0;
			d_de = 0;
			teta_de_p = teta_de;
			d_de_p = d_de;
		}

	} else if (faza == 1) { ///////////////////////////faza1////////////////////////////////////////////////////////////////////////////
		x_e = x_d - x;
		y_e = y_d - y;

		x_el = x_e*cosf(teta) + y_e*sinf(teta);
		y_el = -x_e*sinf(teta) + y_e*cosf(teta);

		if (smer == 1){
			t_ce = atan2f(y_el, x_el);
		} else if (smer == 2){
			t_ce = atan2f(-y_el, -x_el);
		}

		if (t_ce < -M_PI)
			t_ce += 2 * M_PI;
		if (t_ce > M_PI)
			t_ce -= 2 * M_PI;

		if(smer == 1){
			d_e = sqrt(x_el * x_el + y_el * y_el) / 1000; //u metrima
		} else if(smer == 2){
			d_e = -sqrt(x_el * x_el + y_el * y_el) / 1000;
		}

		if (fabs (t_ce) > (M_PI /2) )
		     // if ((theta_ce > ((M_PI/2) + teta)) || (theta_ce < (-(M_PI/2) + teta)))
			{
			      d_e *= -1;
			    if(t_ce>0)
				t_ce -= M_PI;
			      else
				t_ce += M_PI;
			 }

		teta_e = t_ce;
		teta_ei += teta_e;
		teta_de = teta_e - teta_de_p;				//tde pi/2 -pi/4
		teta_de_p = teta_e;

		d_ei += d_e;
		d_de = d_e - d_de_p;				//tde pi/2 -pi/4
		d_de_p = d_e;

		if (d_ei > d_ei_max)
			d_ei = d_ei_max;
		if (d_ei < d_ei_min)
			d_ei = d_ei_min;

		if (teta_ei > teta_ei_max) {
			teta_ei = teta_ei_max;
		} else if (teta_ei < teta_ei_min) {
			teta_ei = teta_ei_min;
		}

		Vdr = -(Kpd * d_e + Kid * d_ei + Kdd * d_de);
		Vdl = -(Kpd * d_e + Kid * d_ei + Kdd * d_de);

		acc = 0.5;

		if (fabs (d_e) < 0.1)
			acc = 1;
		//prosledi_pwm(Vdr, Vdl, Vr, Vl);
		if (fabs (d_e) > 0.1)
			{
			  // uticaj orijentacije na regulciju
			  Vdl -= 1 * (Kp_teta * teta_e + Ki_teta * teta_ei + Kd_teta * teta_de);
			  Vdr += 1 * (Kp_teta * teta_e + Ki_teta * teta_ei + Kd_teta * teta_de);
			}
		      else
			{
			  Vdl -= 0.1 * (Kp_teta * teta_e + Ki_teta * teta_ei + Kd_teta * teta_de);
			  Vdr += 0.1 * (Kp_teta * teta_e + Ki_teta * teta_ei + Kd_teta * teta_de);
			}

		if (fabs(d_e) < d_e_min) {
			acc = 1;
			//medju_stanje();
			teta_ei = 0;
			d_ei = 0;
			faza = 2;
			d_de_p = 0;
			teta_de_p = 0;
		}

	}
	else if ((faza == 2)||(faza == 3)) {

		x_e = x_d - x;
		y_e = y_d - y;

		x_el = x_e*cosf(teta) + y_e*sinf(teta);
		y_el = -x_e*sinf(teta) + y_e*cosf(teta);
		d_e = sqrt(x_el * x_el + y_el * y_el) / 1000; //u metrima

		teta_e = teta_d - teta;

		if (teta_e < -M_PI)
			teta_e += 2 * M_PI;
		if (teta_e > M_PI)
			teta_e -= 2 * M_PI;

		teta_ei += teta_e;
		teta_de = teta_e - teta_de_p; //tde pi/2 -pi/4
		teta_de_p = teta_e;

		if (teta_ei > teta_ei_max) {
			teta_ei = teta_ei_max;
		} else if (teta_ei < teta_ei_min) {
			teta_ei = teta_ei_min;
		}

		Vdr = +(Kp_teta * teta_e + Ki_teta * teta_ei + Kd_teta * teta_de);
		Vdl = -(Kp_teta * teta_e + Ki_teta * teta_ei + Kd_teta * teta_de);


		if(faza == 3)
		{
			if (x_el<0){
				d_e*=-1;
			}
			d_ei += d_e;
			d_de = d_e - d_de_p;				//tde pi/2 -pi/4
			d_de_p = d_e;

			if (d_ei > d_ei_max){
				d_ei = d_ei_max;}
			if (d_ei < d_ei_min){
				d_ei = d_ei_min;}

			Vdr += (Kpd * d_e + Kid * d_ei + Kdd * d_de);
			Vdl += (Kpd * d_e + Kid * d_ei + Kdd * d_de);

			  //medju_stanje();
			  //faza = 0;
			  //teta_ei = 0;
			  //d_ei = 0;

		}

		acc = 0.5;

		if (fabs(teta_e) < M_PI/20 && fabs(d_e) < d_e_min)
			acc = 1;


		if (fabs(teta_e) < teta_min)
			{
			  //medju_stanje();
			  faza = 3;
			  //teta_ei = 0;
			  //d_ei = 0;
			}
		else
			faza = 2;

		//prosledi_pwm(Vdr, Vdl, Vr, Vl);

		}


	if (fabs(Vdr) > 100) {
		if (Vdr < 0) {
			Vdr = -100;
		} else if (Vdr > 0) {
			Vdr = 100;
		}
	}
	if (fabs(Vdl) > 100) {
		if (Vdl < 0) {
			Vdl = -100;
		} else if (Vdl > 0) {
			Vdl = 100;
		}
	}


	if (Vl < Vdl)
		Vl += acc;
	if (Vl > Vdl)
		Vl -= acc;
	if (Vr > Vdr)
		Vr -= acc;
	if (Vr < Vdr){
		Vr += acc;}

		//Vr = 0;
		//Vl = 0;

	if (Vl < 0){ // smer 2
		faktor_ispune_t1_smer2(fabs(Vl));
	} else if (Vl > 0) { // smer 1
		faktor_ispune_t1_smer1(fabs(Vl));
	}
	if (Vr < 0) { //smer 2
		faktor_ispune_t2_smer2(fabs(Vr));
	} else if (Vr > 0) { //smer 1
		faktor_ispune_t2_smer1(fabs(Vr));
	}

	}
}

/*
 void regulacija_pozicije_x (void)
 {
 //Racunanje greske pozicije
 x_proslo = x;
 e_pozicije_x = ref_pozicija_x - x;
 ei_pozicije_x += e_pozicije_x;
 ed_pozicije_x = e_pozicije_x - e_pozicije_prosla_x;

 // Anti-windup -> Saturacija greske integralnog dejstva
 if(ei_pozicije_x > ei_pozicije_x_max)
 {
 ei_pozicije_x = ei_pozicije_x_max;
 }
 else if (ei_pozicije_x < ei_pozicije_x_min)
 {
 ei_pozicije_x = ei_pozicije_x_min;
 }

 //Racunanje brzine iz regulatora
 upravljanje_x = Kp*e_pozicije_x + Ki*ei_pozicije_x + Kd*ed_pozicije_x;

 if (upravljanje_x > 0)
 {
 //DIR = 0
 //GPIOB -> ODR &= ~(0b1 << 5);
 //faktor_ispune_ch2_tim3((uint8_t)upravljanje);
 }
 else if (upravljanje_x < 0)   // upravljanje < 0
 {
 //DIR = 1
 //GPIOB -> ODR |= (0b1 << 5);
 //faktor_ispune_ch2_tim3(-(int8_t)upravljanje);
 }
 else
 {
 //Kocenje
 }
 }

 void regulacija_pozicije_y (void)
 {
 //Racunanje greske pozicije
 y_proslo = y;
 e_pozicije_y = ref_pozicija_y - y;
 ei_pozicije_y += e_pozicije_y;
 ed_pozicije_y = e_pozicije_y - e_pozicije_prosla_y;

 // Anti-windup -> Saturacija greske integralnog dejstva
 if(ei_pozicije_y > ei_pozicije_y_max)
 {
 ei_pozicije_y = ei_pozicije_y_max;
 }
 else if (ei_pozicije_y < ei_pozicije_y_min)
 {
 ei_pozicije_y = ei_pozicije_y_min;
 }

 //Racunanje brzine iz regulatora
 upravljanje_y = Kp*e_pozicije_y + Ki*ei_pozicije_y + Kd*ed_pozicije_y;

 if (upravljanje_y > 0)
 {
 //DIR = 0
 //GPIOB -> ODR &= ~(0b1 << 5);
 //faktor_ispune_ch2_tim3((uint8_t)upravljanje);
 }
 else if (upravljanje_y < 0)   // upravljanje < 0
 {
 //DIR = 1
 //GPIOB -> ODR |= (0b1 << 5);
 //faktor_ispune_ch2_tim3(-(int8_t)upravljanje);
 }
 else
 {
 //Kocenje
 }
 } */

