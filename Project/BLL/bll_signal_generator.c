#include "bsp.h"
#include "bll_signal_generator.h"
#include "bll_uart_send_data.h"
#include "bll_setting.h"
#include "math.h"
#include "string.h"

static u16 SinTable[NPT]; // ģ�����Ҳ����������

static u16 DAC_Val[NPT] = {0};   // DAC��ֵ

static float k = 1.0f; // ���Ʒ��ֵ�м���� ����ϵ��

//��ʼ��sin��
void SinTable_Init(void)
{
	u16 i;
	for (i = 0; i < NPT; i++)
	{
		SinTable[i] = (u16)(2080 + 1990 * sin((PI2 * i) / NPT));
	}
}

/***************************************************/
/****************    DAC����     *******************/

// ���Ҳ�
void SinWaveOut(void)
{
    u16 i;
	for(i = 0; i < NPT; i++)
    {
        DAC_Val[i] = k * SinTable[i];
    }
}

// ����
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

// ���ǲ�
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

// ��ݲ�
void SawtoothWaveOut(void)
{
    u16 i;
	for(i = 0; i < NPT; i++)
    {
        DAC_Val[i] =  i * 4096 / NPT * k;
    }
}

// ֹͣ���
void StophWaveOut(void)
{
    memset(DAC_Val, 0, sizeof(DAC_Val));
}

//BSP_DAC_SetVal(DAC_Val); // ����DACֵ 
//BLL_Uart_Send_Data_Allow();// ���ڿ��Է�������

// ����DAC�����
void DAC_Table_Update(void)
{
    BSP_DAC_STOP(); // �ر����
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
    BSP_DAC_START(); //�������
}

void Signal_Generator_Init(void)
{
    // sin���ʼ��
    SinTable_Init();
    
    WaveOut_Flag = 0; // �ر����
    WaveMode = 1;     // ����Ϊ���Ҳ�

    // ����������εķ��ֵ
    BLL_Set_Signal_Vpp(25);
    
    // ����������ε�Ƶ��
    BLL_Set_Signal_Freq(2000); 
    HAL_OPAMP_Start(&hopamp1);    // STM32G4�ر�ģ��
    BSP_DAC_Tiggle_Timer_Start(); // ��������DAC�Ķ�ʱ��
    DAC_Table_Update(); // ����DAC���  
}

// ���ñ���ϵ��
void BLL_Signal_Generator_Set_k(float temp_k)
{
    k = temp_k;
}
