#ifndef __BLL_WAVE_GENERATE_H_
#define __BLL_WAVE_GENERATE_H_

#include "main.h"
#include "bsp_dac.h"
#include "bll_signal_generator.h"

#define PI2 6.28318530717959 
#define NPT 1024       // DAC��������

extern u8 WaveOut_Flag;   // DAC������ο����ر�
extern u8 WaveMode;       // DAC�����������

extern u16 DAC_Wave_Freq_x10; // ʵ�� Ƶ�� ��10��
extern u16 DAC_Vpp_x10;       // ʵ�ʷ��ֵ��10��

void Signal_Generator_Init(void);
u16 BLL_Get_Signal_Generator_DAC_Val(void);

#endif

