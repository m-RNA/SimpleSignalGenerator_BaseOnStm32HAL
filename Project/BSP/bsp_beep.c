#include "bsp_beep.h"

void BEEP_Ctrl(u8 state)
{
    if(state)
       HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);
    else
       HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET);
}



