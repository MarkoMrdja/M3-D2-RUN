/*
 * usart1.h
 *
 *  Created on: Mar 27, 2022
 *      Author: Marko
 */

#ifndef LIB_PERIFERIJE_USART1_USART1_H_
#define LIB_PERIFERIJE_USART1_USART1_H_


#define AX_SPEED_LENGTH 5
#define LEFT 0
#define RIGHT 1
#define AX_WRITE_DATA 3
#define AX_READ_DATA  2
#define AX_GOAL_SPEED_L 32
#define AX_START 255
#define sendData(packet, length)  	(varSerial->write(packet, length))    	// Write Over Serial
#define AX_GOAL_POSITION_L 30
#define AX_REG_WRITE 4
#define AX_GOAL_LENGTH 5

#include "stm32f4xx.h"
#include <stdint.h>
#include <string.h>
#include<stdbool.h>

void
send_char (char ch);

void
send_str (char *str);


#endif /* LIB_PERIFERIJE_USART1_USART1_H_ */
