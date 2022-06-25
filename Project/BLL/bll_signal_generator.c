#include "bll.h"
#include "bll_signal_generator.h"
#include "math.h"
#include "string.h"
#include "signal_generator_system.h"

#define PI2 6.28318530717959

static u16 SinTable[NPT]; // 模拟正弦波输出缓存区
// static u16 User_Table[5][NPT] = {0}; // 用户输出缓存区

static u16 DAC_Val[NPT] = {0}; // DAC数值

//初始化sin表
void BLL_SinTable_Init(void)
{
    u16 i;
    for (i = 0; i < NPT; i++)
    {
        SinTable[i] = (u16)(2080 + 1990 * sin((PI2 * i) / NPT));
    }
}

/**************************************************/
/**************    DAC码表部分     *****************/

// 正弦波
void SinWaveOut(void)
{
    u16 i;
    for (i = 0; i < NPT; i++)
    {
        DAC_Val[i] = gpSignal_Generator->State.k * SinTable[i];
    }
}

// 方波
void SquareWaveOut(void)
{
    u16 i;
    u16 temp = 4095 * gpSignal_Generator->State.k;
    for (i = 0; i < NPT; i++)
    {
        if (i < NPT >> 1)
            DAC_Val[i] = temp;
        else
            DAC_Val[i] = 0;
    }
}

// 三角波
void TriangularWaveOut(void)
{
    u16 i, j;
    for (i = 0, j = 0; i < NPT; i++)
    {
        DAC_Val[i] = j * 2 * 4095 / NPT * gpSignal_Generator->State.k;

        if (i < NPT >> 1)
            j++;
        else
            j--;
    }
}

// 锯齿波
void SawtoothWaveOut(void)
{
    u16 i;
    for (i = 0; i < NPT; i++)
    {
        DAC_Val[i] = i * 4096 / NPT * gpSignal_Generator->State.k;
    }
}

// 停止输出
void StophWaveOut(void)
{
    memset(DAC_Val, 0, sizeof(DAC_Val));
}

// 更新DAC输出表
void BLL_DAC_Table_Update(void)
{
    gpSignal_Generator->Set.OFF(); // 关闭输出
    if (gpSignal_Generator->State.OFF_ON == 0)
    {
        StophWaveOut();
    }
    else
    {
        switch (gpSignal_Generator->State.Mode)
        {
        case 0:
            StophWaveOut();
            break;
        case 1:
            SinWaveOut();
            break;
        case 2:
            SquareWaveOut();
            break;
        case 3:
            TriangularWaveOut();
            break;
        case 4:
            SawtoothWaveOut();
            break;
        }
    }
    gpSignal_Generator->Set.ON((uint32_t *)DAC_Val, NPT); //开启输出
}

void Signal_Generator_Init(void)
{
    // PSignal_Generator_Type pSG = Get_Signal_Generator_Index();
    //  sin表初始化
    BLL_SinTable_Init();

    gpSignal_Generator->State.OFF_ON = 0; // 关闭输出
    gpSignal_Generator->State.Mode = Sin; // 设置为正弦波

    // 设置输出波形的峰峰值
    gpSignal_Generator->State.Vpp_x10 = 25;
    gpSignal_Generator->Set.Vpp_Update();

    // 设置输出波形的频率
    gpSignal_Generator->State.Freq = 2000;
    gpSignal_Generator->Set.Freq_Update();

#ifdef __OPAMP_H__
    HAL_OPAMP_Start(&hopamp1); // STM32G4特别模块
#endif

    BSP_DAC_Tiggle_Timer_Start();           // 开启控制DAC的定时器
    gpSignal_Generator->Set.Table_Update(); // 开启DAC输出
}
