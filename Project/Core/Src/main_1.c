/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bsp.h"

#include "stdio.h"
#include "string.h"
#include "math.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

// �����ʱ�� �궨��
#define Task_Delay(ID, MS)\
    if(SW_Timer_Tick[ID] > HAL_GetTick()) return;\
    SW_Timer_Tick[ID] = HAL_GetTick() + MS;

#define BEEP_TIME 30     // ������Ĭ������ʱ��ms

#define TX_MAX_LENTH 100 // UART����ͳ���
#define RX_MAX_LENTH 100 // UART�����ճ���

//#define UART_SEND_ADC_DATA

#define PI2 6.28318530717959 
#define NPT 1024       // DAC��������


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
vu32 SW_Timer_Tick[10] = {0}; // �����ʱ������

u8 LED_State = 0x00;          // LED����ʾ״̬
u8 BEEP_State = 0;            // ����������״̬ 

u8 LCD_String_Buffer[21] = {0}; // LCD��ʾ�ַ�����
u8 LCD_Flash_Setting_Flag = 1;  // ˢ��LCD���ñ�־λ

u8 KEY_Val, KEY_Down, KEY_Up, KEY_Old = 0; // ������ر���

u8 Rx_Index = 0; // UART���ռ�������
u8 Rx_Temp = 0;  // UART�жϽ��ջ���
u8 Rx_Buffer[RX_MAX_LENTH] = {0};  //UART���ջ���
u8 Tx_Buffer[TX_MAX_LENTH] = {0};  //UART���ͻ���
u8 UART_Send_Flag = 0;    // ���ڿɷ��ͱ�־λ
//u8 UART_LED_Flag = 0;   // UART�������ָʾ��״̬

u8 Setting_Point = 0;  // ��������
u8 WaveOut_Flag = 0;   // DAC������ο����ر�
u8 WaveMode = 1;       // DAC�����������


u16 DAC_Timer_ARR; // ��ʱ��ARR
float k = 1.0f;   // ���Ʒ��ֵ�м���� ����ϵ��
u16 DAC_Vpp_x10 = 25;       // ʵ�ʷ��ֵ��10��
u16 DAC_Wave_Freq_x10 = 20; // ʵ��Ƶ�ʵ�10��
u16 DAC_Val = 0;       // DAC��ֵ
u16 ADC_Val = 0;       // ADC��ֵ

u16 SinTable[NPT];     //   ģ�����Ҳ����������
u16 UserSinTable[NPT]; // �Զ������Ҳ����������

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void SinTable_Init(void);  // sin���ʼ��

void KEY_Task(void);   // �����������
void BEEP_Task(void);  // ����������
void LCD_Task(void);   // LCDˢ������

void UART_Send_AD_Data_Task(void);     // ʵʱ���ݷ�������
void UART_Check_Rx_Command_Task(void); // ������ռ������


