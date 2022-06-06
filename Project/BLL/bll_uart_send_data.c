#include "bll_uart_send_data.h"
#include "bll_signal_generator.h"
#include "bll.h"
#include "stdio.h"
#include "string.h"

u8 Tx_Buffer[TX_MAX_LENTH] = {0};  //UART发送缓存
static vu8 UART_Send_Flag = 0;    // 串口可发送标志位

// 可以发送数据
void BLL_Allow_Uart_Send_Data(void)
{
    UART_Send_Flag = 1; 
}

// 将实时输出电压数据以STAMP协议发送到上位机
void UART_Send_AD_Data_Task(void)
{
    if(!UART_Send_Flag) return;
    UART_Send_Flag = 0;
    #ifdef UART_SEND_ADC_DATA
    ADC_Val = ADC2_GetAverageVal();
    sprintf((char*)Tx_Buffer, "<%d>{AD}%5.3f\n",BSP_GetTick(), ADC_Val * 3.3f / 4096);
    #else
    sprintf((char*)Tx_Buffer, "<%d>{DA}%5.3f\n",BSP_GetTick(), BLL_Get_Signal_Generator_DAC_Val() * 3.3f / 4096);
    #endif
    BSP_UART_Send(Tx_Buffer);
}
