/***************************************************/
/**************    �����߼�����     ****************/
#include "bsp_key.h"
#include "bll.h"
#include "bll_setting.h"
#include "bll_setting_by_key.h"
#include "bll_signal_generator.h"
#include "bll_beep.h"
#include "ui.h"
#include "tim.h"

static vu32 SW_Timer_Tick = 0;
static vu32 SW_Timer_KEY_Hold_Tick = 0;
vu8 Setting_Point_In_KEY = 0;  

static u8 KEY_Val, KEY_Down, KEY_Up, KEY_Old = 0; // ������ر���

// ��鰴��
void KEY_Task(void)
{
    Task_Delay(75); // ÿ75ms ���һ��
    
    // ��ⰴ��
    KEY_Val = KEY_Scan();
    KEY_Down = KEY_Val & (KEY_Val ^ KEY_Old);
    KEY_Up = ~KEY_Val & (KEY_Val ^ KEY_Old);
    KEY_Old = KEY_Val;
    
    if(KEY_Down)
    {
        UI_Updata_Setting(); // ����LCD����
        
        BLL_Beep_On_Tick(1);  // ���������� 
        
        SW_Timer_KEY_Hold_Tick = BSP_GetTick() + KEY_LONG_HOLD_TICK;
    }
    
    if(SW_Timer_KEY_Hold_Tick > BSP_GetTick())
    {
        switch(KEY_Up)
        {
        case 1: // ���Ʋ����Ƿ����
            WaveOut_Flag = !WaveOut_Flag;
            if(WaveOut_Flag)
            {
                DAC_Table_Update();
                // HAL_DAC_Start_DMA(&hdac1,DAC_CHANNEL_1,(uint32_t*)DAC_Val, NPT, DAC_ALIGN_12B_R);//�������
            }
            else
                BSP_DAC_STOP(); // �ر����
            break;
            
        case 2: // �ı�����ѡ��
            Setting_Point_In_KEY = BLL_Set_Get_Setting_Index();
            if(++Setting_Point_In_KEY >= 3)  
                Setting_Point_In_KEY = 0;
            BLL_Set_Setting_Index(Setting_Point_In_KEY);
            break;
            
        case 3:
            switch(BLL_Set_Get_Setting_Index())
            {
            case 0: // �ı��������

                --WaveMode;  

                if (WaveMode == 0)
                    WaveMode = 4;
                break;
                
            case 1: // �ı䲨��Ƶ��
                
                if(DAC_Wave_Freq < 10)
                    DAC_Wave_Freq++;
                else if (DAC_Wave_Freq < 100)
                    DAC_Wave_Freq+=10;
                else if (DAC_Wave_Freq < 1000)
                    DAC_Wave_Freq+=100;
                else if (DAC_Wave_Freq < 100000)
                    DAC_Wave_Freq+=1000;

                BLL_Set_Signal_Freq(DAC_Wave_Freq);
                break;
                
            case 2: // �ı䲨�η��ֵ
                
                DAC_Vpp_x10+=2;
                if(DAC_Vpp_x10>=34)
                    DAC_Vpp_x10 = 33;
                
                BLL_Set_Signal_Vpp(DAC_Vpp_x10);
                break;
            }   
            
            BLL_Uart_Send_Data_Allow();
            DAC_Table_Update();  
            break;    
            
        case 4:
            switch(BLL_Set_Get_Setting_Index())
            {
            case 0: // �ı��������
                
                ++WaveMode; 
            
                if (WaveMode >= 5)
                    WaveMode = 1;
                break;
                
            case 1: // �ı䲨��Ƶ��
                
                if(DAC_Wave_Freq > 1000)
                    DAC_Wave_Freq -= 1000;
                else if(DAC_Wave_Freq > 100)
                    DAC_Wave_Freq -= 100;
                else if(DAC_Wave_Freq > 10)
                    DAC_Wave_Freq -= 10;
                else if(DAC_Wave_Freq > 1)
                    DAC_Wave_Freq--;
                
                BLL_Set_Signal_Freq(DAC_Wave_Freq);
                break;
                
            case 2: // �ı䲨�η��ֵ
                
                DAC_Vpp_x10-=2;
                if(DAC_Vpp_x10 < 15)
                    DAC_Vpp_x10 = 15;
                
                BLL_Set_Signal_Vpp(DAC_Vpp_x10);
                break;
            }  
            
            BLL_Uart_Send_Data_Allow();
            DAC_Table_Update();   
            break;    
        }
    }
    else
    {
        switch(KEY_Val)
        {
        case 3:
     
            switch(BLL_Set_Get_Setting_Index())
            {
            case 1: // �ı䲨��Ƶ��
                
                if(DAC_Wave_Freq < 10)
                    DAC_Wave_Freq++;
                else if (DAC_Wave_Freq < 100)
                    DAC_Wave_Freq+=10;
                else if (DAC_Wave_Freq < 1000)
                    DAC_Wave_Freq+=100;
                else if (DAC_Wave_Freq < 1000000)
                    DAC_Wave_Freq+=1000;
                
                BLL_Set_Signal_Freq(DAC_Wave_Freq);
                break;
            }            
            BLL_Uart_Send_Data_Allow();
            DAC_Table_Update();   

            break;    
            
        case 4:
        
            switch(BLL_Set_Get_Setting_Index())
            {
                
            case 1: // �ı䲨��Ƶ��
                
                if(DAC_Wave_Freq > 1000)
                    DAC_Wave_Freq -= 1000;
                else if(DAC_Wave_Freq > 100)
                    DAC_Wave_Freq -= 100;
                else if(DAC_Wave_Freq > 10)
                    DAC_Wave_Freq -= 10;
                else if(DAC_Wave_Freq > 1)
                    DAC_Wave_Freq--;
                
                BLL_Set_Signal_Freq(DAC_Wave_Freq);
                break;
                
            }
            BLL_Uart_Send_Data_Allow();
            DAC_Table_Update();   
            
            break;    
        }    
    }
}

