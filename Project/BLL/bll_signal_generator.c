#include "bll_signal_generator.h"
#include "bll_uart_send_data.h"
#include "bll_setting.h"
#include "tim.h"
#include "math.h"

u16 SinTable[NPT];     //   模拟正弦波输出缓存区

u8 WaveOut_Flag = 0;   // DAC输出波形开启关闭
u8 WaveMode = 1;       // DAC输出波形设置

u16 DAC_Val = 0;    // DAC数值
u16 DAC_Timer_ARR;          // 定时器ARR
u16 DAC_Wave_Freq_x10 = 20; // 实际 频率 的10倍
u16 DAC_Vpp_x10 = 25;       // 实际峰峰值的10倍

float k = 1.0f;   // 控制峰峰值中间参数 比例系数

//初始化sin表
void SinTable_Init(void)
{
	u16 i;
	for (i = 0; i < NPT; i++)
	{
		SinTable[i] = (u16)(2047 + 2047 * sin((PI2 * i) / NPT));
	}
}

// 正弦波
void SinWaveOut(void)
{
	static u16 i = 0;
	i++;
	if (i >= NPT)
		i = 0;
    
	DAC_Val = SinTable[i];
    DAC_Val*=k;
    BSP_DAC_SetVal(DAC_Val); // 设置DAC值
}

// 方波
void SquareWaveOut(void)
{
	static u16 i = 0;
	i++;

	if (i >= NPT)
		i = 0;
    
	if (i > NPT>>1)
		DAC_Val = 4095;
	else
		DAC_Val = 0;    
    
    DAC_Val*=k;
    BSP_DAC_SetVal(DAC_Val); // 设置DAC值
}

// 三角波
void TriangularWaveOut(void)
{
	static u16 i = 0;
	static u8 dir = 1;
	if (dir)
		i++;
	else
		i--;
    
	if (i >= NPT>>1)
		dir = 0;
	else if (i == 1)
		dir = 1;
    
	DAC_Val =  i * 8;
    DAC_Val*=k;
    BSP_DAC_SetVal(DAC_Val); // 设置DAC值
}

// 锯齿波
void SawtoothWaveOut(void)
{
	static u16 i = 0;
	i++;
    
	if (i >= NPT)
		i = 0;
    
	DAC_Val = i * 4;
    DAC_Val*=k;
    BSP_DAC_SetVal(DAC_Val); // 设置DAC值
}

// 停止输出
void StopWaveOut(void)
{
    DAC_Val = 0;
    BSP_DAC_SetVal(DAC_Val); // 设置DAC值 
}

// 定时器中断处理
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(WaveOut_Flag == 0)
    {
       StopWaveOut();
    }
    else
    {
        switch(WaveMode)
        {
        case 0:
            StopWaveOut();
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
    
    BLL_Allow_Uart_Send_Data();// 串口可以发送数据
}

void Signal_Generator_Init(void)
{
    // sin表初始化
    SinTable_Init();

    // 设置输出波形的峰峰值
    BLL_Set_Signal_Vpp(DAC_Vpp_x10);
    
    // 设置输出波形的频率
    BLL_Set_Signal_Freq(DAC_Wave_Freq_x10); 
    
    HAL_TIM_Base_Start_IT(&htim6);        // 开启控制DAC的定时器中断
    HAL_DAC_Start(&hdac1, DAC_CHANNEL_1); // 开启DAC输出
}

// 返回当前DAC值
u16 BLL_Get_Signal_Generator_DAC_Val(void)
{
    return DAC_Val;
}