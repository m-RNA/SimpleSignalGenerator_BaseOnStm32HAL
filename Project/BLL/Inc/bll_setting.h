#ifndef __BLL_SETTING_H_
#define __BLL_SETTING_H_

#include "main.h"
#include "bll_setting.h"

// ������������
u8 BLL_Get_Setting_Index(void);

// ��������
void BLL_Set_Setting_Index(u8 Setting_Index);

// ����������ε�Ƶ��
void BLL_Set_Signal_Freq(u16 Freq_x10);

// ����������εķ��ֵ
void BLL_Set_Signal_Vpp(u16 Vpp_x10);
    
#endif

