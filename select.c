#include "header.h"

// 片选修改函数
void chip_select(unsigned char select)
{
    // 关闭中断，防止意外中断影响操作
    EA = 0;

    // 使用if-else语句控制管脚电平
    if(select == 0)
    {
        // 将P2.5, P2.6, P2.7设置为低电平
        P2 &= 0x1F;  // 清除P2.5-P2.7的高3位
    }
    else if(select == 1)
    {
        // P2.5为高电平，P2.6和P2.7为低电平
        P2 = (P2 & 0x1F) | 0x20;  // 设置P2.5为高电平
    }
    else if(select == 2)
    {
        // P2.6为高电平，P2.5和P2.7为低电平
        P2 = (P2 & 0x1F) | 0x40;  // 设置P2.6为高电平
    }

    // WR管脚拉低
    WR = 0;

    // 延时至少3ms
    // 根据单片机晶振频率调整延时
    {
        unsigned int i, j;
        for(i = 0; i < 300; i++)
            for(j = 0; j < 10; j++);
    }

    // WR管脚拉高
    WR = 1;

    // 重新开启中断
    EA = 1;
}