//void LED_UART_Rx_Command_State_Task(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
    
  // ��ʼ������
  k = DAC_Vpp_x10 / 33.0f;
  DAC_Timer_ARR = 1000000 * 10 / NPT / DAC_Wave_Freq_x10;
  SinTable_Init(); // sin���ʼ��
    
    
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM6_Init();
  MX_USART1_UART_Init();
  MX_ADC2_Init();
  MX_DAC1_Init();
  /* USER CODE BEGIN 2 */
  
  // ��ʼ��LCD��LED
  LCD_Init();
  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);
  LED_Disp(LED_State);

  // ��ʾ��ʼLCD����
  LCD_DrawLine(8 + 36 + 8 + 16 + 8, 0, 320, Horizontal); // ����
  LCD_DrawLine(Line8 + 8, 0, 320, Horizontal);           // ����

  LCD_DisplayChineseString(8, 319 - 36 * 2 - 4, 0, 32, 5);        // ��ʾ���� �źŷ�����
  LCD_DisplayChineseString(8 + 36 + 4, 319 - 16 * 4, 0, 16, 11);  // ��ʾ���� �ζ���  �¿���  ������
  
  LCD_DisplayChineseString(24 * 3 + 16, 319 -24 -8, 0, 24, 5);     // ��ʾ �������
  LCD_DisplayChineseString(24 * 4 + 16 + 3, 319 -24 -8, 9, 24, 5); // ��ʾ ����ѡ��
 
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  HAL_UART_Receive_IT(&huart1, &Rx_Temp, 1);  // �������ڽ����ж�
  
  __HAL_TIM_SET_AUTORELOAD(&htim6, DAC_Timer_ARR); // ��ʼ������DAC�Ķ�ʱ��ARR
  HAL_TIM_Base_Start_IT(&htim6);                   // ��������DAC�Ķ�ʱ���ж�
  HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);            // ����DAC���  
  
  while (1)
  {
      KEY_Task();  // �����������
      BEEP_Task(); // ����������
      LCD_Task();  // LCDˢ������
      
      UART_Send_AD_Data_Task();     // ʵʱ���ݷ�������
      UART_Check_Rx_Command_Task(); // ������ռ������
      
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV6;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/***************************************************/
/****************    DAC����     *******************/
//��ʼ��sin��
void SinTable_Init(void)
{
	u16 i;
	for (i = 0; i < NPT; i++)
	{
		//SinTable[i] = (u16)(2047 + 2047 * sin((PI2 * i) / NPT));
		SinTable[i] = (u16)(2047 + 1000 * sin((PI2 * i) / NPT) + 500 * sin((2 * PI2 * i) / NPT) + 250 * sin((3 * PI2 * i) / NPT));
	}
}

void UserSinTable_Init(u16 _2, u16 _3)
{
	u16 i;
	for (i = 0; i < NPT; i++)
	{
		UserSinTable[i] = (u16)(2047 + 2047 * sin((PI2 * i) / NPT));
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
    HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_1,DAC_ALIGN_12B_R,DAC_Val);//����DACֵ
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
    HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_1,DAC_ALIGN_12B_R,DAC_Val);//����DACֵ
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
    HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_1,DAC_ALIGN_12B_R,DAC_Val);//����DACֵ
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
    HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_1,DAC_ALIGN_12B_R,DAC_Val);//����DACֵ
}

void UserWaveOut(void)
{
	static u16 i = 0;
	i++;
	if (i >= NPT)
		i = 0;
    
	DAC_Val = UserSinTable[i];
    DAC_Val*=k;
    HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_1,DAC_ALIGN_12B_R,DAC_Val);//����DACֵ
}
// ֹͣ���
void StophWaveOut(void)
{
    DAC_Val = 0;
    HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_1,DAC_ALIGN_12B_R,DAC_Val);//����DACֵ 
}

// ��ʱ���жϴ���
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
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
        case 5:
            UserWaveOut();
            break;    
        }
    }
    
    UART_Send_Flag = 1; // ���Է�������
}

/***************************************************/
/****************    ���ڲ���     ******************/

// ���ڽ����ж����ݴ���
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(Rx_Index < RX_MAX_LENTH)
    {
        SW_Timer_Tick[3] = HAL_GetTick() + 20;
        Rx_Buffer[Rx_Index] = Rx_Temp;
        Rx_Index++;
    }
    else
    {
        SW_Timer_Tick[3] = HAL_GetTick() - 1;
    }
    HAL_UART_Receive_IT(&huart1, &Rx_Temp, 1);
}

// ��ʵʱ�����ѹ������STAMPЭ�鷢�͵���λ��
void UART_Send_AD_Data_Task(void)
{
    if(!UART_Send_Flag) return;
    UART_Send_Flag = 0;
    #ifdef UART_SEND_ADC_DATA
    ADC_Val = ADC2_GetAverageVal();
    sprintf((char*)Tx_Buffer, "<%d>{AD}%5.3f\n",HAL_GetTick(), ADC_Val * 3.3f / 4096);
    #else
    sprintf((char*)Tx_Buffer, "<%d>{DA}%5.3f\n",HAL_GetTick(), DAC_Val * 3.3f / 4096);
    #endif
    HAL_UART_Transmit(&huart1, Tx_Buffer, strlen((const char*)Tx_Buffer), 100);
}

