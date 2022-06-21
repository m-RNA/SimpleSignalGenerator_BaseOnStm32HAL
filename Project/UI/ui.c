/***************************************************/
/****************    LCD部分      ******************/
#include "ui.h"
#include "bsp.h"
#include "bll.h"
#include "stdio.h"

// 显示波形开启还是关闭
#define SHOW_LCD_WAVE_ON_OR_OFF  LCD_DisplayChineseString(24 * 3 + 16, 319 -24 * 6, 5 + WaveOut_Flag * 2, 24, 2) 

// 显示波形种类：正弦波/方波/三角波/锯齿波
#define SHOW_LCD_WAVE_MODE       LCD_DisplayChineseString(24 * 4 + 16 + 3, 319 -24 * 6 , 14 + (WaveMode - 1) * 3, 24, 3) 

// 显示波形实际频率
#define SHOW_LCD_WAVE_REAL_FREQ     if(DAC_Wave_Freq < 10000) snprintf((char*)LCD_String_Buffer, 20, "  Frq:%dHz            ",DAC_Wave_Freq); \
                                    else                      snprintf((char*)LCD_String_Buffer, 20, "  Frq:%dkHz            ",DAC_Wave_Freq / 1000); \
                                    LCD_DisplayStringLine(Line6, LCD_String_Buffer)

// 显示波形实际峰峰值
#define SHOW_LCD_WAVE_REAL_VPP   snprintf((char*)LCD_String_Buffer, 20, "  Vpp:%3.1fV",DAC_Vpp_x10 / 10.0f);\
                                 LCD_DisplayStringLine(Line7, LCD_String_Buffer) 
#define UI_Set_Highlight() LCD_SetBackColor(Yellow); LCD_SetTextColor(Black)

#define UI_Set_Background()   LCD_SetBackColor(Black);  LCD_SetTextColor(White)

vu32 SW_Timer_Tick = 0;
u8 LCD_String_Buffer[21] = {0}; // LCD显示字符缓存
u8 LCD_Updata_Setting_Flag = 1;  // 刷新LCD设置标志位

u16 ADC_Val = 0;       // ADC数值

void UI_Init(void)
{
    // 初始化LCD和LED
    LCD_Init();
    LCD_Clear(Black);
    UI_Set_Background();
    LED_Disp(0);

    // 显示初始LCD界面 
    LCD_DrawLine(8 + 36 + 8 + 16 + 8, 0, 320, Horizontal); // 画线
    LCD_DrawLine(Line8 + 8, 0, 320, Horizontal);           // 画线

    LCD_DisplayChineseString(8, 319 - 36 * 2 - 4, 0, 32, 5);        // 显示标题 信号发生器
    LCD_DisplayChineseString(8 + 36 + 4, 319 - 16 * 4, 0, 16, 11);  // 显示姓名 未定

    LCD_DisplayChineseString(24 * 3 + 16, 319 -24 -8, 0, 24, 5);     // 显示 波形输出
    LCD_DisplayChineseString(24 * 4 + 16 + 3, 319 -24 -8, 9, 24, 5); // 显示 波形选择
}

void UI_Updata_Setting(void)
{
    LCD_Updata_Setting_Flag = 1;
}

void UI_Update_Task(void)
{
    Task_Delay(90); // 每90ms 刷新一次
    
    // 采集波形 检查是否生成
    #ifndef UART_SEND_ADC_DATA
    ADC_Val = ADC2_GetAverageVal();
    #endif
    LCD_SetTextColor(Green); // 字体颜色为绿色
    snprintf((char*)LCD_String_Buffer, 20, "     ADC:%4.2fV", ADC_Val * 3.3f / 4096);
    LCD_DisplayStringLine(Line9, LCD_String_Buffer);
    LCD_SetTextColor(White); // 字体颜色为白色
    
//    // 检查是否需要更新LCD上显示的设置参数
//    if(LCD_Updata_Setting_Flag == 0) return;
//    LCD_Updata_Setting_Flag = 0;
    
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
    
    UI_Set_Background();

    // 根据设置索引高亮对应的选项
    switch(BLL_Set_Get_Setting_Index())
    {
    case 0: // 高亮显示波形种类
        
        UI_Set_Highlight();
        SHOW_LCD_WAVE_MODE;      // 显示波形种类：正弦波/方波/三角波/锯齿波
        UI_Set_Background(); 
        
        SHOW_LCD_WAVE_REAL_FREQ; // 显示波形实际频率
        SHOW_LCD_WAVE_REAL_VPP;  // 显示波形实际峰峰值
        break;
    
    case 1: // 高亮显示波形实际频率
        
        SHOW_LCD_WAVE_MODE; // 显示波形种类：正弦波/方波/三角波/锯齿波
        
        UI_Set_Highlight();    
        SHOW_LCD_WAVE_REAL_FREQ; // 显示波形实际频率  
        UI_Set_Background(); 
        
        SHOW_LCD_WAVE_REAL_VPP;  // 显示波形实际峰峰值
        break;
    
    case 2: // 高亮显示波形实际峰峰值
        
        SHOW_LCD_WAVE_MODE;      // 显示波形种类：正弦波/方波/三角波/锯齿波
        SHOW_LCD_WAVE_REAL_FREQ; // 显示波形实际频率 

        UI_Set_Highlight();
        SHOW_LCD_WAVE_REAL_VPP;  // 显示波形实际峰峰值
        UI_Set_Background(); 
        break;
    }
}
