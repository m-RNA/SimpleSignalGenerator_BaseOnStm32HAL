#include "bll_signal_generator.h"
#include "bll_uart_send_data.h"
#include "bll_setting.h"
#include "tim.h"
#include "math.h"
#include "string.h"

static u16 SinTable[NPT]; // 模拟正弦波输出缓存区

static u16 DAC_Val[NPT] = {0};   // DAC数值

static float k = 1.0f; // 控制峰峰值中间参数 比例系数

//初始化sin表
void SinTable_Init(void)
{
	u16 i;
	for (i = 0; i < NPT; i++)
	{
        //SinTable[i] = (u16)(2047 + 2047 * sin((PI2 * i) / NPT));
		SinTable[i] = (u16)(2080 + 1990 * sin((PI2 * i) / NPT));
	}
}

/***************************************************/
/****************    DAC部分     *******************/

// 正弦波
void SinWaveOut(void)
{
    u16 i;
	for(i = 0; i < NPT; i++)
    {
        DAC_Val[i] = k * SinTable[i];
    }
}

// 方波
void SquareWaveOut(void)
{
    u16 i;
    u16 temp = 4095 * k;
	for(i = 0; i < NPT; i++)
    {
        if (i < NPT>>1)
            DAC_Val[i] = temp;
        else
            DAC_Val[i] = 0;    
    }
}

// 三角波
void TriangularWaveOut(void)
{
    u16 i, j;
	for(i = 0, j = 0; i < NPT; i++)
    {
        DAC_Val[i] =  j * 2 * 4095 / NPT * k;
        
        if (i < NPT>>1)
            j++;
        else
            j--;       
    }
}

// 锯齿波
void SawtoothWaveOut(void)
{
    u16 i;
	for(i = 0; i < NPT; i++)
    {
        DAC_Val[i] =  i * 4096 / NPT * k;
    }
}

// 停止输出
void StophWaveOut(void)
{
    memset(DAC_Val, 0, sizeof(DAC_Val));
}

//BSP_DAC_SetVal(DAC_Val); // 设置DAC值 
//BLL_Uart_Send_Data_Allow();// 串口可以发送数据

// 更新DAC输出表
void DAC_Table_Update(void)
{
    HAL_DAC_Stop_DMA(&hdac1,DAC_CHANNEL_1); // 关闭输出
    if(WaveOut_Flag == 0)
    {
       StophWaveOut();
    }
    else
    {
        switch(WaveMode)
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
    HAL_DAC_Start_DMA(&hdac1,DAC_CHANNEL_1,(uint32_t*)DAC_Val, NPT, DAC_ALIGN_12B_R);//开启输出
}

void Signal_Generator_Init(void)
{
    // sin表初始化
    SinTable_Init();
    
    WaveOut_Flag = 0; // 关闭输出
    WaveMode = 1;     // 设置为正弦波

    // 设置输出波形的峰峰值
    BLL_Set_Signal_Vpp(25);
    
    // 设置输出波形的频率
    BLL_Set_Signal_Freq(2000); 
    
    HAL_TIM_Base_Start(&DAC_Tiggle_Timer); // 开启控制DAC的定时器
    DAC_Table_Update(); // 开启DAC输出  
}

// 设置比例系数
void BLL_Signal_Generator_Set_k(float temp_k)
{
    k = temp_k;
}
