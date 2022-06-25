/***************************************************/
/****************    蜂鸣器部分     ****************/
#include "bsp_beep.h"
#include "bll_beep.h"
#include "bll.h"

static vu32 SW_Timer_Tick = 0;
static u8 BEEP_State = 0; // 蜂鸣器鸣叫状态 

void BLL_Beep_On_Tick(u8 Tick)
{
    BEEP_State = 1;
    SW_Timer_Tick = BSP_GetTick() + BEEP_TIME * Tick;
}

void BEEP_Task(void)
{
    if(BEEP_State) 
    {
        BSP_BEEP_Crtl(1); //打开蜂鸣器
        
        if(SW_Timer_Tick < BSP_GetTick())  // 计时是否结束
        {
            BSP_BEEP_Crtl(0); //关闭蜂鸣器
            BEEP_State = 0;
        }    
    }
}

