#ifndef __BSP_TIM_H_
#define __BSP_TIM_H_

#include "main.h"
#include "tim.h"

// 开启触发DAC的定时器
#define BSP_DAC_Tiggle_Timer_Start() \
    HAL_TIM_Base_Start(&DAC_TIGGLE_TIMER)

//设置触发DAC的定时器的ARR
#define BSP_DAC_Tiggle_Timer_Set_ARR(ARR_Val) \
    __HAL_TIM_SET_AUTORELOAD(&DAC_TIGGLE_TIMER, ARR_Val)

#endif
