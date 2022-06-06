#include "bll_signal_generator.h"
#include "bll_uart_send_data.h"
#include "bll_setting.h"
#include "tim.h"
#include "math.h"

u16 SinTable[NPT];     //   ģ�����Ҳ����������

u8 WaveOut_Flag = 0;   // DAC������ο����ر�
u8 WaveMode = 1;       // DAC�����������

u16 DAC_Val = 0;    // DAC��ֵ
u16 DAC_Timer_ARR;          // ��ʱ��ARR
u16 DAC_Wave_Freq_x10 = 20; // ʵ�� Ƶ�� ��10��
u16 DAC_Vpp_x10 = 25;       // ʵ�ʷ��ֵ��10��

float k = 1.0f;   // ���Ʒ��ֵ�м���� ����ϵ��

//��ʼ��sin��
void SinTable_Init(void)
{
	u16 i;
	for (i = 0; i < NPT; i++)
	{
		SinTable[i] = (u16)(2047 + 2047 * sin((PI2 * i) / NPT));
	}
}

// ���Ҳ�
void SinWaveOut(void)
{
	static u16 i = 0;
	i++;
	if (i >= NPT)
		i = 0;
    
	DAC_Val = SinTable[i];
    DAC_Val*=k;
    BSP_DAC_SetVal(DAC_Val); // ����DACֵ
}

// ����
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
    BSP_DAC_SetVal(DAC_Val); // ����DACֵ
}

// ���ǲ�
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
    BSP_DAC_SetVal(DAC_Val); // ����DACֵ
}

// ��ݲ�
void SawtoothWaveOut(void)
{
	static u16 i = 0;
	i++;
    
	if (i >= NPT)
		i = 0;
    
	DAC_Val = i * 4;
    DAC_Val*=k;
    BSP_DAC_SetVal(DAC_Val); // ����DACֵ
}

// ֹͣ���
void StopWaveOut(void)
{
    DAC_Val = 0;
    BSP_DAC_SetVal(DAC_Val); // ����DACֵ 
}

// ��ʱ���жϴ���
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
    
    BLL_Allow_Uart_Send_Data();// ���ڿ��Է�������
}

void Signal_Generator_Init(void)
{
    // sin���ʼ��
    SinTable_Init();

    // ����������εķ��ֵ
    BLL_Set_Signal_Vpp(DAC_Vpp_x10);
    
    // ����������ε�Ƶ��
    BLL_Set_Signal_Freq(DAC_Wave_Freq_x10); 
    
    HAL_TIM_Base_Start_IT(&htim6);        // ��������DAC�Ķ�ʱ���ж�
    HAL_DAC_Start(&hdac1, DAC_CHANNEL_1); // ����DAC���
}

// ���ص�ǰDACֵ
u16 BLL_Get_Signal_Generator_DAC_Val(void)
{
    return DAC_Val;
}