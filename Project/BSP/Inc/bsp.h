#ifndef __BSP_H_
#define __BSP_H_

#include "main.h"

#include "bsp_lcd.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_beep.h"
#include "bsp_adc.h"
#include "bsp_dac.h"
#include "bsp_tim.h"
#include "bsp_uart.h"

#define BSP_GetTick() HAL_GetTick()

#if defined (STM32G431xx)
#include "opamp.h"

#define ADC_MOUDLE       hadc2
#define DAC_MOUDLE       hdac3
#define DAC_TIGGLE_TIMER htim2
#define DAC_TIGGLE_TIMER_FREQ 170000000

#elif defined (STM32F103xE)
#define GPIO_SPEED_FREQ_VERY_HIGH GPIO_SPEED_FREQ_HIGH
#define ADC_MOUDLE       hadc1
#define DAC_MOUDLE       hdac
#define DAC_TIGGLE_TIMER htim2
#define DAC_TIGGLE_TIMER_FREQ 72000000
#endif


#endif
