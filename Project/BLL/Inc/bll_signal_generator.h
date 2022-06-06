#ifndef __BLL_WAVE_GENERATE_H_
#define __BLL_WAVE_GENERATE_H_

#include "main.h"
#include "bsp_dac.h"
#include "bll_signal_generator.h"

#define PI2 6.28318530717959 
#define NPT 1024 // DAC采样点数

void Signal_Generator_Init(void);

u16 BLL_Signal_Generator_Get_DAC_Val(void); // 返回当前DAC值

void BLL_Signal_Generator_Set_k(float temp_k); // 设置比例系数

#endif

