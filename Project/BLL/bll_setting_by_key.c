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
vu8 Setting_Point_In_KEY = 0;  

static u8 KEY_Val, KEY_Down, KEY_Old = 0; // ������ر���

// ��鰴��
void KEY_Task(void)
{
    Task_Delay(75); // ÿ75ms ���һ��
    
    // ��ⰴ��
    KEY_Val = KEY_Scan();
    KEY_Down = KEY_Val & (KEY_Val ^ KEY_Old);
    KEY_Old = KEY_Val;
    
    if(KEY_Down)
    {
        BLL_LCD_Updata_Show_Setting(); // ����LCD����
        
        BLL_Beep_On_Tick(1);  // ���������� 
        
        switch(KEY_Down)
        {
        case 1: // ���Ʋ����Ƿ����
            WaveOut_Flag = !WaveOut_Flag;
            break;
            
        case 2: // �ı�����ѡ��
            Setting_Point_In_KEY = BLL_Get_Setting_Index();
            if(++Setting_Point_In_KEY >= 3)  
                Setting_Point_In_KEY = 0;
            BLL_Set_Setting_Index(Setting_Point_In_KEY);
            break;
            
        case 3:
            switch(BLL_Get_Setting_Index())
            {
            case 0: // �ı��������

                --WaveMode;  

                if (WaveMode == 0)
                    WaveMode = 4;
                break;
                
            case 1: // �ı䲨��Ƶ��
                
                if(DAC_Wave_Freq_x10 < 10)
                    DAC_Wave_Freq_x10++;
                else if (DAC_Wave_Freq_x10 < 90)
                    DAC_Wave_Freq_x10+=10;

                BLL_Set_Signal_Freq(DAC_Wave_Freq_x10);
                break;
                
            case 2: // �ı䲨�η��ֵ
                
                DAC_Vpp_x10+=2;
                if(DAC_Vpp_x10>=34)
                    DAC_Vpp_x10 = 33;
                
                BLL_Set_Signal_Vpp(DAC_Vpp_x10);
                break;
            }            
            break;    
            
        case 4:
            switch(BLL_Get_Setting_Index())
            {
            case 0: // �ı��������
                
                ++WaveMode; 
            
                if (WaveMode >= 5)
                    WaveMode = 1;
                break;
                
            case 1: // �ı䲨��Ƶ��
                
                if(DAC_Wave_Freq_x10 > 10)
                    DAC_Wave_Freq_x10 -= 10;
                else if(DAC_Wave_Freq_x10 > 1)
                    DAC_Wave_Freq_x10--;
                
                    BLL_Set_Signal_Freq(DAC_Wave_Freq_x10);

                break;
                
            case 2: // �ı䲨�η��ֵ
                
                DAC_Vpp_x10-=2;
                if(DAC_Vpp_x10 < 15)
                    DAC_Vpp_x10 = 15;
                
                BLL_Set_Signal_Vpp(DAC_Vpp_x10);
                break;
                
            }             
            break;    
        }
    }
}