void UART_Check_Rx_Command_Task(void)
{
    u16 temp_mode, temp_freq, temp_vpp;
    if(!Rx_Index) return;
    if(SW_Timer_Tick[3] > HAL_GetTick()) return; // �� return �� > 
    
    if(Rx_Index == 9) // {0:20,30}
    {
        // ��������ʽ�Ƿ���ȷ
        if((Rx_Buffer[0] != '{') && (Rx_Buffer[2] != ':') && (Rx_Buffer[5] != ',') && (Rx_Buffer[8] != '}'))
            goto Send_Error;

        // ����������ݷ�Χ�Ƿ����
        temp_mode =  Rx_Buffer[1] - '0';
        temp_freq = (Rx_Buffer[3] - '0') * 10 + Rx_Buffer[4] - '0';
        temp_vpp  = (Rx_Buffer[6] - '0') * 10 + Rx_Buffer[7] - '0';
        if(temp_mode >= 6) goto Send_Error;
        if(temp_freq > 90) goto Send_Error;
        if((temp_vpp  > 33) || (temp_vpp  < 15)) goto Send_Error;
        
        // ������ȷ


        if(temp_mode == 0)
        {
            WaveOut_Flag = 0;  // �رղ������
        }
        else
        {
            WaveOut_Flag = 1;     // �򿪲������
            WaveMode = temp_mode; // �����������
        }
        
        // �ı䲨��Ƶ��
        DAC_Wave_Freq_x10 = temp_freq;
        DAC_Timer_ARR = 1000000 * 10 / NPT / DAC_Wave_Freq_x10;
        __HAL_TIM_SET_AUTORELOAD(&htim6, DAC_Timer_ARR);
        
        // �ı䲨�η��ֵ
        DAC_Vpp_x10 = temp_vpp;
        k = DAC_Vpp_x10 / 33.0f;
        
        sprintf((char*)Tx_Buffer, "Signal Generator>> Successfully set.\n");
        HAL_UART_Transmit(&huart1, Tx_Buffer, strlen((const char*)Tx_Buffer), 100);   
        
        LCD_Flash_Setting_Flag = 1; // ����LCD����
        
        BEEP_State = 1; // BEEP����
        SW_Timer_Tick[5] = HAL_GetTick() + BEEP_TIME; // ��λ�����ʱ����ʱ

        //UART_LED_Flag = 1;  // ����LED1 250ms
        //SW_Timer_Tick[4] = 250 +  HAL_GetTick();  // ��λ�����ʱ��250ms��ʱ
    }
    else
    {
        Send_Error: // �������
        
        sprintf((char*)Tx_Buffer, "Signal Generator>> Command error.\n");
        HAL_UART_Transmit(&huart1, Tx_Buffer, strlen((const char*)Tx_Buffer), 100);  
        
        BEEP_State = 1;  // BEEP����
        SW_Timer_Tick[5] = HAL_GetTick() + (BEEP_TIME<<2);  // ��λ�����ʱ����ʱ
        
        //UART_LED_Flag = 2;  //����LED8 250ms
        //SW_Timer_Tick[4] = 250 +  HAL_GetTick(); // ��λ�����ʱ��250ms��ʱ
    }

    // ������ջ���
    Rx_Index = 0;
    memset(Rx_Buffer, 0, RX_MAX_LENTH);
}

/***************************************************/
/**************    �����߼�����     ****************/

