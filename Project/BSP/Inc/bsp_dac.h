#ifndef __BSP_DAC_H_
#define __BSP_DAC_H_

#include "main.h"
#include "dac.h"

////设置DAC值
//#define BSP_DAC_SetVal(val) \
//    HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_1,DAC_ALIGN_12B_R,val)
#define  BSP_DAC_STOP()   HAL_DAC_Stop_DMA(&DAC_MOUDLE,DAC_CHANNEL_1) // 关闭输出
#define  BSP_DAC_START()  HAL_DAC_Start_DMA(&DAC_MOUDLE,DAC_CHANNEL_1,(uint32_t*)DAC_Val, NPT, DAC_ALIGN_12B_R) //开启输出

#endif

