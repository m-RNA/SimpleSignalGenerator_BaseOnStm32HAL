#ifndef __BSP_UART_H_
#define __BSP_UART_H_

#include "main.h"
#include "usart.h"

#define BSP_UART_Send(Data) \
    HAL_UART_Transmit(&huart1, Data, strlen((const char*)Data), 100);

#endif

