#include "bll_setting.h"
#include "bll_signal_generator.h"
#include "tim.h"

static vu8 Setting_Point = 0;  // 设置项索引
extern u16 DAC_Timer_ARR;      // 定时器ARR
extern float k;   // 比例系数（控制峰峰值中间参数）

// 返回设置索引
u8 BLL_Get_Setting_Index(void)
{
    return Setting_Point;
}

// 设置索引
void BLL_Set_Setting_Index(u8 Setting_Index)
{
    Setting_Point = Setting_Index;
}

// 设置输出波形的频率
void BLL_Set_Signal_Freq(u16 Freq_x10)
{
    DAC_Wave_Freq_x10 = Freq_x10;
    DAC_Timer_ARR = 1000000 * 10 / NPT / DAC_Wave_Freq_x10;
    __HAL_TIM_SET_AUTORELOAD(&htim6, DAC_Timer_ARR);
}

// 设置输出波形的峰峰值
void BLL_Set_Signal_Vpp(u16 Vpp_x10)
{
    DAC_Vpp_x10 = Vpp_x10;
    k = DAC_Vpp_x10 / 33.0f;
}
