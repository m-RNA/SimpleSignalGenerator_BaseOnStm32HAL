#include "bll_setting.h"
#include "bll_signal_generator.h"
#include "tim.h"

static vu8 Setting_Point = 0;  // ����������
extern u16 DAC_Timer_ARR;      // ��ʱ��ARR
extern float k;   // ����ϵ�������Ʒ��ֵ�м������

// ������������
u8 BLL_Get_Setting_Index(void)
{
    return Setting_Point;
}

// ��������
void BLL_Set_Setting_Index(u8 Setting_Index)
{
    Setting_Point = Setting_Index;
}

// ����������ε�Ƶ��
void BLL_Set_Signal_Freq(u16 Freq_x10)
{
    DAC_Wave_Freq_x10 = Freq_x10;
    DAC_Timer_ARR = 1000000 * 10 / NPT / DAC_Wave_Freq_x10;
    __HAL_TIM_SET_AUTORELOAD(&htim6, DAC_Timer_ARR);
}

// ����������εķ��ֵ
void BLL_Set_Signal_Vpp(u16 Vpp_x10)
{
    DAC_Vpp_x10 = Vpp_x10;
    k = DAC_Vpp_x10 / 33.0f;
}
