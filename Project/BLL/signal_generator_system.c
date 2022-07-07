#include "signal_generator_system.h"
#include "bsp.h"
#include "bll.h"
#include "ui.h"

Signal_Generator_Type Signal_Generator = {
    .Init = Signal_Generator_Init,

    .State.OFF_ON = 0,
    .State.Mode = Sin,
    .State.Freq = 2000,
    .State.Vpp_x10 = 25,
    .State.k = 0.0f,
    .State.KEY_Setting_Index = 0,

    .Set.OFF = BSP_DAC_Stop,
    .Set.ON = BSP_DAC_Start,
    .Set.Table_Update = BLL_DAC_Table_Update,
    .Set.Freq_Update = BLL_Signal_Freq_Update,
    .Set.Vpp_Update = BLL_Signal_Vpp_Update,
};

KEY_Type KEY = {
    .Init = (void *)0,
    .Task = KEY_Task,
    .Scan = BSP_KEY_Scan,
};

Community_Type Community = {
    .Init = Receive_Uart_Command_Init,
    .SendDataTask = UART_Send_AD_Data_Task,
    .ReceiveCommandTask = UART_Check_Rx_Command_Task,
    .SendDataAllow = BLL_Uart_Send_Data_Allow,
};

PKEY_Type gpKEY = &KEY;
PCommunity_Type gpCommunity = &Community;
PSignal_Generator_Type gpSignal_Generator = &Signal_Generator;
