#ifndef __BLL_WAVE_GENERATE_H_
#define __BLL_WAVE_GENERATE_H_

#include "main.h"
#include "bsp_dac.h"
#include "bll_signal_generator.h"

#define PI2 6.28318530717959 
#define NPT 1024       // DAC采样点数

extern u8 WaveOut_Flag;   // DAC输出波形开启关闭
extern u8 WaveMode;       // DAC输出波形设置

extern u16 DAC_Wave_Freq_x10; // 实际 频率 的10倍
extern u16 DAC_Vpp_x10;       // 实际峰峰值的10倍

void Signal_Generator_Init(void);
u16 BLL_Get_Signal_Generator_DAC_Val(void);

#endif