// ��鰴��
void KEY_Task(void)
{
    Task_Delay(0, 75); // ÿ75ms ���һ��
    
    // ��ⰴ��
    KEY_Val = KEY_Scan();
    KEY_Down = KEY_Val & (KEY_Val ^ KEY_Old);
    KEY_Old = KEY_Val;
    
    if(KEY_Down)
    {
        LCD_Flash_Setting_Flag = 1;  // ����LCD����
        
        BEEP_State = 1; // ���������� 
        SW_Timer_Tick[5] = HAL_GetTick() + BEEP_TIME; // ��λ�����ʱ����ʱ
        
        switch(KEY_Down)
        {
        case 1: // ���Ʋ����Ƿ����
            WaveOut_Flag = !WaveOut_Flag;
            break;
            
        case 2: // �ı�����ѡ��
            if(++Setting_Point >= 3)  
                Setting_Point = 0;
            break;
            
        case 3:
            switch(Setting_Point)
            {
            case 0: // �ı��������

                --WaveMode;  

                if (WaveMode == 0)
                    WaveMode = 4;
                break;
                
            case 1: // �ı䲨��Ƶ��
                
                if(DAC_Wave_Freq_x10 < 10)
                    DAC_Wave_Freq_x10++;
                else if (DAC_Wave_Freq_x10 < 90)
                    DAC_Wave_Freq_x10+=10;

                DAC_Timer_ARR = 1000000 * 10 / NPT / DAC_Wave_Freq_x10;
                __HAL_TIM_SET_AUTORELOAD(&htim6, DAC_Timer_ARR);
                break;
                
            case 2: // �ı䲨�η��ֵ
                
                DAC_Vpp_x10+=2;
                if(DAC_Vpp_x10>=34)
                    DAC_Vpp_x10 = 33;
                k = DAC_Vpp_x10 / 33.0f;
                break;
            }            
            break;    
            
        case 4:
            switch(Setting_Point)
            {
            case 0: // �ı��������
                
                ++WaveMode; 
            
                if (WaveMode >= 5)
                    WaveMode = 1;
                break;
                
            case 1: // �ı䲨��Ƶ��
                
                
                if(DAC_Wave_Freq_x10 > 10)
                    DAC_Wave_Freq_x10 -= 10;
                else if(DAC_Wave_Freq_x10 > 1)
                    DAC_Wave_Freq_x10--;
                
                DAC_Timer_ARR = 1000000 * 10 / NPT / DAC_Wave_Freq_x10;
                __HAL_TIM_SET_AUTORELOAD(&htim6, DAC_Timer_ARR);
                break;
                
            case 2: // �ı䲨�η��ֵ
                
                DAC_Vpp_x10-=2;
                if(DAC_Vpp_x10 < 15)
                    DAC_Vpp_x10 = 15;
                k = DAC_Vpp_x10 / 33.0f;
                break;
                
            }             
            break;    
        }
    }
}

/***************************************************/
/****************    ����������     ****************/

void BEEP_Task(void)
{
    if(BEEP_State) 
    {
        BEEP_Ctrl(1);
        if(SW_Timer_Tick[5] < HAL_GetTick())  // ��ʱ�Ƿ����
        {
            BEEP_Ctrl(0);
            BEEP_State = 0;
        }    
    }
}

/***************************************************/
/****************    LCD����      ******************/

// ��ʾ���ο������ǹر�
#define SHOW_LCD_WAVE_ON_OR_OFF  LCD_DisplayChineseString(24 * 3 + 16, 319 -24 * 6, 5 + WaveOut_Flag * 2, 24, 2) 

// ��ʾ�������ࣺ���Ҳ�/����/���ǲ�/��ݲ�
#define SHOW_LCD_WAVE_MODE       LCD_DisplayChineseString(24 * 4 + 16 + 3, 319 -24 * 6 , 14 + (WaveMode - 1) * 3, 24, 3) 

// ��ʾ����ʵ��Ƶ��
#define SHOW_LCD_WAVE_REAL_FREQ  snprintf((char*)LCD_String_Buffer, 20, "  Frq:%3.1fHz",DAC_Wave_Freq_x10 /10.0f);  \
                                 LCD_DisplayStringLine(Line6, LCD_String_Buffer) 

// ��ʾ����ʵ�ʷ��ֵ
#define SHOW_LCD_WAVE_REAL_VPP   snprintf((char*)LCD_String_Buffer, 20, "  Vpp:%3.1fV",DAC_Vpp_x10 / 10.0f);\
                                 LCD_DisplayStringLine(Line7, LCD_String_Buffer) 

