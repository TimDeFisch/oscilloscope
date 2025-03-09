#include "header.h"

unsigned char work_mode = 0x00;       // 模式选择，默认为模式1（实时显示），用0x00表示
unsigned char fixed_wave_mode = 0x00; // 固定波形选择，默认为正弦波，用0x00表示
unsigned char amp_level = 0x00;       // 幅度档位选择，默认为1档，用0x00表示
unsigned char fre_level = 0x00;       // 频率档位选择，默认为1档，用0x00表示

unsigned char key_dsp_select[MAX_DIGITS] = {0x01, 0x02, 0x04, 0x08}; // 控制数码管位选以及键盘扫描选择
unsigned char display_buffer[MAX_DIGITS] = {0xff, 0xff, 0xff, 0xff}; // 数码管显示缓冲区，默认为0xff，不显示
unsigned char digital_buffer;                                        // 数字信号缓冲区
unsigned int address_buffer = 0;                                     // 6264读写的当前地址
unsigned int address_offset = 0;                                     // 6264读写的当前地址偏移量
unsigned int replay_address_offset = 0; // 6264回放的当前地址偏移量
unsigned int address_flag = 0;          // 6264完成一次循环存储的标志
// 主程序
void main(void)
{
    unsigned char col; // 数码管位选 & 键盘扫描 列数计数

    // 初始化代码
    // init_fre_counter0(); // 定时器初始化
    init_interrupts(); // 中断初始化
    // 暂时性，先屏蔽中断和定时器，首先在原本主循环中尝试实现功能(或许应该改一改，而不是直接屏蔽)
    // 修改了init_interrupts函数，将ET0置0，屏蔽了定时器中断，记得改回来
    init_AD();    // AD寄存器初始化
    init_HC595(); // 74HC595初始化

    // 主循环
    while (1)
    {
        // 模式选择
        switch (work_mode)
        {
        case 0:
            mode_realtime(); // 模式1 波形实时显示
            break;
        case 1:
            mode_replay(); // 模式2 波形回放显示
            break;
        case 2:
            mode_measure(); // 模式3 测量
            break;
        default:
            break;
        }

        // 数码管显示 & 按键扫描
        for (col = 0; col < MAX_DIGITS; col++)
        {
            HC595_load(combine(key_dsp_select[col], display_buffer[col]));
            HC595_output();
            key_scan(col);
        }
    }
}

unsigned int combine(unsigned char a, unsigned char b)
{
    return (a << 8) | b;
}

void debug()
{
    unsigned char col;
    // 数码管：1
    display_buffer[2] = 0x7d;
    // 数码管显示 & 按键扫描
    while (1)
    {
        for (col = 0; col < MAX_DIGITS; col++)
        {
            HC595_load(combine(key_dsp_select[col], display_buffer[col]));
            HC595_output();
            key_scan(col);
        }
    }
}

// 模式1 波形实时显示
void mode_realtime(void)
{
    // AD转化
    AD_get();
    // 实时信号存储到6264
    XBYTE[0x1000 + address_offset] = digital_buffer;//保证片选为0的同时，使用余下的12位地址存储信号
    address_offset++;
    if (address_offset >= DA_LEN)
    {
        address_offset = 0;
        address_flag = 1;
    }
    // 实时信号输出到DAC通道1
    DA_CH1 = digital_buffer;
    // 固定波形输出到DAC通道2
    fixed_wave_generate(fixed_wave_mode, amp_level, fre_level);
    DA_CH2 = digital_buffer;
}

// 模式2 波形回放显示
void mode_replay(void)
{
    // AD转化
    AD_get();
    // 实时信号输出到DAC通道2
    DA_CH2 = digital_buffer;
    // 回放信号输出到DAC通道1（还没想好怎么处理地址逻辑，目前想的是【如下代码】，这样比较安全，但是可读长度随机，万一刚读完一个循环就开始回放就gg；或者【从1000+address_offset+1开始】，但循环存储需要区分第一个循环和之后的循环，因为第一个循环address_offset之后的位置还没存东西）
    DA_CH1 = XBYTE[0x1000 + replay_address_offset];
    replay_address_offset++;
    if(address_flag == 1){
        if (replay_address_offset >= DA_LEN)
        {
            replay_address_offset = 0;
        }
    }else{
        if (replay_address_offset >= address_offset)
        {
            replay_address_offset = 0;
        }
    }

}

