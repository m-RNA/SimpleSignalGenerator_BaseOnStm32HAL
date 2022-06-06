#ifndef __BLL_BEEP_H_
#define __BLL_BEEP_H_

#include "main.h"
#include "bll_beep.h"

#define BEEP_TIME 30     // 蜂鸣器默认鸣叫节拍时长ms

void BLL_Beep_On_Tick(u8 Tick);

void BEEP_Task(void);  // 蜂鸣器任务

#endif

