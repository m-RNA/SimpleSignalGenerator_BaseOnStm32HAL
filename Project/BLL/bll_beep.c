/***************************************************/
/****************    ����������     ****************/
#include "bsp_beep.h"
#include "bll_beep.h"
#include "bll.h"

static vu32 SW_Timer_Tick = 0;
static u8 BEEP_State = 0; // ����������״̬ 

void BLL_Beep_On_Tick(u8 Tick)
{
    BEEP_State = 1;
    SW_Timer_Tick = BSP_GetTick() + BEEP_TIME * Tick;
}

void BEEP_Task(void)
{
    if(BEEP_State) 
    {
        BEEP_Ctrl(1); //�򿪷�����
        
        if(SW_Timer_Tick < BSP_GetTick())  // ��ʱ�Ƿ����
        {
            BEEP_Ctrl(0); //�رշ�����
            BEEP_State = 0;
        }    
    }
}

