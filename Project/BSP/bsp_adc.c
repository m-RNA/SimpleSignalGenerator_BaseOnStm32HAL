#include "adc.h"
#include "bsp_adc.h"

u16 ADC2_GetAverageVal(void)
{
    u32 ADC_Sum = 0;
    #define ADD_TIMES 8
    u8 i = ADD_TIMES;
    
    while(i--)
    {
        HAL_ADC_Start(&hadc2);
        HAL_ADC_PollForConversion(&hadc2, 100);
        ADC_Sum+=HAL_ADC_GetValue(&hadc2);
    }
    return ADC_Sum / ADD_TIMES;
}

