#ifndef __BLL_H_
#define __BLL_H_

#include "main.h"
#include "bsp.h"
#include "bll_signal_generator.h"
#include "bll_uart_send_data.h"
#include "bll_setting_by_uart_receive_command.h"
#include "bll_setting_by_key.h"
#include "bll_beep.h"
#include "bll_setting.h"
//#include "bll_led.h"

// 软件定时器 宏定义
#define Task_Delay(MS)\
    if(SW_Timer_Tick > BSP_GetTick()) return;\
    SW_Timer_Tick = BSP_GetTick() + MS;
    
#endif