// 模式3 测量
void mode_measure(void)
{
    // AD转化
    // AD_get();
    // TODO
    // 信号存储
    // 信号特征提取
    // 数码管数值计算
}

// HC595数据加载到移位寄存器
void HC595_load(unsigned int dat)
{
    unsigned char i;
    for (i = 0; i < 12; i++)
    {
        if (dat & 0x0800)
        { // TODO 4个LED灯怎么操控？
            SDAT = 1;
        }
        else
        {
            SDAT = 0;
        }
        SCLK = 0;
        delay_10us(1);
        SCLK = 1; // 上升沿将数据放入移位寄存器
        delay_10us(1);
        dat <<= 1;
    }
}

// HC595锁存输出
void HC595_output(void)
{
    RCLK = 0;
    delay_10us(1);
    RCLK = 1; // 上升沿将数据放入输出寄存器
    delay_10us(1);
}

// 按键扫描
void key_scan(unsigned char col)
{
    // 使用消抖操作
    if (KEY1 == 1)
    {
        delay_10us(2000); // 等待20ms
        if (KEY1 == 1)
        { // 未抬起，视为按下
            while (1)
            {
                delay_10us(2000); // 每20ms检测一次，持续等待直至抬起
                if (KEY1 == 0)
                {
                    delay_10us(2000); // 等待20ms
                    if (KEY1 == 0)
                    {
                        key_action(1, col); // 已抬起，执行按键动作
                        break;              // 跳出循环
                    }
                }
            }
        }
    }
    else if (KEY2 == 1)
    {
        delay_10us(2000); // 等待20ms
        if (KEY2 == 1)
        { // 未抬起，视为按下
            while (1)
            {
                delay_10us(2000); // 每20ms检测一次，持续等待直至抬起
                if (KEY2 == 0)
                {
                    delay_10us(2000); // 等待20ms
                    if (KEY2 == 0)
                    {
                        key_action(2, col); // 已抬起，执行按键动作
                        break;              // 跳出循环
                    }
                }
            }
        }
    }
}

// 按键动作
// TODO key1 key2 反了？
void key_action(unsigned char row, unsigned char col)
{
    if (row == 1)
    {
        switch (col)
        {
        case 0: // 按键S1，在3个工作模式中循环切换（实时显示、回放显示、测量）
            work_mode = (work_mode + 1) % MODE_NUM;
            debug_key(1);
            break;
        case 1: // 按键S2，切换波形类型（正弦波、三角波、方波、锯齿波）
            fixed_wave_mode = (fixed_wave_mode + 1) % FIXED_WAVE_NUM;
            debug_key(2);
            break;
        case 2: // 按键S3，增加幅度档位
            if (amp_level < AMP_NUM - 1)
            {
                amp_level++;
            }
            debug_key(3);
            break;
        case 3: // 按键S4，减小幅度档位
            if (amp_level > 0)
            {
                amp_level--;
            }

            debug_key(4);
            break;
        default:
            break;
        }
    }
    if (row == 2)
    {
        switch (col)
        {
        case 0: // 按键S5，增加频率档位
            if (fre_level < FRE_NUM - 1)
            {
                fre_level++;
            }

            debug_key(5);
            break;
        case 1: // 按键S6，减小频率档位
            if (fre_level > 0)
            {
                fre_level--;
            }
            debug_key(6);
            break;
        case 2: // 按键S7
            // 未定义
            debug_key(7);
            break;
        case 3: // 按键S8
            // 未定义

            debug_key(8);
            break;
        default:
            break;
        }
    }
}

// 10us延时函数
void delay_10us(unsigned char n)
{
    unsigned char i;
    for (n; n > 0; n--)
        for (i = 26; i > 0; i--)
            ;
}
