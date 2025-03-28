#include "header.h"

// 定时器初始化
void init_timer0(void)
{
    TMOD &= 0xF0; // 清空原设置
    TMOD |= 0x02; // timer0：常开8位自动重载定时器
    TL0 = 0x06;   // 计数初值，12MHz——250us——4kHz，注意现在分频为1/4，因此定时器每次时间为1ms
    TH0 = 0x06;   // 重载值，同计数初值
    TR0 = 1;      // 启用timer0
}

// 中断初始化
void init_interrupts(void)
{
    EA = 1;  // 允许全局中断
    ES = 1;  // 允许串行中断
    EX0 = 0; // 禁用外部中断INT0
    EX1 = 0; // 禁用外部中断INT1
    ET0 = 1; // 允许timer0中断
    ET1 = 0; // 禁用timer1中断
}

// AD 寄存器初始化
void init_AD()
{
    P1ASF = 0x08; // P1ASF.3=1，P1.3 口作为模拟功能 A/D 使用
    ADC_CONTR = 0xE3;
    AUXR1 &= 0xfb;  // 10 位 A/D 转换结果的高 8 位存放在 ADC_RES 中，低 2 位存放在ADC_RESL 的低 2 位中
    CLK_DIV = 0x02; // 修改分频为1/4，使得WR正常识别
}

// HC595初始化
void init_HC595(void)
{
    SCLK = 0;
    RCLK = 0;
}