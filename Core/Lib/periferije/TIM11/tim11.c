

/*
 * tim11.c
 *
 *  Created on: Mar 27, 2022
 *      Author: Marko
 */
#include "tim11.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern float Vdr;
extern float Vdl;
extern float Vr;
extern float Vl;

uint8_t senzor1 = 0;
uint8_t senzor2 = 0;
uint8_t senzor3 = 0;
uint8_t senzor4 = 0;
uint8_t senzor5 = 0;
uint32_t TIME = 0;


void
tim5_init (void)
{
  RCC->APB1ENR |= (0b1 << 3);

  TIM5->CR1 &= ~(0xffff);
  TIM5->CR1 |= ((0b1 << 2) | (0b1 << 7));

  TIM5->DIER |= (0b1 << 0);


  TIM5->PSC = (42 - 1);
  TIM5->ARR = (1000 - 1);

  TIM5->EGR |= (0b1 << 0);

  TIM5->CR1 |= (0b1 << 0);

  NVIC->ISER[1] |= (0b1 << 18);
}



void
TIM5_IRQHandler(void) //Ovim interaptom menjati promenjive, a promenjive proveravati (if) pa slati preko UART interapta
{
  if (TIM5->SR & (0b1 << 0))
    {
      TIM5->SR &= ~(0b1 << 0);
      TIME ++;
      sens_robot();
      regulacija();
      //regulacija_ugla();


      if ((GPIOC->IDR & (0b1 << 0)) == (0b1 << 0))
	{
    	  senzor1 = 150;
	}

      else if ((GPIOC->IDR & (0b1 << 0)) == 0)
	{
    	  senzor1 = 0;
	}

      if ((GPIOC->IDR & (0b1 << 1)) == (0b1 << 1))
	{
    	  senzor2 = 150;
	}

      else if ((GPIOC->IDR & (0b1 << 1)) == 0)
	{
    	  senzor2 = 0;
	}

      if ((GPIOC->IDR & (0b1 << 2)) == (0b1 << 2))
	{
    	  senzor3 = 150;
	}

      else if ((GPIOC->IDR & (0b1 << 2)) == 0)
	{
    	  senzor3 = 0;
	}

      if ((GPIOC->IDR & (0b1 << 3)) == (0b1 << 3))
	{
    	  senzor4 = 150;
	}

      else if ((GPIOC->IDR & (0b1 << 3)) == 0)
	{
    	  senzor4 = 0;
	}

      if (((GPIOC->IDR & (0b1 << 5))) == (0b1 << 5))
	{
    	  senzor5 = 150;
	}

      else if (((GPIOC->IDR & (0b1 << 5))) == 0)
	{
    	  senzor5 = 0;
	}

      //TESTIRANJE ///////////////////////////////////////////////////////////////////////////
      //////////////////////////////////////////////////////////////////////////////////////
    }
}

