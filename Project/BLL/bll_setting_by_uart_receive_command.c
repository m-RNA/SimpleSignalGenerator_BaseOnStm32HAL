/***************************************************/
/****************    ���ڲ���     ******************/

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

#define RX_MAX_LENTH 100 // UART�����ճ���
static vu32 SW_Timer_Tick = {0}; // �����ʱ������

u8 Rx_Index = 0; // UART���ռ�������
u8 Rx_Temp = 0;  // UART�жϽ��ջ���
u8 Rx_Buffer[RX_MAX_LENTH] = {0};  //UART���ջ���

// �������ڽ����ж�
void Receive_Uart_Command_Init(void)
{
  HAL_UART_Receive_IT(&huart1, &Rx_Temp, 1);  
}

// ���ڽ����ж����ݴ���
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
    if(SW_Timer_Tick > BSP_GetTick()) return; // �� return �� > 
    
    if(Rx_Index <= 14) // {0:20,3.0}
    {
        // ��������ʽ�Ƿ���ȷ
        if((Rx_Buffer[0] != '{') && (Rx_Buffer[2] != ':') && (Rx_Buffer[Rx_Index - 5] != ',') && (Rx_Buffer[Rx_Index - 3] != '.')&& (Rx_Buffer[Rx_Index - 1] != '}'))
            goto Send_Error;

        // ����������ݷ�Χ�Ƿ����
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
        
        // ������ȷ
        if(temp_mode == 0)
        {
            WaveOut_Flag = 0;  // �رղ������
        }
        else
        {
            WaveOut_Flag = 1;     // �򿪲������
            WaveMode = temp_mode; // �����������
        }
        
        // �ı䲨��Ƶ��
        BLL_Set_Signal_Freq(temp_freq);
        
        // �ı䲨�η��ֵ
        BLL_Set_Signal_Vpp(temp_vpp);
        
        sprintf((char*)Tx_Buffer, "Signal Generator>> Set OK.\n");
        BSP_UART_Send(Tx_Buffer);
        
        UI_Updata_Setting(); // ����LCD����
        
        BLL_Beep_On_Tick(1); // ����������

        //UART_LED_Flag = 1;  // ����LED1 250ms
        //SW_Timer_Tick[4] = 250 +  BSP_GetTick();  // ��λ�����ʱ��250ms��ʱ
    }
    else
    {
        Send_Error: // �������
        
        sprintf((char*)Tx_Buffer, "Signal Generator>> Command error.\n");
        BSP_UART_Send(Tx_Buffer);
        
        BLL_Beep_On_Tick(4); // ����������

        //UART_LED_Flag = 2;  //����LED8 250ms
        //SW_Timer_Tick[4] = 250 +  BSP_GetTick(); // ��λ�����ʱ��250ms��ʱ
    }

    // ������ջ���
    memset(Rx_Buffer, 0, Rx_Index);
    Rx_Index = 0;
}

