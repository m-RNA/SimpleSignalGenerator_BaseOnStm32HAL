#ifndef __BLL_BEEP_H_
#define __BLL_BEEP_H_

#include "main.h"

typedef struct
{
    void (*Init)(void);
    void (*Task)(void);
    void (*ON_Tick)(u8 Tick);
    void (*Ctrl)(u8 State);
} BEEP_Type, *pBEEP_Type;

#define BEEP_TIME 30 // 蜂鸣器默认鸣叫节拍时长ms

extern pBEEP_Type gpBEEP;

#endif
