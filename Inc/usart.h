/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
extern uint8_t rx_byte;
#include <stdarg.h>   // ֧�ֿɱ�������� va_list ��
#include <stdio.h>    // ֧�� vsnprintf
#include <string.h>   // ֧�� strlen
	#define UART2_TX_QUEUE_LEN 10
#define UART2_TX_BUF_SIZE  250
	typedef struct {
    char data[UART2_TX_BUF_SIZE];
} uart2_msg_t;
	
#define UART2_RX_BUFFER_SIZE 256
extern uint8_t uart2_rx_buffer[UART2_RX_BUFFER_SIZE];

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */
void uart2_dma_send(const char *buf, uint16_t len);
void USART2_UART_StartDMA(void);  // �������������� main.c ����ʽ��������
void uart2_rx_callback(uint8_t *data, uint16_t len);  // �����ص�����

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */
void uart2_printf(const char *fmt, ...);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

