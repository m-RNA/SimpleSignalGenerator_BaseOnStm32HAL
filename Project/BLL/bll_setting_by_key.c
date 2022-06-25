/***************************************************/
/**************    按键逻辑部分     ****************/
#include "bsp_key.h"
#include "bll.h"
#include "ui.h"
#include "tim.h"
#include "signal_generator_system.h"

static vu32 SW_Timer_Tick = 0;
static vu32 SW_Timer_KEY_Hold_Tick = 0;

static u8 KEY_Val, KEY_Down, KEY_Up, KEY_Old = 0; // 按键相关变量

// 检查按键
void KEY_Task(void)
{
    Task_Delay(75); // 每75ms 检测一次

    // 检测按键
    KEY_Val = BSP_KEY_Scan();
    KEY_Down = KEY_Val & (KEY_Val ^ KEY_Old);
    KEY_Up = ~KEY_Val & (KEY_Val ^ KEY_Old);
    KEY_Old = KEY_Val;

    if (KEY_Down)
    {
        gpUI->Reflash(); // 更新LCD界面

        gpBEEP->ON_Tick(1); // 蜂鸣器鸣叫

        SW_Timer_KEY_Hold_Tick = BSP_GetTick() + KEY_LONG_HOLD_TICK;
    }

    if (SW_Timer_KEY_Hold_Tick > BSP_GetTick())
    {
        switch (KEY_Up)
        {
        case 1: // 控制波形是否输出
            gpSignal_Generator->State.OFF_ON = !gpSignal_Generator->State.OFF_ON;
            if (gpSignal_Generator->State.OFF_ON)
                gpSignal_Generator->Set.Table_Update(); //开启输出
            else
                gpSignal_Generator->Set.OFF(); // 关闭输出
            break;

        case 2: // 改变设置选项
            if (++gpSignal_Generator->State.KEY_Setting_Index >= 3)
                gpSignal_Generator->State.KEY_Setting_Index = 0;
            break;

        case 3:
            switch (gpSignal_Generator->State.KEY_Setting_Index)
            {
            case 0: // 改变输出波形

                --gpSignal_Generator->State.Mode;

                if (gpSignal_Generator->State.Mode == Stop)
                    gpSignal_Generator->State.Mode = (WaveOut_TYPE)4;
                break;

            case 1: // 改变波形频率

                if (gpSignal_Generator->State.Freq < 10)
                    gpSignal_Generator->State.Freq++;
                else if (gpSignal_Generator->State.Freq < 100)
                    gpSignal_Generator->State.Freq += 10;
                else if (gpSignal_Generator->State.Freq < 1000)
                    gpSignal_Generator->State.Freq += 100;
                else if (gpSignal_Generator->State.Freq < 100000)
                    gpSignal_Generator->State.Freq += 1000;

                gpSignal_Generator->Set.Freq_Update();
                break;

            case 2: // 改变波形峰峰值

                gpSignal_Generator->State.Vpp_x10 += 2;
                if (gpSignal_Generator->State.Vpp_x10 >= 34)
                    gpSignal_Generator->State.Vpp_x10 = 33;

                gpSignal_Generator->Set.Vpp_Update();
                break;
            }

            gpCommunity->SendDataAllow();
            gpSignal_Generator->Set.Table_Update();
            break;

        case 4:
            switch (gpSignal_Generator->State.KEY_Setting_Index)
            {
            case 0: // 改变输出波形

                ++gpSignal_Generator->State.Mode;

                if (gpSignal_Generator->State.Mode >= (WaveOut_TYPE)5)
                    gpSignal_Generator->State.Mode = Sin;
                break;

            case 1: // 改变波形频率

                if (gpSignal_Generator->State.Freq > 1000)
                    gpSignal_Generator->State.Freq -= 1000;
                else if (gpSignal_Generator->State.Freq > 100)
                    gpSignal_Generator->State.Freq -= 100;
                else if (gpSignal_Generator->State.Freq > 10)
                    gpSignal_Generator->State.Freq -= 10;
                else if (gpSignal_Generator->State.Freq > 1)
                    gpSignal_Generator->State.Freq--;

                gpSignal_Generator->Set.Freq_Update();
                break;

            case 2: // 改变波形峰峰值

                gpSignal_Generator->State.Vpp_x10 -= 2;
                if (gpSignal_Generator->State.Vpp_x10 < 15)
                    gpSignal_Generator->State.Vpp_x10 = 15;

                gpSignal_Generator->Set.Vpp_Update();
                break;
            }

            BLL_Uart_Send_Data_Allow();
            BLL_DAC_Table_Update();
            break;
        }
    }
    else
    {
        switch (KEY_Val)
        {
        case 3:

            switch (gpSignal_Generator->State.KEY_Setting_Index)
            {
            case 1: // 改变波形频率

                if (gpSignal_Generator->State.Freq < 10)
                    gpSignal_Generator->State.Freq++;
                else if (gpSignal_Generator->State.Freq < 100)
                    gpSignal_Generator->State.Freq += 10;
                else if (gpSignal_Generator->State.Freq < 1000)
                    gpSignal_Generator->State.Freq += 100;
                else if (gpSignal_Generator->State.Freq < 1000000)
                    gpSignal_Generator->State.Freq += 1000;

                gpSignal_Generator->Set.Freq_Update();
                break;
            }
            BLL_Uart_Send_Data_Allow();
            BLL_DAC_Table_Update();

            break;

        case 4:

            switch (gpSignal_Generator->State.KEY_Setting_Index)
            {

            case 1: // 改变波形频率

                if (gpSignal_Generator->State.Freq > 1000)
                    gpSignal_Generator->State.Freq -= 1000;
                else if (gpSignal_Generator->State.Freq > 100)
                    gpSignal_Generator->State.Freq -= 100;
                else if (gpSignal_Generator->State.Freq > 10)
                    gpSignal_Generator->State.Freq -= 10;
                else if (gpSignal_Generator->State.Freq > 1)
                    gpSignal_Generator->State.Freq--;

                gpSignal_Generator->Set.Freq_Update();
                break;
            }
            BLL_Uart_Send_Data_Allow();
            BLL_DAC_Table_Update();

            break;
        }
    }
}
