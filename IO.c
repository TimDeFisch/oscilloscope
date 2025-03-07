#include "header.h"

// 数字信号输出函数
void digital_output() {
    P0 = digital_buffer;
}

// 数字信号读取函数
void digital_read() {
    digital_buffer = P0;
}

// 地址输出函数
void address_latch()
{
    
    
    // P00-P07对应A0-A7
    P0 = address_buffer & 0xFF;

    // 6264的地址段选
    // P20对应A8, P21对应A9, P22对应A10, P23对应A11, P24对应A12
    P2 = (P2 & 0xE0) | ((address_buffer >> 8) & 0x1F);

    // 拉低ALE管脚产生下降沿，锁存地址
    ALE = 0;
    
    // 地址自增，到8095后从0重新开始
    address_buffer++;
    if (address_buffer > 8095) {
        address_buffer = 0;
    }

    void delay_10us(1);//延时一下，等待地址稳定
    // 用完ALE置一
    ALE = 1;
}

