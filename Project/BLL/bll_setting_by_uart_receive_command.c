/***************************************************/
/****************    串口部分     ******************/

#include "bll_setting.h"
#include "bll_setting_by_uart_receive_command.h"
#include "bll_uart_send_data.h"
#include "bll_beep.h"
#include "bll_signal_generator.h"
#include "ui.h"
#include "stdio.h"
#include "string.h"
#include "tim.h"
#include "bll.h"

#define RX_MAX_LENTH 100 // UART最大接收长度
static vu32 SW_Timer_Tick = {0}; // 软件定时器数组

u8 Rx_Index = 0; // UART接收计数索引
u8 Rx_Temp = 0;  // UART中断接收缓存
u8 Rx_Buffer[RX_MAX_LENTH] = {0};  //UART接收缓存

// 开启串口接收中断
void Receive_Uart_Command_Init(void)
{
  HAL_UART_Receive_IT(&huart1, &Rx_Temp, 1);  
}

// 串口接收中断数据处理
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(Rx_Index < RX_MAX_LENTH)
    {
        SW_Timer_Tick = BSP_GetTick() + 20;
        Rx_Buffer[Rx_Index] = Rx_Temp;
        Rx_Index++;
    }
    else
    {
        SW_Timer_Tick = BSP_GetTick() - 1;
    }
    HAL_UART_Receive_IT(&huart1, &Rx_Temp, 1);
}

void UART_Check_Rx_Command_Task(void)
{
    u16 i;
    u32 temp_freq = 0;
    u16 temp_mode, temp_vpp;
    
    if(!Rx_Index) return;
    if(SW_Timer_Tick > BSP_GetTick()) return; // ※ return 用 > 
    
    if(Rx_Index <= 14) // {0:20,3.0}
    {
        // 检查命令格式是否正确
        if((Rx_Buffer[0] != '{') && (Rx_Buffer[2] != ':') && (Rx_Buffer[Rx_Index - 5] != ',') && (Rx_Buffer[Rx_Index - 3] != '.')&& (Rx_Buffer[Rx_Index - 1] != '}'))
            goto Send_Error;

        // 检查命令数据范围是否合理
        temp_mode =  Rx_Buffer[1] - '0';
        temp_vpp  = (Rx_Buffer[Rx_Index - 4] - '0') * 10+ Rx_Buffer[Rx_Index - 2] - '0';
        for(i = 3; i < Rx_Index - 5; i++)
        {
            temp_freq*=10;
            temp_freq+=(Rx_Buffer[i] - '0');
        }
        if(temp_mode >= 5) goto Send_Error;
        if(temp_freq > 100000) goto Send_Error;
        if((temp_vpp  > 33) || (temp_vpp  < 15)) goto Send_Error;
        
        // 命令正确
        if(temp_mode == 0)
        {
            WaveOut_Flag = 0;  // 关闭波形输出
        }
        else
        {
            WaveOut_Flag = 1;     // 打开波形输出
            WaveMode = temp_mode; // 设置输出波形
        }
        
        // 改变波形频率
        BLL_Set_Signal_Freq(temp_freq);
        
        // 改变波形峰峰值
        BLL_Set_Signal_Vpp(temp_vpp);
        
        sprintf((char*)Tx_Buffer, "Signal Generator>> Set OK.\n");
        BSP_UART_Send(Tx_Buffer);
        
        UI_Updata_Setting(); // 更新LCD界面
        
        BLL_Beep_On_Tick(1); // 开启蜂鸣器

        //UART_LED_Flag = 1;  // 点亮LED1 250ms
        //SW_Timer_Tick[4] = 250 +  BSP_GetTick();  // 复位软件定时器250ms计时
    }
    else
    {
        Send_Error: // 命令错误
        
        sprintf((char*)Tx_Buffer, "Signal Generator>> Command error.\n");
        BSP_UART_Send(Tx_Buffer);
        
        BLL_Beep_On_Tick(4); // 开启蜂鸣器

        //UART_LED_Flag = 2;  //点亮LED8 250ms
        //SW_Timer_Tick[4] = 250 +  BSP_GetTick(); // 复位软件定时器250ms计时
    }

    // 清零接收缓冲
    memset(Rx_Buffer, 0, Rx_Index);
    Rx_Index = 0;
}

