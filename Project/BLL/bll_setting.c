#include "bsp.h"
#include "bll_setting.h"
#include "bll_signal_generator.h"

static vu8 Setting_Point = 0;  // ����������
static u32 DAC_Timer_ARR;      // ��ʱ��ARR

u8 WaveOut_Flag;   // DAC������ο���
u8 WaveMode;       // DAC�����������

u32 DAC_Wave_Freq; // ʵ��Ƶ��
u16 DAC_Vpp_x10;   // ʵ�ʷ��ֵ��10��


// ������������
u8 BLL_Set_Get_Setting_Index(void)
{
    return Setting_Point;
}

// ��������
void BLL_Set_Setting_Index(u8 Setting_Index)
{
    Setting_Point = Setting_Index;
}

// ����������ε�Ƶ��
void BLL_Set_Signal_Freq(u32 Freq)
{
    DAC_Wave_Freq = Freq;
    DAC_Timer_ARR = 170000000 / NPT / DAC_Wave_Freq;
    __HAL_TIM_SET_AUTORELOAD(&DAC_Tiggle_Timer, DAC_Timer_ARR);
}

// ����������εķ��ֵ
void BLL_Set_Signal_Vpp(u16 Vpp_x10)
{
    DAC_Vpp_x10 = Vpp_x10;
    BLL_Signal_Generator_Set_k(DAC_Vpp_x10 / 33.0f);
}
