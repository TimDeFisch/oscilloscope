#include <reg51.h>
#include <absacc.h>
#include <intrins.h>

#define DA_CH1 XBYTE[0x5fff] // CS1，向DA1输出
#define DA_CH2 XBYTE[0x3fff] // CS2，向DA2输出
#define GEN_LEN 200          // 生成指定波形的长度
#define BASE_LINE 100        // 基线
#define AD_LEN 250           // 幅值，频率测量时用
#define DA_LEN 4000          // 存储DA数据的内存长度

// // 测量用
// unsigned int amp_counter = 0; // 幅值计数器
// unsigned int fre_counter = 0; // 频率计数器
// unsigned int rising = 0;     // 上升沿标志
// unsigned int points = 0;     // 连续符合条件的点数
// unsigned int amp,amp_measured,amp_max,amp_min; //幅值

#define MODE_NUM 3       // 工作模式数量
#define FIXED_WAVE_NUM 4 // 固定波形数量
#define MAX_DIGITS 4     // 最多支持4位数码管
#define AMP_NUM 4        // 幅度档位数量
#define FRE_NUM 4        // 频率档位数量

extern unsigned char work_mode;       // 模式选择，默认为模式1（实时显示），用0x00表示
extern unsigned char fixed_wave_mode; // 固定波形选择，默认为正弦波，用0x00表示
extern unsigned int gen_fre_counter;    // DA波形生成计时器
extern unsigned char amp_level;       // 幅度档位选择，默认为1档，用0x00表示
extern unsigned char fre_level;       // 频率档位选择，默认为1档，用0x00表示

extern unsigned char key_dsp_select[MAX_DIGITS]; // 控制数码管位选以及键盘扫描选择
extern unsigned char display_buffer[MAX_DIGITS]; // 数码管显示缓冲区，默认为0xff，不显示
extern unsigned char digital_buffer;             // 数字信号缓冲区
// extern unsigned int address_buffer;              // 6264读写的当前地址

// 引脚定义
// HC595
sbit SDAT = P1 ^ 0; // 串行数据
sbit SCLK = P1 ^ 1; // 串行时钟
sbit RCLK = P1 ^ 2; // 锁存时钟

// 按键地址
sbit KEY1 = P3 ^ 4;
sbit KEY2 = P3 ^ 5;

// sbit ALE = 0xC5;  // ALE连接到P4.5管脚，可以位寻址

// 检查接口
sbit CHECK_4 = P1 ^ 4;
sbit CHECK_5 = P1 ^ 5;
sbit CHECK_6 = P1 ^ 6;
sbit CHECK_7 = P1 ^ 7;

// AD寄存器地址
sfr P1ASF = 0x9D;     // P1 Analog Special Function
sfr AUXR1 = 0xA2;     // Auxiliary register1
sfr ADC_CONTR = 0xBC; // A/D 转换控制特殊功能寄存器
sfr ADC_RES = 0xBD;   // A/D 转换结果寄存器
sfr CLK_DIV = 0x97;   // 时钟分频寄存器

void init_timer0(void); // 定时器初始化
void init_interrupts(void);   // 中断初始化
void init_AD(void);           // AD寄存器初始化
void init_HC595(void);        // HC595初始化

unsigned int combine(unsigned char high, unsigned char low);    // 将加载到HC595的高4位和低8位数据结合到一起
void HC595_load(unsigned int dat); // HC595数据加载到移位寄存器
void HC595_output(void);           // HC595锁存输出
void key_scan(unsigned char i);    // 按键扫描
void key_action(unsigned char row, unsigned char col);

void delay_10us(unsigned char n); // 10us延时

void mode_realtime(void);
void mode_replay(void);
void mode_measure(void);

void AD_get(void); // AD采样
void fixed_wave_generate(int mode, int amp, int fre);

float roundf(float x);
void debug(unsigned int a,unsigned int b,unsigned int c,unsigned int d);
void debug_key(unsigned char key_num);