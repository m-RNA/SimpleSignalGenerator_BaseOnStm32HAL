#ifndef __SG_SYETEM_H_
#define __SG_SYETEM_H_
#include "main.h"

typedef enum
{
    Stop,
    Sin,
    Square,
    Sawtooth,
    Triangular,
    User1,
    User2,
    User3,
    User4,
    User5,
} WaveOut_TYPE;

typedef struct
{
    u8 OFF_ON : 1;            // 输出波形开启关闭
    WaveOut_TYPE Mode : 4;    // 输出波形设置
    u32 Freq;                 // 实际频率
    u16 Vpp_x10;              // 实际峰峰值的10倍
    float k;                  // 比例系数
    u8 KEY_Setting_Index : 2; // 按键设置索引
} Signal_Generator_State_Type, *PSignal_Generator_State_Type;

typedef struct
{
    void (*OFF)(void);      // 关闭波形输出
    void (*ON)(u32 *, u16); // 开启波形输出

    void (*Type)(WaveOut_TYPE); // 设置生成的波形
    void (*Table_Update)(void); // 更新DAC输出表

    void (*Freq_Update)(void); // 更新输出频率
    void (*Vpp_Update)(void);  // 更新输出峰峰值
} Signal_Generator_Set_Type, *PSignal_Generator_Set_Type;

typedef struct
{
    void (*Init)(void);
    Signal_Generator_State_Type State;
    Signal_Generator_Set_Type Set;
    void (*Beep_On_Tick)(u8 Tick);
    void (*Uart_Send_Wave_Data)(void);
} Signal_Generator_Type, *PSignal_Generator_Type;

typedef struct
{
    void (*Init)(void);
    void (*Task)(void);
    void (*ON_Tick)(u8 Tick);
    void (*Ctrl)(u8 State);
} BEEP_Type, *PBEEP_Type;

typedef struct
{
    void (*Init)(void);
    void (*Task)(void);
    u8 (*Scan)(void);
} KEY_Type, *PKEY_Type;

typedef struct
{
    void (*Init)(void);
    void (*UpdateTask)(void);
    void (*Reflash)(void);
} UI_Type, *PUI_Type;

typedef struct
{
    void (*Init)(void);
    void (*SendDataTask)(void);
    void (*ReceiveCommandTask)(void);
    void (*SendDataAllow)(void);
} Community_Type, *PCommunity_Type;

extern PSignal_Generator_Type gpSignal_Generator;
extern PBEEP_Type gpBEEP;
extern PUI_Type gpUI;
extern PCommunity_Type gpCommunity;
extern PKEY_Type gpKEY;

#endif
