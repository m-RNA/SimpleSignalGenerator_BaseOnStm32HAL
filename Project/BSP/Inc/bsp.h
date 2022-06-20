#ifndef __BSP_H_
#define __BSP_H_

#include "main.h"
#include "tim.h"
#include "dac.h"
#include "opamp.h"

#include "bsp_lcd.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_adc.h"
#include "bsp_beep.h"
#include "bsp_tim.h"
#include "bsp_dac.h"
#include "bsp_uart.h"

#define BSP_GetTick() HAL_GetTick()

#define DAC_MOUDLE       hdac3
#define DAC_Tiggle_Timer htim2

#endif
