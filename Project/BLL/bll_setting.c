#include "bsp.h"
#include "bll_setting.h"
#include "bll_signal_generator.h"

static vu8 Setting_Point = 0;  // 设置项索引
static u32 DAC_Timer_ARR;      // 定时器ARR

u8 WaveOut_Flag;   // DAC输出波形开关
u8 WaveMode;       // DAC输出波形设置

u32 DAC_Wave_Freq; // 实际频率
u16 DAC_Vpp_x10;   // 实际峰峰值的10倍


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
void BLL_Set_Signal_Freq(u32 Freq)
{
    DAC_Wave_Freq = Freq;
    DAC_Timer_ARR = 170000000 / NPT / DAC_Wave_Freq;
    __HAL_TIM_SET_AUTORELOAD(&DAC_Tiggle_Timer, DAC_Timer_ARR);
}

// 设置输出波形的峰峰值
void BLL_Set_Signal_Vpp(u16 Vpp_x10)
{
    DAC_Vpp_x10 = Vpp_x10;
    BLL_Signal_Generator_Set_k(DAC_Vpp_x10 / 33.0f);
}
