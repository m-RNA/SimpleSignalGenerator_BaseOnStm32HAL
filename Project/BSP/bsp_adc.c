#include "bsp_adc.h"

u16 BSP_ADC_GetAverageVal(void)
{
    u32 ADC_Sum = 0;
    #define ADD_TIMES 8
    u8 i = ADD_TIMES;
    
    while(i--)
    {
        HAL_ADC_Start(&ADC_MOUDLE);
        HAL_ADC_PollForConversion(&ADC_MOUDLE, 100);
        ADC_Sum+=HAL_ADC_GetValue(&ADC_MOUDLE);
    }
    return ADC_Sum / ADD_TIMES;
}

