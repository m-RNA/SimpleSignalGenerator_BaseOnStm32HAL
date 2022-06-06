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

// 软件定时器 宏定义
#define Task_Delay(ID, MS)\
    if(SW_Timer_Tick[ID] > HAL_GetTick()) return;\
    SW_Timer_Tick[ID] = HAL_GetTick() + MS;

#define BEEP_TIME 30     // 蜂鸣器默认鸣叫时长ms

#define TX_MAX_LENTH 100 // UART最大发送长度
#define RX_MAX_LENTH 100 // UART最大接收长度

//#define UART_SEND_ADC_DATA

#define PI2 6.28318530717959 
#define NPT 1024       // DAC采样点数


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
vu32 SW_Timer_Tick[10] = {0}; // 软件定时器数组

u8 LED_State = 0x00;          // LED灯显示状态
u8 BEEP_State = 0;            // 蜂鸣器鸣叫状态 

u8 LCD_String_Buffer[21] = {0}; // LCD显示字符缓存
u8 LCD_Flash_Setting_Flag = 1;  // 刷新LCD设置标志位

u8 KEY_Val, KEY_Down, KEY_Up, KEY_Old = 0; // 按键相关变量

u8 Rx_Index = 0; // UART接收计数索引
u8 Rx_Temp = 0;  // UART中断接收缓存
u8 Rx_Buffer[RX_MAX_LENTH] = {0};  //UART接收缓存
u8 Tx_Buffer[TX_MAX_LENTH] = {0};  //UART发送缓存
u8 UART_Send_Flag = 0;    // 串口可发送标志位
//u8 UART_LED_Flag = 0;   // UART命令接收指示灯状态

u8 Setting_Point = 0;  // 设置索引
u8 WaveOut_Flag = 0;   // DAC输出波形开启关闭
u8 WaveMode = 1;       // DAC输出波形设置


u16 DAC_Timer_ARR; // 定时器ARR
float k = 1.0f;   // 控制峰峰值中间参数 比例系数
u16 DAC_Vpp_x10 = 25;       // 实际峰峰值的10倍
u16 DAC_Wave_Freq_x10 = 20; // 实际频率的10倍
u16 DAC_Val = 0;       // DAC数值
u16 ADC_Val = 0;       // ADC数值

u16 SinTable[NPT];     //   模拟正弦波输出缓存区
u16 UserSinTable[NPT]; // 自定义正弦波输出缓存区

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void SinTable_Init(void);  // sin表初始化

void KEY_Task(void);   // 按键检测任务
void BEEP_Task(void);  // 蜂鸣器任务
void LCD_Task(void);   // LCD刷新任务

void UART_Send_AD_Data_Task(void);     // 实时数据发送任务
void UART_Check_Rx_Command_Task(void); // 命令接收检查任务


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
    
  // 初始化参数
  k = DAC_Vpp_x10 / 33.0f;
  DAC_Timer_ARR = 1000000 * 10 / NPT / DAC_Wave_Freq_x10;
  SinTable_Init(); // sin表初始化
    
    
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
  
  // 初始化LCD和LED
  LCD_Init();
  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);
  LED_Disp(LED_State);

  // 显示初始LCD界面
  LCD_DrawLine(8 + 36 + 8 + 16 + 8, 0, 320, Horizontal); // 画线
  LCD_DrawLine(Line8 + 8, 0, 320, Horizontal);           // 画线

  LCD_DisplayChineseString(8, 319 - 36 * 2 - 4, 0, 32, 5);        // 显示标题 信号发生器
  LCD_DisplayChineseString(8 + 36 + 4, 319 - 16 * 4, 0, 16, 11);  // 显示姓名 段定泓  陈俊健  樊嘉晖
  
  LCD_DisplayChineseString(24 * 3 + 16, 319 -24 -8, 0, 24, 5);     // 显示 波形输出
  LCD_DisplayChineseString(24 * 4 + 16 + 3, 319 -24 -8, 9, 24, 5); // 显示 波形选择
 
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  HAL_UART_Receive_IT(&huart1, &Rx_Temp, 1);  // 开启串口接收中断
  
  __HAL_TIM_SET_AUTORELOAD(&htim6, DAC_Timer_ARR); // 初始化控制DAC的定时器ARR
  HAL_TIM_Base_Start_IT(&htim6);                   // 开启控制DAC的定时器中断
  HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);            // 开启DAC输出  
  
  while (1)
  {
      KEY_Task();  // 按键检测任务
      BEEP_Task(); // 蜂鸣器任务
      LCD_Task();  // LCD刷新任务
      
      UART_Send_AD_Data_Task();     // 实时数据发送任务
      UART_Check_Rx_Command_Task(); // 命令接收检查任务
      
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
/****************    DAC部分     *******************/
//初始化sin表
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
// 正弦波
void SinWaveOut(void)
{
	static u16 i = 0;
	i++;
	if (i >= NPT)
		i = 0;
    
	DAC_Val = SinTable[i];
    DAC_Val*=k;
    HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_1,DAC_ALIGN_12B_R,DAC_Val);//设置DAC值
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
    HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_1,DAC_ALIGN_12B_R,DAC_Val);//设置DAC值
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
    HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_1,DAC_ALIGN_12B_R,DAC_Val);//设置DAC值
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
    HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_1,DAC_ALIGN_12B_R,DAC_Val);//设置DAC值
}

