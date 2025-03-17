#include "header.h"
#include <math.h>

// 数码管段码表
static const unsigned char seg_code[] = {
    0x11, 0x7d, 0x23, 0x29, 0x4d, 0x89, 0x81, 0x3d, 0x01, 0x09, 0xef ,0xff,0x00 ,//0-9 负号 空 满
};

void debug(unsigned int a,unsigned int b,unsigned int c,unsigned int d)
{
  display_buffer[0] = seg_code[a];
	display_buffer[1] = seg_code[b];
	display_buffer[2] = seg_code[c];
	display_buffer[3] = seg_code[d];
}

void display_amp(unsigned int a,unsigned int b,unsigned int c,unsigned int d)
{
  display_buffer[0] = seg_code[a]&0xfe;
	display_buffer[1] = seg_code[b];
	display_buffer[2] = seg_code[c];
	display_buffer[3] = seg_code[d];
}

void display_fre(unsigned int a,unsigned int b,unsigned int c,unsigned int d)
{
  display_buffer[0] = seg_code[a];
	display_buffer[1] = seg_code[b];
	display_buffer[2] = seg_code[c]&0xfe;
	display_buffer[3] = seg_code[d];
}

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

void debug_key(unsigned char key_num) {
    display_buffer[0] = seg_code[key_num];
}