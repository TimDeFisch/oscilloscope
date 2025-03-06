#include "header.h"
#include <math.h>

// 数码管段码表
static const unsigned char seg_code[] = {
    0x11, 0x7d, 0x23, 0x29, 0x4d, 0x89, 0x81, 0x3d, 0x01, 0x09, 0xef // 0-9, 负号
};

// 显示数字n在第i位数码管上
void fdisp_num(int i, int n, int show_dot)
{
    if (0 <= i < MAX_DIGITS - 1) // 检查数码管是否存在
    {
        if (n >= 0 && n <= 10) // 检查数字是否在有效范围内
        {
            display_buffer[i] = seg_code[n]; // 获取段码
            if (show_dot)
                display_buffer[i] &= 0xFE; // 点亮小数点
        }
        else
        {
            display_buffer[i] = 0xff; // 无效数字，显示为空白
        }
    }
}

// 显示浮点数n在数码管上(考虑舍入)（-999~9999，负数不一定存在,但是写了比没写好）
void fdisp_float(float n)
{   
    int is_negative, start_index, int_part, digit_pos, current_digit;
    int digits[MAX_DIGITS] = {0}; // 用于存储每一位数字
    int count = 0;                // 记录数字的位数
    unsigned char i;
    // 限制范围
    if (n <= -999.5)
    {
        n = -999;
    }
    else if (n >= 9999.5)
    {
        n = 9999;
    }

    // 初始化数码管缓冲区
    for (i = 0; i < MAX_DIGITS; i++)
    {
        display_buffer[i] = 0xff; // 默认显示为空白
    }

    is_negative = 0; // 是否为负数
    start_index = 0; // 显示的起始位置

    // 判断是否为负数
    if (n < 0)
    {
        is_negative = 1;
        n = -n;              // 取绝对值
        fdisp_num(0, 10, 0); // 在第一位显示负号
        start_index = 1;     // 起始位置顺延到第二位
    }

    // 判断大于等于1的数字（保留四位有效数字）
    if (n >= 1)
    {
        // 保留四位有效数字
        n = roundf(n * 10000) / 10000.0; // 四舍五入到四位有效数字

        int_part = (int)n;
        digit_pos = start_index;

        // 显示整数部分
        // int digits[MAX_DIGITS] = {0}; // 用于存储每一位数字
        // int count = 0;                // 记录数字的位数

        while (int_part > 0 && count < MAX_DIGITS)
        {
            digits[count++] = int_part % 10; // 从低位到高位存储每一位
            int_part /= 10;
        }

        // 从最高位开始显示
        for (i = count - 1; i >= 0; i--)
        {
            fdisp_num(digit_pos++, digits[i], 0);
        }

        n -= int_part; // 去掉整数部分，留下小数部分

        while (digit_pos < MAX_DIGITS)
        {
            n *= 10;
            current_digit = (int)n;
            fdisp_num(digit_pos++, current_digit, (digit_pos == start_index + 1)); // 点亮小数点
            n -= current_digit;                                                    // 更新小数部分
        }
    }
    // 处理小于1的数字（保留三位有效数字）
    else if (n < 1 && n >= 0)
    {
        // 保留三位有效数字
        n = roundf(n * 1000) / 1000.0; // 四舍五入到三位有效数字

        fdisp_num(start_index, 0, 1);                        // 显示0
        fdisp_num(start_index + 1, (int)(n * 10) % 10, 0);   // 显示小数点后第一位
        fdisp_num(start_index + 2, (int)(n * 100) % 10, 0);  // 显示小数点后第二位
        fdisp_num(start_index + 3, (int)(n * 1000) % 10, 0); // 显示小数点后第三位（负数不显示）
    }
}

float roundf(float x) {
    int int_part = (int)x;  // 取整数部分
    float frac_part = x - int_part;  // 取小数部分

    if (x >= 0) {
        return (frac_part >= 0.5f) ? (int_part + 1) : int_part;
    } else {
        return (frac_part <= -0.5f) ? (int_part - 1) : int_part;
    }
}

void debug_key(unsigned char key_num) {
    display_buffer[0] = seg_code[key_num];
}