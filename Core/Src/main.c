/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart6;
char prijem = 0;
int i = 0, j = 0;
uint8_t upravljanje = 0;
uint8_t prom = 15;
uint8_t prom2 = 15;
uint8_t step = 0;
extern uint32_t TIME;
extern float teta_d;
extern float teta_min, x_d, y_d, teta_ei;
extern uint8_t faza;
extern uint8_t smer;
extern float teta_ei_max, teta_ei_min;
extern float Kpd, Kid, Kdd;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART6_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */

	HAL_Init();
	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_TIM3_Init();
	MX_TIM4_Init();
	MX_TIM1_Init();
	MX_USART6_UART_Init();
	/* USER CODE BEGIN 2 */
	tim5_init();
	pid_init();

	extern uint32_t TIME;
	extern float teta_d;
	extern float teta_min, x_d, y_d, teta_ei;
	extern float teta_ei;
	extern uint8_t faza;
	extern int kreni;
	teta_min = 0.045;
	teta_d = 0; //M_PI/2.0;

	x_d = 0.0;
	y_d = 0.0;
	teta_d = 0.0;

	faza = 0;
	teta_ei = 0.0;
	upravljanje = 40;

	volatile uint8_t flag_send = 0;
	int step_init = 1;
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);

	//faktor_ispune_t2_smer1((float)upravljanje);
	//faktor_ispune_t1_smer1((float)upravljanje);
	//pokreni_traku(upravljanje);

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

	////////////////////////ugao

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */

	while (1) {

		switch (step) { //ljubicasta strana, sto 1
		case 0:
			if(step_init){
				step_init = 0;
				smer = 2;
				faza = 0;
				x_d = -688.93;
				y_d = 132.35;
				teta_d = -0.794889677;
			}
			if(faza == 3){
				step_init = 1;
				step = 1;
			}
			break;
		case 1:
			if (flag_send == 0) //ide na prvi
			{
				HAL_UART_Transmit(&huart6, (uint8_t*) "d", 1, 100);
				flag_send = 1;
			}
			if (step_init) {
				teta_min  = 0.06;
				step_init = 0;
				smer = 2;
				faza = 0;
				x_d = -920.0;
				y_d = 410.0; //prva
				teta_d = -(M_PI/2);

			}
			if (faza == 3) {
				step_init = 1;
				step = 2;
				flag_send= 0;
			}
			break;
		case 2:
			if (flag_send == 0) //meri prvi
			{
				HAL_UART_Transmit(&huart6, (uint8_t*) "e", 1, 100);
				flag_send = 1;
			}
			if (step_init) {
				step_init = 0;

				teta_min = 0.055;
				Kpd = 100; //44
				Kid = 0.165; //	0.085
				Kdd = 150000; //24k
				teta_ei_max = 110;
				teta_ei_min = -110;

				kreni = 2;
				faza = 4;
				medju_stanje();
				HAL_Delay(1000);
				flag_send = 0;
				step_init = 1;
				step = 3;
			}
			break;
		case 3:
			if (step_init){ //ide na drugi
				step_init = 0;

				kreni = 2;
				faza = 4;
				medju_stanje();
				HAL_Delay(750);

				smer = 2;
				kreni = 1;
				x_d = -925.0;
				y_d = 623.5; //druga
				teta_d = -1.555;
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 4;
			}
			break;
		case 4:
			if (flag_send == 0) //obara drugi, ide na treci
			{
				HAL_UART_Transmit(&huart6, (uint8_t*) "f", 1, 100);
				flag_send = 1;
			}
			if (step_init){
				step_init = 0;

				kreni = 2;
				faza = 4;
				medju_stanje();
				HAL_Delay(750);
				kreni = 1;

				smer = 2;
				Kid = 0.135;
				x_d = -920.0;
				y_d = 822.2; //treca 815.5
				teta_d = -1.555;
				faza = 0;
			}
			if (faza == 3){
				if(prijem == 'm'){
					step_init = 1;
					step = 6;
					prijem = 0;
					flag_send = 0;
				} else if (prijem == 'n'){
					step_init = 1;
					step = 5;
					prijem = 0;
					flag_send = 0;
				}
			}
			break;
		case 5:
			if (flag_send == 0) //nije nas, obara treci i ide na cetvrti
				{
				HAL_UART_Transmit(&huart6, (uint8_t*) "f", 1, 100);
				flag_send = 1;
				}
			if (step_init){
				step_init = 0;
				smer = 2;

				kreni = 2;
				faza = 4;
				medju_stanje();
				HAL_Delay(750);
				kreni = 1;

				Kid = 0.165;
				x_d = -920.0;
				y_d = 1010.5; //cetvrta
				teta_d = -1.55;
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 7;
				flag_send = 0;
			}
			break;
		case 6:
			if (step_init){ //nas je, ide do cetvrte
				step_init = 0;
				smer = 2;
				x_d = -920.0;
				y_d = 1010.5; //cetvrta
				teta_d = -1.55;
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 7;
				flag_send = 0;
			}
			break;
		case 7:
			if (flag_send == 0) //meri cetvrti
			{
				HAL_UART_Transmit(&huart6, (uint8_t*) "e", 1, 100);
				flag_send = 1;
			}
			if (step_init) {
				step_init = 0;
				kreni = 2;
				faza = 4;
				medju_stanje();
				HAL_Delay(1500);
				step_init = 1;

				if (prijem == 'n'){
					prijem = 0;
					step = 8;
					flag_send = 0;
				} else if(prijem == 'm'){
					prijem = 0;
					step = 11;
					flag_send = 0;
				}
			}
			break;
		case 8:
			if (step_init){ //nije nas, ide na peti
				step_init = 0;
				smer = 2;
				kreni = 1;
				x_d = -910.0;
				y_d = 1200.5; //peta
				teta_d = -1.555;
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 9;
			}
			break;
		case 9:
			if (flag_send == 0) //obara peti, ide na sesti
				{
				HAL_UART_Transmit(&huart6, (uint8_t*) "f", 1, 100);
				flag_send = 1;
				}
			if (step_init){
				step_init = 0;

				kreni = 2;
				faza = 4;
				medju_stanje();
				HAL_Delay(750);
				kreni = 1;

				smer = 2;
				x_d = -910.0;
				y_d = 1386.0; //sesta
				teta_d = -1.555;
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 10;
				flag_send = 0;
			}
			break;
		case 10:
			if (flag_send == 0) //obara sesti, ide do sedmog ('n')
				{
				HAL_UART_Transmit(&huart6, (uint8_t*) "f", 1, 100);
				flag_send = 1;
				}
			if (step_init){
				step_init = 0;

				kreni = 2;
				faza = 4;
				medju_stanje();
				HAL_Delay(750);
				kreni = 1;

				Kpd = 35; //40
				Kid = 0.13; //	0.15
				Kdd = 22000; //25k

				//Kpd = 31; //44
				//Kid = 0.11; //	0.085
				//Kdd = 20000; //24k

				smer = 2;
				x_d = -915.0; //sedma
				y_d = 1580.5;
				teta_d = -1.54;
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 101;
				flag_send = 0;
			}
			break;
		case 101:
			if (step_init){ //zavrsava sa kvadratima
				step_init = 0;

				Kpd = 35; //40
				Kid = 0.13; //	0.15
				Kdd = 22000; //25k

				//Kpd = 31; //44
				//Kid = 0.11; //	0.085
				//Kdd = 20000; //24k

				smer = 1;
				kreni = 1;
				x_d = -889.64967913063435; //gotovi kvadrati
				y_d = 1723.5882749804816; //gotovi kvadrati
				teta_d = 0;
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 121;
				flag_send = 0;
			}
			break;
		case 11:
			if (step_init){ //ide do sedmog ('m')
				step_init = 0;
				smer = 2;

				teta_min = 0.06;
				Kpd = 27; //44
				Kid = 0.2; //	0.085
				Kdd = 20000; //24k
				teta_ei_max = 110;
				teta_ei_min = -110;

				kreni = 1;
				x_d = -920.0; //sedma
				y_d = 1555.5;
				teta_d = -1.555;
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 12;
				flag_send = 0;
			}
			break;
		case 12:
			if (flag_send == 0) //obara sedmi, zavrsava sa kvadratima
				{
				HAL_UART_Transmit(&huart6, (uint8_t*) "f", 1, 100);
				flag_send = 1;
				}
			if (step_init){
				step_init = 0;

				kreni = 2;
				faza = 4;
				medju_stanje();
				HAL_Delay(750);
				kreni = 1;

				smer = 1;
				teta_min = 0.45;

				Kpd = 35; //40
				Kid = 0.13; //	0.15
				Kdd = 22000; //25k

				//Kpd = 31; //44
				//Kid = 0.11; //	0.085
				//Kdd = 20000; //24k

				x_d = -889.64967913063435; //gotovi kvadrati
				y_d = 1723.5882749804816; //gotovi kvadrati
				teta_d = 0;
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 121;
				flag_send = 0;
			}
			break;
		case 121:
			if (flag_send == 0) //zatvara mernu ruku
				{
				HAL_UART_Transmit(&huart6, (uint8_t*) "g", 1, 100);
				flag_send = 1;
				}
			if (step_init){
				step_init = 0;
				smer = 1;
				kreni = 1;
				x_d = -68.783264755755852; //
				y_d = 1425.3779902611661; //
				teta_d = -0.79460203669441398;
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 13;
				flag_send = 0;
			}
			break;
		case 13:
			if (flag_send == 0) //otvara rucice i gura u kamp
				{
				HAL_UART_Transmit(&huart6, (uint8_t*) "a", 1, 100);
				flag_send = 1;
				}
			if (step_init){
				step_init = 0;
				smer = 1;
				x_d = 233.08653425554098;
				y_d = 0.0;
				teta_d = -1.5980244544771267;
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 14;
				flag_send = 0;
			}
			break;
		case 14:
			if (step_init){ //ide na sredinu terena
				step_init = 0;
				smer = 2;
				x_d = 233.08653425554098;
				y_d = 1185.0;
				teta_d = -(M_PI/2);
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 15;
				flag_send = 0;
			}
			break;
		case 15:
			if (step_init){ //pozicija pre iskopa
				step_init = 0;
				smer = 1;
				x_d = -282.6;
				y_d = 1116.9;
				teta_d = -1.934445591410622;
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 151;
				flag_send = 0;
			}
			break;
		case 151:
			if (step_init){ //gura zetone u supu
				step_init = 0;
				smer = 1;

				x_d = -795.36357318251066;
				y_d = 53.929638001396214;
				teta_d = -2.3770949414320057;

				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 16;
				flag_send = 0;
			}
			break;
		case 16:
			if (flag_send == 0) //zatvara rucice i staje
				{
				HAL_UART_Transmit(&huart6, (uint8_t*) "b", 1, 200);
				flag_send = 1;
				}
			if (step_init){
				step_init = 0;

				kreni = 2;
				faza = 4;
				medju_stanje();
				HAL_Delay(1500);
				step_init = 1;
				step = 17;
				flag_send = 0;

				//smer = 2;
				//x_d = -584.54;
				//y_d = 270.536;
				//teta_d = -2.3842854956674273;
				//faza = 0;
			}
			break;
		case 17:
			if (flag_send == 0) //izbacuje repliku i stoji
				{
				HAL_UART_Transmit(&huart6, (uint8_t*) "c", 1, 200);
				flag_send = 1;
				}
			if (step_init){ //dolazi do supe
				step_init = 0;
				kreni = 2;
				faza = 4;
				medju_stanje();
				HAL_Delay(1500);
				step_init = 1;
				step = 18;
				flag_send = 0;
			}
			break;
		case 18:
			if (step_init){ //vraca se unazad
				step_init = 0;
				kreni = 1;
				smer = 2;
				x_d = 81.434081098829679;
				y_d = 655.28716262263526;
				teta_d = -2.8398777764959959;
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 19;
				flag_send = 0;
			}
			break;
		case 19:
			if (step_init){ //zavrsava
				step_init = 0;
				kreni = 1;
				smer = 2;
				x_d = 510.75763405124746;
				y_d = 189.07135436795926;
				teta_d = 2.4549445368059182;
				faza = 0;
			}
			if (faza == 3){
				step_init = 2;
				step = 20;
				flag_send = 0;
			}
			break;
		case 20:
			kreni = 2;
			faza = 4;
			medju_stanje();
			break;
		}


		/*switch (step) { //zuta strana, sto 2
				case 0:
					if(step_init){
						step_init = 0;
						smer = 2;
						faza = 0;
						x_d = -247.27348704650433;
						y_d = -326.98518802001365;
						teta_d = M_PI/2;
					}
					if(faza == 3){
						step_init = 1;
						step = 111;
					}
					break;
				case 111:
					if(step_init){
						step_init = 0;
						smer = 1;
						faza = 0;
						x_d = -611.0;
						y_d = -256.6;
						teta_d = -2.556187280063817;
					}
					if(faza == 3){
						step_init = 1;
						step = 1;
					}
					break;
				case 1:
					if (flag_send == 0) //ide na prvi
					{
						HAL_UART_Transmit(&huart6, (uint8_t*) "d", 1, 100);
						flag_send = 1;
					}
					if (step_init) {
						teta_min  = 0.06;
						step_init = 0;
						smer = 1;
						faza = 0;
						x_d = -900.0;
						y_d = -370.0; //prva
						teta_d = -(M_PI/2);

					}
					if (faza == 3) {
						step_init = 1;
						step = 2;
						flag_send= 0;
					}
					break;
				case 2:
					if (flag_send == 0) //meri prvi
					{
						HAL_UART_Transmit(&huart6, (uint8_t*) "y", 1, 100);
						flag_send = 1;
					}
					if (step_init) {
						step_init = 0;

						teta_min = 0.055;
						Kpd = 100; //44
						Kid = 0.165; //	0.085
						Kdd = 150000; //24k
						teta_ei_max = 110;
						teta_ei_min = -110;

						//Kpd = 145;
						//Kid = 0.16;
						//Kdd = 170000;
						//teta_ei_max = 110; //140
						//teta_ei_min = -110;

						kreni = 2;
						faza = 4;
						medju_stanje();
						HAL_Delay(1500);
						flag_send = 0;
						step_init = 1;
						step = 3;
					}
					break;
				case 3:
					if (step_init){ //ide na drugi
						step_init = 0;
						smer = 1;
						kreni = 1;
						x_d = -900.0;
						y_d = -565.0; //druga
						teta_d = -1.555; //-1.555
						faza = 0;
					}
					if (faza == 3){
						step_init = 1;
						step = 4;
					}
					break;
				case 4:
					if (flag_send == 0) //obara drugi, ide na treci
					{
						HAL_UART_Transmit(&huart6, (uint8_t*) "f", 1, 100);
						flag_send = 1;
					}
					if (step_init){
						step_init = 0;

						kreni = 2;
						faza = 4;
						medju_stanje();
						HAL_Delay(750);
						kreni = 1;

						smer = 1;
						Kid = 0.135;
						x_d = -900.0;
						y_d = -758.0; //treca
						teta_d = -1.555;
						faza = 0;
					}
					if (faza == 3){
						if(prijem == 'm'){
							step_init = 1;
							step = 6;
							prijem = 0;
							flag_send = 0;
						} else if (prijem == 'n'){
							step_init = 1;
							step = 5;
							prijem = 0;
							flag_send = 0;
						}
					}
					break;
				case 5:
					if (flag_send == 0) //nije nas, obara treci i ide na cetvrti
						{
						HAL_UART_Transmit(&huart6, (uint8_t*) "f", 1, 100);
						flag_send = 1;
						}
					if (step_init){
						step_init = 0;

						kreni = 2;
						faza = 4;
						medju_stanje();
						HAL_Delay(750);
						kreni = 1;

						smer = 1;
						Kid = 0.165;
						x_d = -900.0;
						y_d = -990.0; //cetvrta
						teta_d = -1.55;
						faza = 0;
					}
					if (faza == 3){
						step_init = 1;
						step = 7;
						flag_send = 0;
					}
					break;
				case 6:
					if (step_init){ //nas je, ide do cetvrte
						step_init = 0;
						smer = 1;
						x_d = -900.0;
						y_d = -990.0; //cetvrta
						teta_d = -1.55;
						faza = 0;
					}
					if (faza == 3){
						step_init = 1;
						step = 7;
						flag_send = 0;
					}
					break;
				case 7:
					if (flag_send == 0) //meri cetvrti
					{
						HAL_UART_Transmit(&huart6, (uint8_t*) "y", 1, 100);
						flag_send = 1;
					}
					if (step_init) {
						step_init = 0;
						kreni = 2;
						faza = 4;
						medju_stanje();
						HAL_Delay(1500);
						step_init = 1;

						if (prijem == 'n'){
							prijem = 0;
							step = 8;
							flag_send = 0;
						} else if(prijem == 'm'){
							prijem = 0;
							step = 11;
							flag_send = 0;
						}
					}
					break;
				case 8:
					if (step_init){ //nije nas, ide na peti
						step_init = 0;
						smer = 1;
						kreni = 1;
						x_d = -880.0;
						y_d = -1150.0; //peta
						teta_d = -1.555;
						faza = 0;
					}
					if (faza == 3){
						step_init = 1;
						step = 9;
					}
					break;
				case 9:
					if (flag_send == 0) //obara peti, ide na sesti
						{
						HAL_UART_Transmit(&huart6, (uint8_t*) "f", 1, 100);
						flag_send = 1;
						}
					if (step_init){
						step_init = 0;

						kreni = 2;
						faza = 4;
						medju_stanje();
						HAL_Delay(750);
						kreni = 1;

						smer = 1;
						x_d = -880.0;
						y_d = -1350.0; //sesta
						teta_d = -1.555;
						faza = 0;
					}
					if (faza == 3){
						step_init = 1;
						step = 10;
						flag_send = 0;
					}
					break;
				case 10:
					if (flag_send == 0) //obara sesti, ide do sedmog ('n')
						{
						HAL_UART_Transmit(&huart6, (uint8_t*) "f", 1, 100);
						flag_send = 1;
						}
					if (step_init){
						step_init = 0;

						kreni = 2;
						faza = 4;
						medju_stanje();
						HAL_Delay(750);
						kreni = 1;

						Kpd = 35; //40
						Kid = 0.13; //	0.15
						Kdd = 22000; //25k

						//Kpd = 31; //44
						//Kid = 0.11; //	0.085
						//Kdd = 20000; //24k

						smer = 1;
						x_d = -880.0; //sedma
						y_d = -1500.0;
						teta_d = -1.555;
						faza = 0;
					}
					if (faza == 3){
						step_init = 1;
						step = 101;
						flag_send = 0;
					}
					break;
				case 101:
					if (step_init){ //zavrsava sa kvadratima
						step_init = 0;

						Kpd = 35; //40
						Kid = 0.13; //	0.15
						Kdd = 22000; //25k

						//Kpd = 31; //44
						//Kid = 0.11; //	0.085
						//Kdd = 20000; //24k

						smer = 1;
						kreni = 1;
						x_d = -790.5; //gotovi kvadrati
						y_d = -1737.1; //gotovi kvadrati
						teta_d = 0.11984212908712522;
						faza = 0;
					}
					if (faza == 3){
						step_init = 1;
						step = 121; //121
						flag_send = 0;
					}
					break;
				case 11:
					if (step_init){ //ide do sedmog ('m')
						step_init = 0;
						smer = 1;

						teta_min = 0.06;
						Kpd = 27; //44
						Kid = 0.2; //	0.085
						Kdd = 20000; //24k
						teta_ei_max = 110;
						teta_ei_min = -110;

						kreni = 1;
						x_d = -880.0; //sedma
						y_d = -1540.0;
						teta_d = -1.555;
						faza = 0;
					}
					if (faza == 3){
						step_init = 1;
						step = 12;
						flag_send = 0;
					}
					break;
				case 12:
					if (flag_send == 0) //obara sedmi, zavrsava sa kvadratima
						{
						HAL_UART_Transmit(&huart6, (uint8_t*) "f", 1, 100);
						flag_send = 1;
						}
					if (step_init){
						step_init = 0;

						kreni = 2;
						faza = 4;
						medju_stanje();
						HAL_Delay(750);
						kreni = 1;

						smer = 1;
						teta_min = 0.45;

						Kpd = 35; //40
						Kid = 0.13; //	0.15
						Kdd = 22000; //25k

						//Kpd = 31; //44
						//Kid = 0.11; //	0.085
						//Kdd = 20000; //24k

						x_d = -790.5; //gotovi kvadrati
						y_d = -1737.1; //gotovi kvadrati
						teta_d = 0.11984212908712522;
						faza = 0;
					}
					if (faza == 3){
						step_init = 1;
						step = 121; //121
						flag_send = 0;
					}
					break;
				case 121:
					if (flag_send == 0) //zatvara mernu ruku
						{
						HAL_UART_Transmit(&huart6, (uint8_t*) "g", 1, 100);
						flag_send = 1;
						}
					if (step_init){
						step_init = 0;
						smer = 1;
						kreni = 1;
						x_d = 161.16170210700122;
						y_d = -1270.4307438898545;
						teta_d = 1.5360898783298822;
						faza = 0;
					}
					if (faza == 3){
						step_init = 1;
						step = 13;
						flag_send = 0;
					}
					break;
				case 13:
					if (flag_send == 0) //otvara rucice i gura u kamp
						{
						HAL_UART_Transmit(&huart6, (uint8_t*) "a", 1, 100);
						flag_send = 1;
						}
					if (step_init){
						step_init = 0;
						smer = 1;
						x_d = 178.86453406299776;
						y_d = 0.0;
						teta_d = M_PI/2;
						faza = 0;
					}
					if (faza == 3){
						step_init = 1;
						step = 14;
						flag_send = 0;
					}
					break;
				case 14:
					if (step_init){ //ide na sredinu terena
						step_init = 0;
						smer = 2;
						x_d = 178.86453406299776;
						y_d = -1270.5019829186729;
						teta_d = M_PI/2;
						faza = 0;
					}
					if (faza == 3){
						step_init = 1;
						step = 15;
						flag_send = 0;
					}
					break;
				case 15:
					if (step_init){ //pozicija pre iskopa
						step_init = 0;
						smer = 1;
						x_d = -132.34570782162817;
						y_d = -933.73934417722842;
						teta_d = 2.3095037698774163;
						faza = 0;
					}
					if (faza == 3){
						step_init = 1;
						step = 151;
						flag_send = 0;
					}
					break;
				case 151:
					if (step_init){ //gura zetone u supu
						step_init = 0;
						smer = 1;

						x_d = -608.2588594276715;
						y_d = 61.525390773486748;
						teta_d = 2.2244637067312638;

						//x_d = -769.44181640189663;
						//y_d = 77.679425104781217;
						//teta_d = -2.3842854956674273;
						faza = 0;
					}
					if (faza == 3){
						step_init = 1;
						step = 16;
						flag_send = 0;
					}
					break;
				case 16:
					if (flag_send == 0) //zatvara rucice i staje
						{
						HAL_UART_Transmit(&huart6, (uint8_t*) "b", 1, 200);
						flag_send = 1;
						}
					if (step_init){
						step_init = 0;

						kreni = 2;
						faza = 4;
						medju_stanje();
						HAL_Delay(1500);
						step_init = 1;
						step = 17;
						flag_send = 0;

						//smer = 2;
						//x_d = -584.54;
						//y_d = 270.536;
						//teta_d = -2.3842854956674273;
						//faza = 0;
					}
					break;
				case 17:
					if (flag_send == 0) //izbacuje repliku i stoji
						{
						HAL_UART_Transmit(&huart6, (uint8_t*) "c", 1, 200);
						flag_send = 1;
						}
					if (step_init){ //dolazi do supe
						step_init = 0;
						kreni = 2;
						faza = 4;
						medju_stanje();
						HAL_Delay(1500);
						step_init = 1;
						step = 18;
						flag_send = 0;
					}
					break;
				case 18:
					if (step_init){ //vraca se unazad
						step_init = 0;
						kreni = 1;
						smer = 2;
						x_d = -214.44863326733491;
						y_d = -544.61122040530654;
						teta_d = 2.6951081788981597;
						faza = 0;
					}
					if (faza == 3){
						step_init = 1;
						step = 19;
						flag_send = 0;
					}
					break;
				case 19:
					if (step_init){ //zavrsava
						step_init = 0;
						kreni = 1;
						smer = 2;
						x_d = 668.88864066157748;
						y_d = -148.68586535069068;
						teta_d = -2.2692371495709693;
						faza = 0;
					}
					if (faza == 3){
						step_init = 2;
						step = 20;
						flag_send = 0;
					}
					break;
				case 20:
					kreni = 2;
					faza = 4;
					medju_stanje();
					break;
				}*/


		/*switch (step) { //ljubicasta strana, sto 2
		case 0:
			if(step_init){
				step_init = 0;
				smer = 2;
				faza = 0;
				x_d = -688.93;
				y_d = 132.35;
				teta_d = -0.794889677;
			}
			if(faza == 3){
				step_init = 1;
				step = 1;
			}
			break;
		case 1:
			if (flag_send == 0) //ide na prvi
			{
				HAL_UART_Transmit(&huart6, (uint8_t*) "d", 1, 100);
				flag_send = 1;
			}
			if (step_init) {
				teta_min  = 0.06;
				step_init = 0;
				smer = 2;
				faza = 0;
				x_d = -930.0;
				y_d = 430.0; //prva
				teta_d = -(M_PI/2);

			}
			if (faza == 3) {
				step_init = 1;
				step = 2;
				flag_send= 0;
			}
			break;
		case 2:
			if (flag_send == 0) //meri prvi
			{
				HAL_UART_Transmit(&huart6, (uint8_t*) "e", 1, 100);
				flag_send = 1;
			}
			if (step_init) {
				step_init = 0;

				teta_min = 0.055;
				Kpd = 100; //44
				Kid = 0.165; //	0.085
				Kdd = 150000; //24k
				teta_ei_max = 110;
				teta_ei_min = -110;

				kreni = 2;
				faza = 4;
				medju_stanje();
				HAL_Delay(1500);
				flag_send = 0;
				step_init = 1;
				step = 3;
			}
			break;
		case 3:
			if (step_init){ //ide na drugi
				step_init = 0;
				smer = 2;
				kreni = 1;
				x_d = -925.0;
				y_d = 630.5; //druga
				teta_d = -1.555;
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 4;
			}
			break;
		case 4:
			if (flag_send == 0) //obara drugi, ide na treci
			{
				HAL_UART_Transmit(&huart6, (uint8_t*) "f", 1, 100);
				flag_send = 1;
			}
			if (step_init){
				step_init = 0;

				kreni = 2;
				faza = 4;
				medju_stanje();
				HAL_Delay(750);
				kreni = 1;

				smer = 2;
				Kid = 0.135;
				x_d = -920.0;
				y_d = 820.5; //treca
				teta_d = -1.555;
				faza = 0;
			}
			if (faza == 3){
				if(prijem == 'm'){
					step_init = 1;
					step = 6;
					prijem = 0;
					flag_send = 0;
				} else if (prijem == 'n'){
					step_init = 1;
					step = 5;
					prijem = 0;
					flag_send = 0;
				}
			}
			break;
		case 5:
			if (flag_send == 0) //nije nas, obara treci i ide na cetvrti
				{
				HAL_UART_Transmit(&huart6, (uint8_t*) "f", 1, 100);
				flag_send = 1;
				}
			if (step_init){
				step_init = 0;

				kreni = 2;
				faza = 4;
				medju_stanje();
				HAL_Delay(750);
				kreni = 1;

				smer = 2;
				Kid = 0.165;
				x_d = -920.0;
				y_d = 1010.5; //cetvrta
				teta_d = -1.555;
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 7;
				flag_send = 0;
			}
			break;
		case 6:
			if (step_init){ //nas je, ide do cetvrte
				step_init = 0;
				smer = 2;
				x_d = -925.0;
				y_d = 1010.5; //cetvrta
				teta_d = -1.555;
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 7;
				flag_send = 0;
			}
			break;
		case 7:
			if (flag_send == 0) //meri cetvrti
			{
				HAL_UART_Transmit(&huart6, (uint8_t*) "e", 1, 100);
				flag_send = 1;
			}
			if (step_init) {
				step_init = 0;
				kreni = 2;
				faza = 4;
				medju_stanje();
				HAL_Delay(1500);
				step_init = 1;

				if (prijem == 'n'){
					prijem = 0;
					step = 8;
					flag_send = 0;
				} else if(prijem == 'm'){
					prijem = 0;
					step = 11;
					flag_send = 0;
				}
			}
			break;
		case 8:
			if (step_init){ //nije nas, ide na peti
				step_init = 0;
				smer = 2;
				kreni = 1;
				x_d = -920.0;
				y_d = 1200.5; //peta
				teta_d = -1.555;
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 9;
			}
			break;
		case 9:
			if (flag_send == 0) //obara peti, ide na sesti
				{
				HAL_UART_Transmit(&huart6, (uint8_t*) "f", 1, 100);
				flag_send = 1;
				}
			if (step_init){
				step_init = 0;

				kreni = 2;
				faza = 4;
				medju_stanje();
				HAL_Delay(750);
				kreni = 1;

				smer = 2;
				x_d = -920.0;
				y_d = 1386.0; //sesta
				teta_d = -1.555;
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 10;
				flag_send = 0;
			}
			break;
		case 10:
			if (flag_send == 0) //obara sesti, ide do sedmog ('n')
				{
				HAL_UART_Transmit(&huart6, (uint8_t*) "f", 1, 100);
				flag_send = 1;
				}
			if (step_init){
				step_init = 0;

				kreni = 2;
				faza = 4;
				medju_stanje();
				HAL_Delay(750);
				kreni = 1;

				Kpd = 35; //40
				Kid = 0.13; //	0.15
				Kdd = 22000; //25k

				//Kpd = 31; //44
				//Kid = 0.11; //	0.085
				//Kdd = 20000; //24k

				smer = 2;
				x_d = -920.0; //sedma
				y_d = 1580.5;
				teta_d = -1.555;
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 101;
				flag_send = 0;
			}
			break;
		case 101:
			if (step_init){ //zavrsava sa kvadratima
				step_init = 0;

				Kpd = 35; //40
				Kid = 0.13; //	0.15
				Kdd = 22000; //25k

				//Kpd = 31; //44
				//Kid = 0.11; //	0.085
				//Kdd = 20000; //24k

				smer = 1;
				kreni = 1;
				x_d = -611.22; //gotovi kvadrati
				y_d = 1272.67; //gotovi kvadrati
				teta_d = -1.1650584132075892;
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 121;
				flag_send = 0;
			}
			break;
		case 11:
			if (step_init){ //ide do sedmog ('m')
				step_init = 0;
				smer = 2;

				teta_min = 0.06;
				Kpd = 27; //44
				Kid = 0.2; //	0.085
				Kdd = 20000; //24k
				teta_ei_max = 110;
				teta_ei_min = -110;

				kreni = 1;
				x_d = -920.0; //sedma
				y_d = 1580.5;
				teta_d = -1.555;
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 12;
				flag_send = 0;
			}
			break;
		case 12:
			if (flag_send == 0) //obara sedmi, zavrsava sa kvadratima
				{
				HAL_UART_Transmit(&huart6, (uint8_t*) "f", 1, 100);
				flag_send = 1;
				}
			if (step_init){
				step_init = 0;

				kreni = 2;
				faza = 4;
				medju_stanje();
				HAL_Delay(750);
				kreni = 1;

				smer = 1;
				teta_min = 0.45;

				Kpd = 35; //40
				Kid = 0.13; //	0.15
				Kdd = 22000; //25k

				//Kpd = 31; //44
				//Kid = 0.11; //	0.085
				//Kdd = 20000; //24k

				x_d = -611.22; //gotovi kvadrati
				y_d = 1272.67; //gotovi kvadrati
				teta_d = -1.1650584132075892;
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 121;
				flag_send = 0;
			}
			break;
		case 121:
			if (flag_send == 0) //zatvara mernu ruku
				{
				HAL_UART_Transmit(&huart6, (uint8_t*) "g", 1, 100);
				flag_send = 1;
				}
			if (step_init){
				step_init = 0;
				smer = 1;
				kreni = 1;
				x_d = 295.18486230093748; //
				y_d = 917.91638100561397; //
				teta_d = -1.8285051331840805;
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 13;
				flag_send = 0;
			}
			break;
		case 13:
			if (flag_send == 0) //otvara rucice i gura u kamp
				{
				HAL_UART_Transmit(&huart6, (uint8_t*) "a", 1, 100);
				flag_send = 1;
				}
			if (step_init){
				step_init = 0;
				smer = 1;
				x_d = 295.18486230093748;
				y_d = 0.0;
				teta_d = -(M_PI/2);
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 14;
				flag_send = 0;
			}
			break;
		case 14:
			if (step_init){ //ide na sredinu terena
				step_init = 0;
				smer = 2;
				x_d = 100.0;
				y_d = 1185.0;
				teta_d = -(M_PI/2);
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 15;
				flag_send = 0;
			}
			break;
		case 15:
			if (step_init){ //pozicija pre iskopa
				step_init = 0;
				smer = 1;
				x_d = -282.6;
				y_d = 1116.9;
				teta_d = -1.934445591410622;
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 151;
				flag_send = 0;
			}
			break;
		case 151:
			if (step_init){ //gura zetone u supu
				step_init = 0;
				smer = 1;

				x_d = -758.82751498497134;
				y_d = 83.98769502231238;
				teta_d = -2.3683704166178359;

				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 16;
				flag_send = 0;
			}
			break;
		case 16:
			if (flag_send == 0) //zatvara rucice i staje
				{
				HAL_UART_Transmit(&huart6, (uint8_t*) "b", 1, 200);
				flag_send = 1;
				}
			if (step_init){
				step_init = 0;

				kreni = 2;
				faza = 4;
				medju_stanje();
				HAL_Delay(1500);
				step_init = 1;
				step = 17;
				flag_send = 0;

				//smer = 2;
				//x_d = -584.54;
				//y_d = 270.536;
				//teta_d = -2.3842854956674273;
				//faza = 0;
			}
			break;
		case 17:
			if (flag_send == 0) //izbacuje repliku i stoji
				{
				HAL_UART_Transmit(&huart6, (uint8_t*) "c", 1, 200);
				flag_send = 1;
				}
			if (step_init){ //dolazi do supe
				step_init = 0;
				kreni = 2;
				faza = 4;
				medju_stanje();
				HAL_Delay(1500);
				step_init = 1;
				step = 18;
				flag_send = 0;
			}
			break;
		case 18:
			if (step_init){ //vraca se unazad
				step_init = 0;
				kreni = 1;
				smer = 2;
				x_d = 81.434081098829679;
				y_d = 655.28716262263526;
				teta_d = -2.8398777764959959;
				faza = 0;
			}
			if (faza == 3){
				step_init = 1;
				step = 19;
				flag_send = 0;
			}
			break;
		case 19:
			if (step_init){ //zavrsava
				step_init = 0;
				kreni = 1;
				smer = 2;
				x_d = 510.75763405124746;
				y_d = 189.07135436795926;
				teta_d = 2.4549445368059182;
				faza = 0;
			}
			if (faza == 3){
				step_init = 2;
				step = 20;
				flag_send = 0;
			}
			break;
		case 20:
			kreni = 2;
			faza = 4;
			medju_stanje();
			break;
		}*/

	}

	/* USER CODE END WHILE */

	/* USER CODE BEGIN 3 */
}
/* USER CODE END 3 */

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 4;
	RCC_OscInitStruct.PLL.PLLN = 84;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief TIM1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM1_Init(void) {

	/* USER CODE BEGIN TIM1_Init 0 */

	/* USER CODE END TIM1_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };
	TIM_OC_InitTypeDef sConfigOC = { 0 };
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = { 0 };

	/* USER CODE BEGIN TIM1_Init 1 */

	/* USER CODE END TIM1_Init 1 */
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 0;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 4200;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim1) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&htim1) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1)
			!= HAL_OK) {
		Error_Handler();
	}
	if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2)
			!= HAL_OK) {
		Error_Handler();
	}
	if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3)
			!= HAL_OK) {
		Error_Handler();
	}
	if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4)
			!= HAL_OK) {
		Error_Handler();
	}
	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime = 0;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM1_Init 2 */

	/* USER CODE END TIM1_Init 2 */
	HAL_TIM_MspPostInit(&htim1);

}

