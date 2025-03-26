#include "header.h"
#include <math.h>

// 数码管段码表
static const unsigned char seg_code[] = {
    0x11, 0x7d, 0x23, 0x29, 0x4d, 0x89, 0x81, 0x3d, 0x01, 0x09, 0xef, 0xff, 0x00, // 0-9 负号 空 满
};

//显示
void debug(unsigned int a, unsigned int b, unsigned int c, unsigned int d)
{
  display_buffer[0] = seg_code[a];
  display_buffer[1] = seg_code[b];
  display_buffer[2] = seg_code[c];
  display_buffer[3] = seg_code[d];
}

//幅值显示（三位小数）
void display_amp(unsigned int a, unsigned int b, unsigned int c, unsigned int d)
{
  display_buffer[0] = seg_code[a] & 0xfe;
  display_buffer[1] = seg_code[b];
  display_buffer[2] = seg_code[c];
  display_buffer[3] = seg_code[d];
}

//频率显示（一位小数）
void display_fre(unsigned int a, unsigned int b, unsigned int c, unsigned int d)
{
  display_buffer[0] = seg_code[a];
  display_buffer[1] = seg_code[b];
  display_buffer[2] = seg_code[c] & 0xfe;
  display_buffer[3] = seg_code[d];
}