void UserWaveOut(void)
{
	static u16 i = 0;
	i++;
	if (i >= NPT)
		i = 0;
    
	DAC_Val = UserSinTable[i];
    DAC_Val*=k;
    HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_1,DAC_ALIGN_12B_R,DAC_Val);//设置DAC值
}
// 停止输出
void StophWaveOut(void)
{
    DAC_Val = 0;
    HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_1,DAC_ALIGN_12B_R,DAC_Val);//设置DAC值 
}

// 定时器中断处理
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
    
    UART_Send_Flag = 1; // 可以发送数据
}

/***************************************************/
/****************    串口部分     ******************/

// 串口接收中断数据处理
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

// 将实时输出电压数据以STAMP协议发送到上位机
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
    if(SW_Timer_Tick[3] > HAL_GetTick()) return; // ※ return 用 > 
    
    if(Rx_Index == 9) // {0:20,30}
    {
        // 检查命令格式是否正确
        if((Rx_Buffer[0] != '{') && (Rx_Buffer[2] != ':') && (Rx_Buffer[5] != ',') && (Rx_Buffer[8] != '}'))
            goto Send_Error;

        // 检查命令数据范围是否合理
        temp_mode =  Rx_Buffer[1] - '0';
        temp_freq = (Rx_Buffer[3] - '0') * 10 + Rx_Buffer[4] - '0';
        temp_vpp  = (Rx_Buffer[6] - '0') * 10 + Rx_Buffer[7] - '0';
        if(temp_mode >= 6) goto Send_Error;
        if(temp_freq > 90) goto Send_Error;
        if((temp_vpp  > 33) || (temp_vpp  < 15)) goto Send_Error;
        
        // 命令正确


        if(temp_mode == 0)
        {
            WaveOut_Flag = 0;  // 关闭波形输出
        }
        else
        {
            WaveOut_Flag = 1;     // 打开波形输出
            WaveMode = temp_mode; // 设置输出波形
        }
        
        // 改变波形频率
        DAC_Wave_Freq_x10 = temp_freq;
        DAC_Timer_ARR = 1000000 * 10 / NPT / DAC_Wave_Freq_x10;
        __HAL_TIM_SET_AUTORELOAD(&htim6, DAC_Timer_ARR);
        
        // 改变波形峰峰值
        DAC_Vpp_x10 = temp_vpp;
        k = DAC_Vpp_x10 / 33.0f;
        
        sprintf((char*)Tx_Buffer, "Signal Generator>> Successfully set.\n");
        HAL_UART_Transmit(&huart1, Tx_Buffer, strlen((const char*)Tx_Buffer), 100);   
        
        LCD_Flash_Setting_Flag = 1; // 更新LCD界面
        
        BEEP_State = 1; // BEEP鸣叫
        SW_Timer_Tick[5] = HAL_GetTick() + BEEP_TIME; // 复位软件定时器计时

        //UART_LED_Flag = 1;  // 点亮LED1 250ms
        //SW_Timer_Tick[4] = 250 +  HAL_GetTick();  // 复位软件定时器250ms计时
    }
    else
    {
        Send_Error: // 命令错误
        
        sprintf((char*)Tx_Buffer, "Signal Generator>> Command error.\n");
        HAL_UART_Transmit(&huart1, Tx_Buffer, strlen((const char*)Tx_Buffer), 100);  
        
        BEEP_State = 1;  // BEEP鸣叫
        SW_Timer_Tick[5] = HAL_GetTick() + (BEEP_TIME<<2);  // 复位软件定时器计时
        
        //UART_LED_Flag = 2;  //点亮LED8 250ms
        //SW_Timer_Tick[4] = 250 +  HAL_GetTick(); // 复位软件定时器250ms计时
    }

    // 清零接收缓冲
    Rx_Index = 0;
    memset(Rx_Buffer, 0, RX_MAX_LENTH);
}

