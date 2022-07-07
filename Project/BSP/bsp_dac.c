#include "bsp_dac.h"

inline void BSP_DAC_Stop(void) // 关闭输出
{
    HAL_DAC_Stop_DMA(&DAC_MOUDLE, DAC_CHANNEL_1);
}

inline void BSP_DAC_Start(u32 *Table, u16 Lenth) //开启输出
{
    HAL_DAC_Start_DMA(&DAC_MOUDLE, DAC_CHANNEL_1, Table, Lenth, DAC_ALIGN_12B_R);
}
