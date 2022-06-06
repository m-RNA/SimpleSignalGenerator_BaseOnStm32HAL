#ifndef __BLL_UART_SEND_DATA_H_
#define __BLL_UART_SEND_DATA_H_

#include "main.h"
#include "bll_uart_send_data.h"
#include "bsp_uart.h"

#define TX_MAX_LENTH 100 // UART最大发送长度

extern u8 Tx_Buffer[TX_MAX_LENTH];

void BLL_Uart_Send_Data_Allow(void); // 可以发送数据
void UART_Send_AD_Data_Task(void);   // 实时数据发送任务

#endif

