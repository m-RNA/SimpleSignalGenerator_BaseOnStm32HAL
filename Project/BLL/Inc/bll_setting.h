#ifndef __BLL_SETTING_H_
#define __BLL_SETTING_H_

#include "main.h"
#include "bll_setting.h"

// 返回设置索引
u8 BLL_Get_Setting_Index(void);

// 设置索引
void BLL_Set_Setting_Index(u8 Setting_Index);

// 设置输出波形的频率
void BLL_Set_Signal_Freq(u16 Freq_x10);

// 设置输出波形的峰峰值
void BLL_Set_Signal_Vpp(u16 Vpp_x10);
    
#endif

