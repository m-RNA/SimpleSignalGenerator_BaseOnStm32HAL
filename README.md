## 1.前言
2022/6/9  作者：[@BILIBILI：m-RNA](https://space.bilibili.com/41224928  "@BILIBILI：m-RNA 个人主页")    E-mail：m-RNA@qq.com      

简易信号发生器：
- 硬件平台：蓝桥杯嵌入式 ***Stm32G431RBT6*** 开发板
- 软件设计：基于Stm32**HAL**库开发，尝试使用分层思想编写代码，方便他人移植使用。
- 波形输出引脚： ***PA2***
- 电压采集引脚： ***PA5***

软件分层：
- UI：表现层
- BLL：业务逻辑层
- BSP：板级支持包
- HAL：硬件抽象层
## 2.性能指标
目前此简易信号发生器采用 ***DMA+OPAMP*** +TIM+DAC方案，已实现功能有：
- 输出波形：正弦波、方波、三角波、锯齿波；
- 输出峰峰值：1.5~3.3V
- 输出频率：1Hz~94.8kHz
> 注：频率还能设置得更高，不过频率过高波形会失真，DAC的转换需要时间（经测量约为1.5us）
## 3.使用提示
输出波形设置方法：
- 通过按键（支持长按）粗糙设置输出波形；
- 通过串口发送命令。 

串口命令格式：{Type:Freq,Vpp}  
命令长度最短为9个字符。其中，各部分的含义如下表所示。  

| 协议格式 | {    | Type     | ：   | Freq   | ,    | Vpp    | }    |
| -------- | ---- | -------- | ---- | ------ | ---- | ------ | ---- |
| 具体含义 | 包头 | 波形种类 | 格式 | 频率   | 格式 | 峰峰值 | 包尾 |
| 字节长度 | 1    | 1        | 1    | 不定长 | 1    | 3      | 1    |

- Type为一个数字，表示输出波形的状态，0-4分别表示以下含义：停止输出、输出正弦波、输出方波、输出三角波、输出锯齿波；  
- 冒号之后的Freq表示频率，单位为Hz，例如：1表示设置输出波形频率为1Hz，114514表示设置输出波形频率为114514Hz，依此类推；  
- 逗号之后的Vpp表示峰峰值，单位为V，格式为x.x，例如：2.0表示设置输出波形峰峰值为2.0V，3.3表示设置输出波形峰峰值为3.3V，依此类推。  

例如，向串口发送 *{1:10000,1.5}*，即设置输出波形为10kHz且峰峰值为1.5V的正弦波；而发送 *{4:5,2.5}*，即设置输出波形为5Hz且峰峰值为2.5V的锯齿波。
> 注：发送命令无需加上回车换行。

