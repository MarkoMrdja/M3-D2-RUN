/*
 * usart1.c
 *
 *  Created on: Mar 27, 2022
 *      Author: Marko
 */
#include "usart1.h"

extern UART_HandleTypeDef huart6;
void send_char(char ch);

void send_str(char *str) {
	while (*str != '\0') {
		send_char(*str);
		str++;
	}
}

void send_char(char ch) {
	USART6->CR1 |= (0b1 << 3);
	USART6->DR = ch;

	while (!(USART6->SR & (0b1 << 6))) {
		__NOP();
	}
	USART6->CR1 &= ~(0b1 << 3);
	USART6->SR &= ~(0b1 << 6); //moramo ga sami resetovati
}
