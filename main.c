#include "header.h"

unsigned char work_mode = 0x00;       // 模式选择，默认为模式1（实时显示），用0x00表示
unsigned char measure_mode = 0x00;    // 测量模式选择，默认为幅值测量，用0x00表示
unsigned char fixed_wave_mode = 0x00; // 固定波形选择，默认为正弦波，用0x00表示
unsigned char amp_level = 0x01;       // 幅度档位选择，默认为1档，用0x01表示
unsigned char fre_level = 0x01;       // 频率档位选择，默认为1档，用0x01表示
int n = 0;                          // 实时测得值
int n_old = 0;                      // 上一次测得值
int a = 0, b = 0, c = 0, d = 0;     // 数码管显示的四位数
unsigned char key_dsp_select[MAX_DIGITS] = {0x01, 0x02, 0x04, 0x08}; // 控制数码管位选以及键盘扫描选择
unsigned char display_buffer[MAX_DIGITS] = {0x00, 0x00, 0x00, 0x00}; // 数码管显示缓冲区，默认全显示
unsigned char digital_buffer;                                        // 数字信号缓冲区
unsigned int address_offset = 0;                                     // 6264读写的当前地址偏移量
unsigned int replay_address_offset = 0;                              // 6264回放的当前地址偏移量
unsigned int address_flag = 0;                                       // 6264完成一次循环存储的标志

unsigned char fre_level_table[4] = {1, 2, 4, 8}; // 频率档位表
unsigned char fre_level_index = 0;               // 频率档位索引
unsigned char AD_Flag = 0;                       // AD采样标志，为1时表示完成了一次AD采样，主循环中可以进行处理；为0时表示未完成AD采样
extern unsigned char fre_counter;                // 频率计数器
// 主程序
void main(void)
{
    unsigned char col; // 数码管位选 & 键盘扫描 列数计数

    // 初始化代码
    init_interrupts(); // 中断初始化
    init_timer0();     // 定时器初始化
    init_timer1();     // 定时器1初始化
    init_AD();         // AD寄存器初始化
    init_HC595();      // 74HC595初始化

    // 主循环
    while (1)
    {
        if (AD_Flag == 1)
        {
            switch (work_mode)
            {
            case 0:
                debug(1, (unsigned int)digital_buffer / 100, (unsigned int)(digital_buffer / 10) % 10, (unsigned int)digital_buffer % 10);//这个Debug最后要删掉
                mode_realtime(); // 模式1 波形实时显示
                break;
            case 1:
                debug(2, 10, 11, 11);
                mode_replay(); // 模式2 波形回放显示
                break;
            case 2:
                mode_measure(); // 模式3 测量
                break;
            default:
                break;
            }
            AD_Flag = 0; // 复位标志
        }
      
        // 数码管显示 & 按键扫描
        for (col = 0; col < MAX_DIGITS; col++)
        {
            CHECK_6 = !CHECK_6; // P1.6 翻转，便于检测显示频率
            HC595_load(combine(key_dsp_select[col], display_buffer[col]));
            HC595_output();
            key_scan(col);
        }
    }
}
// 进入timer1中断表示过了250ms，频率计数器增加1
void timer1_interrupt(void) interrupt 3
{
    EA = 0; // 禁止全局中断

    // 恢复定时器重载值
    TL1 = 0xdc;  // 定时器初值
    TH1 = 0x0b;  // 定时器重载值
    
    // 频率计数器增加1
    fre_counter++;

    EA = 1; // 重新开启全局中断
}

// 以固定频率进入timer0中断，在中断中进行工作操作
void timer0_interrupt(void) interrupt 1
{
    EA = 0; // 暂时禁止全局中断
    // 模式选择
    AD_get(); // AD采样
    AD_Flag = 1; // 完成一次AD采样

    EA = 1; // 退出中断前，重新开启全局中断
}

