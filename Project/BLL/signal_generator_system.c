#include "signal_generator_system.h"
#include "bsp.h"
#include "bll.h"
#include "ui.h"

Signal_Generator_Type Signal_Generator = {
    Signal_Generator_Init,
    {0, Sin, 2000, 25, 0.0f, 0},
    {
        BSP_DAC_Stop,
        BSP_DAC_Start,

        (void *)0,
        BLL_DAC_Table_Update,

        BLL_Signal_Freq_Update,
        BLL_Signal_Vpp_Update,
    },
    BLL_Beep_On_Tick,
    BLL_Uart_Send_Data_Allow,
};

UI_Type UI = {
    UI_Init,
    UI_Update_Task,
    UI_Updata_Setting,
};

BEEP_Type BEEP = {
    (void *)0,
    BEEP_Task,
    BLL_Beep_On_Tick,
    BSP_BEEP_Crtl,
};

KEY_Type KEY = {
    (void *)0,
    KEY_Task,
    BSP_KEY_Scan,
};

Community_Type Community = {
    Receive_Uart_Command_Init,
    UART_Send_AD_Data_Task,
    UART_Check_Rx_Command_Task,
    BLL_Uart_Send_Data_Allow,
};

PUI_Type gpUI = &UI;
PKEY_Type gpKEY = &KEY;
PBEEP_Type gpBEEP = &BEEP;
PCommunity_Type gpCommunity = &Community;
PSignal_Generator_Type gpSignal_Generator = &Signal_Generator;
