/***************************************************/
/****************    LCD����      ******************/
#include "ui.h"
#include "bsp.h"
#include "bll.h"
#include "stdio.h"
#include "bll_setting.h"
#include "bll_signal_generator.h"

// ��ʾ���ο������ǹر�
#define SHOW_LCD_WAVE_ON_OR_OFF  LCD_DisplayChineseString(24 * 3 + 16, 319 -24 * 6, 5 + WaveOut_Flag * 2, 24, 2) 

// ��ʾ�������ࣺ���Ҳ�/����/���ǲ�/��ݲ�
#define SHOW_LCD_WAVE_MODE       LCD_DisplayChineseString(24 * 4 + 16 + 3, 319 -24 * 6 , 14 + (WaveMode - 1) * 3, 24, 3) 

// ��ʾ����ʵ��Ƶ��
#define SHOW_LCD_WAVE_REAL_FREQ     if(DAC_Wave_Freq < 10000) snprintf((char*)LCD_String_Buffer, 20, "  Frq:%dHz            ",DAC_Wave_Freq); \
                                    else                      snprintf((char*)LCD_String_Buffer, 20, "  Frq:%dkHz            ",DAC_Wave_Freq / 1000); \
                                    LCD_DisplayStringLine(Line6, LCD_String_Buffer)

// ��ʾ����ʵ�ʷ��ֵ
#define SHOW_LCD_WAVE_REAL_VPP   snprintf((char*)LCD_String_Buffer, 20, "  Vpp:%3.1fV",DAC_Vpp_x10 / 10.0f);\
                                 LCD_DisplayStringLine(Line7, LCD_String_Buffer) 
                                 
vu32 SW_Timer_Tick = 0;
u8 LCD_String_Buffer[21] = {0}; // LCD��ʾ�ַ�����
u8 LCD_Updata_Setting_Flag = 1;  // ˢ��LCD���ñ�־λ

u16 ADC_Val = 0;       // ADC��ֵ

void UI_Init(void)
{
  // ��ʼ��LCD��LED
  LCD_Init();
  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);
  LED_Disp(0);
   
  // ��ʾ��ʼLCD���� 
  LCD_DrawLine(8 + 36 + 8 + 16 + 8, 0, 320, Horizontal); // ����
  LCD_DrawLine(Line8 + 8, 0, 320, Horizontal);           // ����

  LCD_DisplayChineseString(8, 319 - 36 * 2 - 4, 0, 32, 5);        // ��ʾ���� �źŷ�����
  LCD_DisplayChineseString(8 + 36 + 4, 319 - 16 * 4, 0, 16, 11);  // ��ʾ���� δ��
  
  LCD_DisplayChineseString(24 * 3 + 16, 319 -24 -8, 0, 24, 5);     // ��ʾ �������
  LCD_DisplayChineseString(24 * 4 + 16 + 3, 319 -24 -8, 9, 24, 5); // ��ʾ ����ѡ��
}

void UI_Updata_Setting(void)
{
    LCD_Updata_Setting_Flag = 1;
}

void UI_Update_Task(void)
{
    Task_Delay(90); // ÿ90ms ˢ��һ��
    
    // �ɼ����� ����Ƿ�����
    #ifndef UART_SEND_ADC_DATA
    ADC_Val = ADC2_GetAverageVal();
    #endif
    LCD_SetTextColor(Green); // ������ɫΪ��ɫ
    snprintf((char*)LCD_String_Buffer, 20, "     ADC:%4.2fV", ADC_Val * 3.3f / 4096);
    LCD_DisplayStringLine(Line9, LCD_String_Buffer);
    LCD_SetTextColor(White); // ������ɫΪ��ɫ
    
//    // ����Ƿ���Ҫ����LCD����ʾ�����ò���
//    if(LCD_Updata_Setting_Flag == 0) return;
//    LCD_Updata_Setting_Flag = 0;
    
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
    switch(BLL_Set_Get_Setting_Index())
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
