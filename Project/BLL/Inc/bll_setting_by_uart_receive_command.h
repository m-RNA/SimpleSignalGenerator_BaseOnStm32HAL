#ifndef __BLL_UART_RECEIVE_COMMANDH_
#define __BLL_UART_RECEIVE_COMMANDH_

#include "main.h"
#include "bll_setting_by_uart_receive_command.h"
#include "usart.h"

void Receive_Uart_Command_Init(void);
void UART_Check_Rx_Command_Task(void); // 命令接收检查任务

#endif

