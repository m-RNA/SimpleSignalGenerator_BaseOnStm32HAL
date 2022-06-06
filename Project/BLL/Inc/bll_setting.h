#ifndef __BLL_SETTING_H_
#define __BLL_SETTING_H_

#include "main.h"
#include "bll_setting.h"

extern u8 WaveOut_Flag;   // DAC������ο����ر�
extern u8 WaveMode;       // DAC�����������

extern u16 DAC_Wave_Freq_x10; // ʵ�� Ƶ�� ��10��
extern u16 DAC_Vpp_x10;       // ʵ�ʷ��ֵ��10��

u8 BLL_Set_Get_Setting_Index(void);           // ������������
void BLL_Set_Setting_Index(u8 Setting_Index); // ������������

void BLL_Set_Signal_Freq(u16 Freq_x10); // ����������ε�Ƶ��
void BLL_Set_Signal_Vpp(u16 Vpp_x10);   // ����������εķ��ֵ

    
#endif

