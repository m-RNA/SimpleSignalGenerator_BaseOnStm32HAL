#ifndef __BLL_BEEP_H_
#define __BLL_BEEP_H_

#include "main.h"
#include "bll_beep.h"

#define BEEP_TIME 30     // ������Ĭ�����н���ʱ��ms

void BLL_Beep_On_Tick(u8 Tick);

void BEEP_Task(void);  // ����������

#endif

