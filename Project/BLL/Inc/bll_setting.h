#ifndef __BLL_SETTING_H_
#define __BLL_SETTING_H_

#include "main.h"
#include "bll_setting.h"

extern u8 WaveOut_Flag;   // DAC输出波形开启关闭
extern u8 WaveMode;       // DAC输出波形设置

extern u16 DAC_Wave_Freq_x10; // 实际 频率 的10倍
extern u16 DAC_Vpp_x10;       // 实际峰峰值的10倍

u8 BLL_Set_Get_Setting_Index(void);           // 返回设置索引
void BLL_Set_Setting_Index(u8 Setting_Index); // 设置设置索引

void BLL_Set_Signal_Freq(u16 Freq_x10); // 设置输出波形的频率
void BLL_Set_Signal_Vpp(u16 Vpp_x10);   // 设置输出波形的峰峰值

    
#endif

