#ifndef __BSP_DAC_H_
#define __BSP_DAC_H_

#include "main.h"
#include "bsp_dac.h"
#include "dac.h"

//…Ë÷√DAC÷µ
#define BSP_DAC_SetVal(val) \
    HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_1,DAC_ALIGN_12B_R,val)

#endif

