#include "bsp_dac.h"

inline void BSP_DAC_Stop(void) // �ر����
{
    HAL_DAC_Stop_DMA(&DAC_MOUDLE, DAC_CHANNEL_1);
}

inline void BSP_DAC_Start(u32 *Table, u16 Lenth) //�������
{
    HAL_DAC_Start_DMA(&DAC_MOUDLE, DAC_CHANNEL_1, Table, Lenth, DAC_ALIGN_12B_R);
}
