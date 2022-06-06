#include "bll_setting.h"
#include "bll_signal_generator.h"
#include "tim.h"

static vu8 Setting_Point = 0;  // 设置项索引
static u16 DAC_Timer_ARR;      // 定时器ARR

u8 WaveOut_Flag = 0;   // DAC输出波形开关
u8 WaveMode = 1;       // DAC输出波形设置

u16 DAC_Wave_Freq_x10 = 20; // 实际 频率 的10倍
u16 DAC_Vpp_x10 = 25;       // 实际峰峰值的10倍


// 返回设置索引
u8 BLL_Set_Get_Setting_Index(void)
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
    u16 temp_k;
    DAC_Vpp_x10 = Vpp_x10;
    temp_k = DAC_Vpp_x10 / 33.0f;
    BLL_Signal_Generator_Set_k(temp_k);
}