/**
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void) {

	/* USER CODE BEGIN TIM3_Init 0 */

	/* USER CODE END TIM3_Init 0 */

	TIM_Encoder_InitTypeDef sConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM3_Init 1 */

	/* USER CODE END TIM3_Init 1 */
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 0;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 65535;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
	sConfig.IC1Polarity = TIM_ICPOLARITY_FALLING;
	sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC1Filter = 0;
	sConfig.IC2Polarity = TIM_ICPOLARITY_FALLING;
	sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC2Filter = 0;
	if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM3_Init 2 */

	/* USER CODE END TIM3_Init 2 */

}

/**
 * @brief TIM4 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM4_Init(void) {

	/* USER CODE BEGIN TIM4_Init 0 */

	/* USER CODE END TIM4_Init 0 */

	TIM_Encoder_InitTypeDef sConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM4_Init 1 */

	/* USER CODE END TIM4_Init 1 */
	htim4.Instance = TIM4;
	htim4.Init.Prescaler = 0;
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.Period = 65535;
	htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
	sConfig.IC1Polarity = TIM_ICPOLARITY_FALLING;
	sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC1Filter = 0;
	sConfig.IC2Polarity = TIM_ICPOLARITY_FALLING;
	sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC2Filter = 0;
	if (HAL_TIM_Encoder_Init(&htim4, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM4_Init 2 */

	/* USER CODE END TIM4_Init 2 */

}

/**
 * @brief USART6 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART6_UART_Init(void) {

	/* USER CODE BEGIN USART6_Init 0 */

	/* USER CODE END USART6_Init 0 */

	/* USER CODE BEGIN USART6_Init 1 */

	/* USER CODE END USART6_Init 1 */
	huart6.Instance = USART6;
	huart6.Init.BaudRate = 115200;
	huart6.Init.WordLength = UART_WORDLENGTH_8B;
	huart6.Init.StopBits = UART_STOPBITS_1;
	huart6.Init.Parity = UART_PARITY_NONE;
	huart6.Init.Mode = UART_MODE_TX_RX;
	huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart6.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart6) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART6_Init 2 */

	/* USER CODE END USART6_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4 | GPIO_PIN_5, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB,
			GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5,
			GPIO_PIN_RESET);

	/*Configure GPIO pins : PC0 PC1 PC2 PC3
	 PC5 PC9 */
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3
			| GPIO_PIN_5 | GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : PA4 PA5 */
	GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PB0 PB1 PB3 PB4
	 PB5 */
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_4
			| GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

