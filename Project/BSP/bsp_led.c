#include "bsp_led.h"

void BSP_LED_Disp(u8 ucLED)
{
    HAL_GPIO_WritePin(GPIOC, 0xFF00,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, ucLED<<8,GPIO_PIN_RESET);
    
    HAL_GPIO_WritePin(LED_LOCK_GPIO_Port, LED_LOCK_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED_LOCK_GPIO_Port, LED_LOCK_Pin, GPIO_PIN_RESET);
}
