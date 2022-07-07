#include "bsp.h"
#include "bll_setting.h"
#include "bll_signal_generator.h"
#include "signal_generator_system.h"

// 设置输出波形的频率
inline void BLL_Signal_Freq_Update(void)
{
    // gpSignal_Generator->State.Freq = Freq;
    BSP_DAC_Tiggle_Timer_Set_ARR(DAC_TIGGLE_TIMER_FREQ / NPT / gpSignal_Generator->State.Freq);
}

// 设置输出波形的峰峰值
inline void BLL_Signal_Vpp_Update(void)
{
    gpSignal_Generator->State.k = gpSignal_Generator->State.Vpp_x10 / 33.0f;
    gpSignal_Generator->Set.Table_Update();
}
