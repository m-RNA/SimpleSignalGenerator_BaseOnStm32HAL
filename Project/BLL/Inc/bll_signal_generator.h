#ifndef __BLL_WAVE_GENERATE_H_
#define __BLL_WAVE_GENERATE_H_

#include "main.h"

#define NPT 256 // DAC采样点数

void Signal_Generator_Init(void);

void BLL_DAC_Table_Update(void);

void BLL_Signal_Generator_Set_k(float temp_k);

#endif
