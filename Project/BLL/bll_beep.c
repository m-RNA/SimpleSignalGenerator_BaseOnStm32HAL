/***************************************************/
/****************    蜂鸣器部分     ****************/
#include "bll_beep.h"
#include "bll.h"

static vu32 SW_Timer_Tick = 0;
static u8 BEEP_State = 0; // 蜂鸣器鸣叫状态

static inline void BSP_BEEP_Crtl(u8 state)
{
    if (state)
        HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);
    else
        HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET);
}

static inline void BLL_Beep_On_Tick(u8 Tick)
{
    BEEP_State = 1;
    SW_Timer_Tick = BSP_GetTick() + BEEP_TIME * Tick;
}

static void BEEP_Task(void)
{
    if (BEEP_State)
    {
        BSP_BEEP_Crtl(1); //打开蜂鸣器

        if (SW_Timer_Tick < BSP_GetTick()) // 计时是否结束
        {
            BSP_BEEP_Crtl(0); //关闭蜂鸣器
            BEEP_State = 0;
        }
    }
}

BEEP_Type BEEP = {
    .Init = (void *)0,
    .Task = BEEP_Task,
    .ON_Tick = BLL_Beep_On_Tick,
    .Ctrl = BSP_BEEP_Crtl,
};
pBEEP_Type gpBEEP = &BEEP;
