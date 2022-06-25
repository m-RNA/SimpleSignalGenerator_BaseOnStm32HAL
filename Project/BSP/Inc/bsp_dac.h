#ifndef __BSP_DAC_H_
#define __BSP_DAC_H_

#include "main.h"
#include "bsp.h"
#include "dac.h"

void BSP_DAC_Stop(void);                   // 关闭输出
void BSP_DAC_Start(u32 *Table, u16 Lenth); //开启输出

#endif