void LCD_Task(void)
{
    Task_Delay(2, 90); // ÿ90ms ˢ��һ��
    
    // �ɼ����� ����Ƿ�����
    #ifndef UART_SEND_ADC_DATA
    ADC_Val = ADC2_GetAverageVal();
    #endif
    LCD_SetTextColor(Green); // ������ɫΪ��ɫ
    snprintf((char*)LCD_String_Buffer, 20, "     ADC:%4.2fV", ADC_Val * 3.3f / 4096);
    LCD_DisplayStringLine(Line9, LCD_String_Buffer);
    LCD_SetTextColor(White); // ������ɫΪ��ɫ
    
    // ����Ƿ���Ҫ����LCD����ʾ�����ò���
    if(LCD_Flash_Setting_Flag == 0) return;
    LCD_Flash_Setting_Flag = 0;
    
    if(WaveOut_Flag)
    {    
        LCD_SetBackColor(Green); // �����������ʱ ���屳��ɫΪ��ɫ
        LCD_SetTextColor(Black); // ������ɫΪ��ɫ
    }
    else
    {
        LCD_SetBackColor(Red);   // �ر��������ʱ ���屳��ɫΪ��ɫ
        LCD_SetTextColor(White); // ������ɫΪ��ɫ
    }
    SHOW_LCD_WAVE_ON_OR_OFF; // ��ʾ�������������/�ر�
    
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);

    // ������������������Ӧ��ѡ��
    switch(Setting_Point)
    {
    case 0: // ������ʾ��������
        
        LCD_SetBackColor(Yellow);
        LCD_SetTextColor(Black);
        SHOW_LCD_WAVE_MODE;      // ��ʾ�������ࣺ���Ҳ�/����/���ǲ�/��ݲ�
        LCD_SetBackColor(Black);
        LCD_SetTextColor(White);  
        
        SHOW_LCD_WAVE_REAL_FREQ; // ��ʾ����ʵ��Ƶ��
        SHOW_LCD_WAVE_REAL_VPP;  // ��ʾ����ʵ�ʷ��ֵ
        break;
    
    case 1: // ������ʾ����ʵ��Ƶ��
        
        SHOW_LCD_WAVE_MODE; // ��ʾ�������ࣺ���Ҳ�/����/���ǲ�/��ݲ�
        
        LCD_SetBackColor(Yellow);
        LCD_SetTextColor(Black);        
        SHOW_LCD_WAVE_REAL_FREQ; // ��ʾ����ʵ��Ƶ��  
        LCD_SetBackColor(Black);
        LCD_SetTextColor(White);
        
        SHOW_LCD_WAVE_REAL_VPP;  // ��ʾ����ʵ�ʷ��ֵ
        break;
    
    case 2: // ������ʾ����ʵ�ʷ��ֵ
        
        SHOW_LCD_WAVE_MODE; // ��ʾ�������ࣺ���Ҳ�/����/���ǲ�/��ݲ�
        
        SHOW_LCD_WAVE_REAL_FREQ; // ��ʾ����ʵ��Ƶ�� 

        LCD_SetBackColor(Yellow);
        LCD_SetTextColor(Black);         
        SHOW_LCD_WAVE_REAL_VPP;  // ��ʾ����ʵ�ʷ��ֵ
        LCD_SetBackColor(Black);
        LCD_SetTextColor(White); 
        break;
    }
}

/***************************************************/
/****************    LED����      ******************/

//void LED_UART_Rx_Command_State_Task(void)
//{
//    if(UART_LED_Flag == 1) 
//    {
//        LED_Disp(0x01);
//        if(SW_Timer_Tick[4] < HAL_GetTick())
//        {
//            LED_Disp(0x00);
//            UART_LED_Flag = 0;
//        }
//    }
//    else if(UART_LED_Flag == 2) 
//    {
//        LED_Disp(0x80);
//        if(SW_Timer_Tick[4] < HAL_GetTick())
//        {
//            LED_Disp(0x00);
//            UART_LED_Flag = 0;
//        }
//    }
//}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