// 模式1 波形实时显示
void mode_realtime(void)
{
    // AD转化
    //AD_get();
    // 实时信号存储到6264
    XBYTE[0x0700 + address_offset] = (digital_buffer >> 1) + 0x40; // 保证片选为0的同时，使用余下的12位地址存储信号（注意，存储了1/2）
    address_offset++;
    if (address_offset >= DA_LEN)
    {
        address_offset = 0;
        address_flag = 1; // 标记该变量表示波形已经在6264中存满，方便回放模式中选择读取终点地址
    }
    // 实时信号输出到DAC通道1
    DA_CH1 = (digital_buffer >> 1) + 0x40;
    // 固定波形输出到DAC通道2
    fixed_wave_generate(fixed_wave_mode, amp_level, fre_level);
    DA_CH2 = digital_buffer;
}

// 模式2 波形回放显示
void mode_replay(void)
{
    // AD转化
    // AD_get();
    // 实时信号输出到DAC通道2
    DA_CH2 = (digital_buffer >> 1) + 0x40;
    // 从6264中读取信号
    DA_CH1 = XBYTE[0x0700 + replay_address_offset];
    replay_address_offset++;
    if (address_flag == 1)
    {
        if (replay_address_offset >= DA_LEN)
        {
            replay_address_offset = 0;
        }
    }
    else
    {
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
    // 信号特征提取
    measure_wavedata();

    // 信号特征显示
    if (measure_mode == 0) // 显示幅值
    {
        n = amp_measured * 20; // 计算振幅，按比例缩放，单位V，但是利用了5/256约等于0.02，于是集合在main显示部分1000*0。02=20
        if (n != n_old)
        {
            n_old = n;
            a = n / 1000;
            b = (n / 100) % 10;
            c = (n / 10) % 10;
            d = n % 10;
        }
        display_amp(a, b, c, d);
    } // 小数点需要显示在第一位
    else
    {
        n = fre_measured * 10;
        if (n != n_old)
        {
            n_old = n;
            a = n / 1000;
            b = (n / 100) % 10;
            c = (n / 10) % 10;
            d = n % 10;
        }
        display_fre(a, b, c, d);
    } // 小数点需要显示在第三位
}

// 将加载到HC595的高4位和低8位数据结合到一起
unsigned int combine(unsigned char a, unsigned char b)
{
    return (a << 8) | b;
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
void key_action(unsigned char row, unsigned char col)
{
    if (row == 2)
    {
        switch (col)
        {
        case 0: // 按键S1，在3个工作模式中循环切换（实时显示、回放显示、测量）
            work_mode = (work_mode + 1) % MODE_NUM;
            // debug_key(1);
            break;
        case 1: // 按键S2，切换波形类型（正弦波、三角波、方波、锯齿波）
            fixed_wave_mode = (fixed_wave_mode + 1) % FIXED_WAVE_NUM;
            // debug_key(2);
            break;
        case 2: // 按键S3，增加幅度档位
            if (amp_level < AMP_NUM)
            {
                amp_level++;
            }
            // debug_key(3);
            break;
        case 3: // 按键S4，减小幅度档位
            if (amp_level > 1)
            {
                amp_level--;
            }
            // debug_key(4);
            break;
        default:
            break;
        }
    }
    if (row == 1)
    {
        switch (col)
        {
        case 0: // 按键S5，增加频率档位
            if (fre_level_index < FRE_NUM - 1)
            {
                fre_level_index++;
            }
            fre_level = fre_level_table[fre_level_index];
            // debug_key(5);
            break;
        case 1: // 按键S6，减小频率档位
            if (fre_level_index > 0)
            {
                fre_level_index--;
            }
            fre_level = fre_level_table[fre_level_index];
            // debug_key(6);
            break;
        case 2: // 按键S7
            measure_mode = (measure_mode + 1) % MEASURE_MODE_NUM;
            // debug_key(7);
            break;
        case 3: // 按键S8
            // 未定义
            // debug_key(8);
            break;
        default:
            break;
        }
    }
}

// 40us延时函数（考虑了分频）
void delay_10us(unsigned char n)
{
    unsigned char a, b;
    for (b = n; b > 0; b--)
        for (a = 2; a > 0; a--)
            ;
}

// 400ms延时函数（考虑了分频）
void delay_100ms(void)
{
    unsigned char a, b, c;
    for (c = 19; c > 0; c--)
        for (b = 20; b > 0; b--)
            for (a = 130; a > 0; a--)
                ;
}