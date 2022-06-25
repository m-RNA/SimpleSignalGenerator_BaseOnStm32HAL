/***************************************************/
/****************    串口部分     ******************/
#include "bll_uart_send_data.h"
#include "bll.h"
#include "stdio.h"
#include "string.h"
#include "signal_generator_system.h"

u8 Tx_Buffer[TX_MAX_LENTH] = {0}; // UART发送缓存
static vu8 UART_Send_Flag = 0;    // 串口可发送标志位

// 可以发送数据
void BLL_Uart_Send_Data_Allow(void)
{
    UART_Send_Flag = 1;
}

// 将实时输出电压数据发送到上位机
void UART_Send_AD_Data_Task(void)
{
    if (!UART_Send_Flag)
        return;
    UART_Send_Flag = 0;

    sprintf((char *)Tx_Buffer, "Signal Generator>> WaveType:%d, Frequency:%dHz, Vpp:%3.1fV\n", gpSignal_Generator->State.Mode, gpSignal_Generator->State.Freq, gpSignal_Generator->State.Vpp_x10 / 10.0f);

    BSP_UART_Send(Tx_Buffer);
}