/***************************************************/
/**************    按键逻辑部分     ****************/

// 检查按键
void KEY_Task(void)
{
    Task_Delay(0, 75); // 每75ms 检测一次
    
    // 检测按键
    KEY_Val = KEY_Scan();
    KEY_Down = KEY_Val & (KEY_Val ^ KEY_Old);
    KEY_Old = KEY_Val;
    
    if(KEY_Down)
    {
        LCD_Flash_Setting_Flag = 1;  // 更新LCD界面
        
        BEEP_State = 1; // 蜂鸣器鸣叫 
        SW_Timer_Tick[5] = HAL_GetTick() + BEEP_TIME; // 复位软件定时器计时
        
        switch(KEY_Down)
        {
        case 1: // 控制波形是否输出
            WaveOut_Flag = !WaveOut_Flag;
            break;
            
        case 2: // 改变设置选项
            if(++Setting_Point >= 3)  
                Setting_Point = 0;
            break;
            
        case 3:
            switch(Setting_Point)
            {
            case 0: // 改变输出波形

                --WaveMode;  

                if (WaveMode == 0)
                    WaveMode = 4;
                break;
                
            case 1: // 改变波形频率
                
                if(DAC_Wave_Freq_x10 < 10)
                    DAC_Wave_Freq_x10++;
                else if (DAC_Wave_Freq_x10 < 90)
                    DAC_Wave_Freq_x10+=10;

                DAC_Timer_ARR = 1000000 * 10 / NPT / DAC_Wave_Freq_x10;
                __HAL_TIM_SET_AUTORELOAD(&htim6, DAC_Timer_ARR);
                break;
                
            case 2: // 改变波形峰峰值
                
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
            case 0: // 改变输出波形
                
                ++WaveMode; 
            
                if (WaveMode >= 5)
                    WaveMode = 1;
                break;
                
            case 1: // 改变波形频率
                
                
                if(DAC_Wave_Freq_x10 > 10)
                    DAC_Wave_Freq_x10 -= 10;
                else if(DAC_Wave_Freq_x10 > 1)
                    DAC_Wave_Freq_x10--;
                
                DAC_Timer_ARR = 1000000 * 10 / NPT / DAC_Wave_Freq_x10;
                __HAL_TIM_SET_AUTORELOAD(&htim6, DAC_Timer_ARR);
                break;
                
            case 2: // 改变波形峰峰值
                
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
/****************    蜂鸣器部分     ****************/

void BEEP_Task(void)
{
    if(BEEP_State) 
    {
        BEEP_Ctrl(1);
        if(SW_Timer_Tick[5] < HAL_GetTick())  // 计时是否结束
        {
            BEEP_Ctrl(0);
            BEEP_State = 0;
        }    
    }
}

/***************************************************/
/****************    LCD部分      ******************/

// 显示波形开启还是关闭
#define SHOW_LCD_WAVE_ON_OR_OFF  LCD_DisplayChineseString(24 * 3 + 16, 319 -24 * 6, 5 + WaveOut_Flag * 2, 24, 2) 

// 显示波形种类：正弦波/方波/三角波/锯齿波
#define SHOW_LCD_WAVE_MODE       LCD_DisplayChineseString(24 * 4 + 16 + 3, 319 -24 * 6 , 14 + (WaveMode - 1) * 3, 24, 3) 

// 显示波形实际频率
#define SHOW_LCD_WAVE_REAL_FREQ  snprintf((char*)LCD_String_Buffer, 20, "  Frq:%3.1fHz",DAC_Wave_Freq_x10 /10.0f);  \
                                 LCD_DisplayStringLine(Line6, LCD_String_Buffer) 

// 显示波形实际峰峰值
#define SHOW_LCD_WAVE_REAL_VPP   snprintf((char*)LCD_String_Buffer, 20, "  Vpp:%3.1fV",DAC_Vpp_x10 / 10.0f);\
                                 LCD_DisplayStringLine(Line7, LCD_String_Buffer) 

void LCD_Task(void)
{
    Task_Delay(2, 90); // 每90ms 刷新一次
    
    // 采集波形 检查是否生成
    #ifndef UART_SEND_ADC_DATA
    ADC_Val = ADC2_GetAverageVal();
    #endif
    LCD_SetTextColor(Green); // 字体颜色为绿色
    snprintf((char*)LCD_String_Buffer, 20, "     ADC:%4.2fV", ADC_Val * 3.3f / 4096);
    LCD_DisplayStringLine(Line9, LCD_String_Buffer);
    LCD_SetTextColor(White); // 字体颜色为白色
    
    // 检查是否需要更新LCD上显示的设置参数
    if(LCD_Flash_Setting_Flag == 0) return;
    LCD_Flash_Setting_Flag = 0;
    
    if(WaveOut_Flag)
    {    
        LCD_SetBackColor(Green); // 开启输出波形时 字体背景色为绿色
        LCD_SetTextColor(Black); // 字体颜色为黑色
    }
    else
    {
        LCD_SetBackColor(Red);   // 关闭输出波形时 字体背景色为红色
        LCD_SetTextColor(White); // 字体颜色为白色
    }
    SHOW_LCD_WAVE_ON_OR_OFF; // 显示波形输出：开启/关闭
    
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);

    // 根据设置索引高亮对应的选项
    switch(Setting_Point)
    {
    case 0: // 高亮显示波形种类
        
        LCD_SetBackColor(Yellow);
        LCD_SetTextColor(Black);
        SHOW_LCD_WAVE_MODE;      // 显示波形种类：正弦波/方波/三角波/锯齿波
        LCD_SetBackColor(Black);
        LCD_SetTextColor(White);  
        
        SHOW_LCD_WAVE_REAL_FREQ; // 显示波形实际频率
        SHOW_LCD_WAVE_REAL_VPP;  // 显示波形实际峰峰值
        break;
    
    case 1: // 高亮显示波形实际频率
        
        SHOW_LCD_WAVE_MODE; // 显示波形种类：正弦波/方波/三角波/锯齿波
        
        LCD_SetBackColor(Yellow);
        LCD_SetTextColor(Black);        
        SHOW_LCD_WAVE_REAL_FREQ; // 显示波形实际频率  
        LCD_SetBackColor(Black);
        LCD_SetTextColor(White);
        
        SHOW_LCD_WAVE_REAL_VPP;  // 显示波形实际峰峰值
        break;
    
    case 2: // 高亮显示波形实际峰峰值
        
        SHOW_LCD_WAVE_MODE; // 显示波形种类：正弦波/方波/三角波/锯齿波
        
        SHOW_LCD_WAVE_REAL_FREQ; // 显示波形实际频率 

        LCD_SetBackColor(Yellow);
        LCD_SetTextColor(Black);         
        SHOW_LCD_WAVE_REAL_VPP;  // 显示波形实际峰峰值
        LCD_SetBackColor(Black);
        LCD_SetTextColor(White); 
        break;
    }
}

/***************************************************/
/****************    LED部分      ******************/

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
