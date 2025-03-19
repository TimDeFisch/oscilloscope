#include "header.h"

unsigned int gen_fre_counter = 0; // DA波形生成计时器

// 测量用
unsigned int amp_counter = 0; // 幅值计数器
unsigned int fre_counter = 0; // 频率计数器
unsigned int rising = 0;     // 上升沿标志
unsigned int points = 0;     // 连续符合条件的点数
unsigned char amp,amp_old,amp_max,amp_min; //幅值
unsigned int MIN_Flag,MAX_Flag,fre_flag=0;
float amp_measured = 0.0; //测量得到的幅值
float fre_measured = 100.0; //测量得到的频率

// 波形表，AMPLITUDE = 96，OFFSET = 0
unsigned char xdata sin_wave[GEN_LEN] = {48,48,49,50,51,51,52,53,54,54,55,56,56,57,58,59,59,60,61,62,62,63,64,64,65,66,67,67,68,69,69,70,71,71,72,73,73,74,74,75,76,76,77,78,78,79,79,80,80,81,81,82,82,83,84,84,84,85,85,86,86,87,87,88,88,88,89,89,90,90,90,91,91,91,92,92,92,92,93,93,93,93,94,94,94,94,94,95,95,95,95,95,95,95,95,95,95,95,95,95,96,95,95,95,95,95,95,95,95,95,95,95,95,95,94,94,94,94,94,93,93,93,93,92,92,92,92,91,91,91,90,90,90,89,89,88,88,88,87,87,86,86,85,85,84,84,84,83,82,82,81,81,80,80,79,79,78,78,77,76,76,75,74,74,73,73,72,71,71,70,69,69,68,67,67,66,65,64,64,63,62,62,61,60,59,59,58,57,56,56,55,54,54,53,52,51,51,50,49,48,48,47,46,45,44,44,43,42,41,41,40,39,39,38,37,36,36,35,34,33,33,32,31,31,30,29,28,28,27,26,26,25,24,24,23,22,22,21,21,20,19,19,18,17,17,16,16,15,15,14,14,13,13,12,11,11,11,10,10,9,9,8,8,7,7,7,6,6,5,5,5,4,4,4,3,3,3,3,2,2,2,2,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,5,5,5,6,6,7,7,7,8,8,9,9,10,10,11,11,11,12,13,13,14,14,15,15,16,16,17,17,18,19,19,20,21,21,22,22,23,24,24,25,26,26,27,28,28,29,30,31,31,32,33,33,34,35,36,36,37,38,39,39,40,41,41,42,43,44,44,45,46,47};
unsigned char xdata saw_wave[GEN_LEN] = {0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,6,6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,8,8,8,8,8,8,8,8,9,9,9,9,9,9,9,9,9,10,10,10,10,10,10,10,10,11,11,11,11,11,11,11,11,12,12,12,12,12,12,12,12,12,13,13,13,13,13,13,13,13,14,14,14,14,14,14,14,14,15,15,15,15,15,15,15,15,15,16,16,16,16,16,16,16,16,17,17,17,17,17,17,17,17,18,18,18,18,18,18,18,18,18,19,19,19,19,19,19,19,19,20,20,20,20,20,20,20,20,21,21,21,21,21,21,21,21,21,22,22,22,22,22,22,22,22,23,23,23,23,23,23,23,23,24,24,24,24,24,24,24,24,24,25,25,25,25,25,25,25,25,26,26,26,26,26,26,26,26,27,27,27,27,27,27,27,27,27,28,28,28,28,28,28,28,28,29,29,29,29,29,29,29,29,30,30,30,30,30,30,30,30,30,31,31,31,31,31,31,31,31,32,32,32,32,32,32,32,32,33,33,33,33,33,33,33,33,33,34,34,34,34,34,34,34,34,35,35,35,35,35,35,35,35,36,36,36,36,36,36,36,36,36,37,37,37,37,37,37,37,37,38,38,38,38,38,38,38,38,39,39,39,39,39,39,39,39,39,40,40,40,40,40,40,40,40,41,41,41,41,41,41,41,41,42,42,42,42,42,42,42,42,42,43,43,43,43,43,43,43,43,44,44,44,44,44,44,44,44,45,45,45,45,45,45,45,45,45,46,46,46,46,46,46,46,46,47,47,47,47,47,47,47,47};
unsigned char xdata rec_wave[GEN_LEN] = {48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned char xdata tri_wave[GEN_LEN] = {0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,6,7,7,7,7,8,8,8,8,9,9,9,9,10,10,10,10,11,11,11,11,12,12,12,12,12,13,13,13,13,14,14,14,14,15,15,15,15,16,16,16,16,17,17,17,17,18,18,18,18,18,19,19,19,19,20,20,20,20,21,21,21,21,22,22,22,22,23,23,23,23,24,24,24,24,24,25,25,25,25,26,26,26,26,27,27,27,27,28,28,28,28,29,29,29,29,30,30,30,30,30,31,31,31,31,32,32,32,32,33,33,33,33,34,34,34,34,35,35,35,35,36,36,36,36,36,37,37,37,37,38,38,38,38,39,39,39,39,40,40,40,40,41,41,41,41,42,42,42,42,42,43,43,43,43,44,44,44,44,45,45,45,45,46,46,46,46,47,47,47,47,48,47,47,47,47,46,46,46,46,45,45,45,45,44,44,44,44,43,43,43,43,42,42,42,42,42,41,41,41,41,40,40,40,40,39,39,39,39,38,38,38,38,37,37,37,37,36,36,36,36,36,35,35,35,35,34,34,34,34,33,33,33,33,32,32,32,32,31,31,31,31,30,30,30,30,30,29,29,29,29,28,28,28,28,27,27,27,27,26,26,26,26,25,25,25,25,24,24,24,24,24,23,23,23,23,22,22,22,22,21,21,21,21,20,20,20,20,19,19,19,19,18,18,18,18,18,17,17,17,17,16,16,16,16,15,15,15,15,14,14,14,14,13,13,13,13,12,12,12,12,12,11,11,11,11,10,10,10,10,9,9,9,9,8,8,8,8,7,7,7,7,6,6,6,6,6,5,5,5,5,4,4,4,4,3,3,3,3,2,2,2,2,1,1,1,1,0,0,0,0};

// AD采样
void AD_get()
{
    int i = 0;          // 循环用
    CHECK_4 = !CHECK_4; // P1.4 翻转，便于检测采样频率

    ADC_RES = 0;       // 清空寄存器
    ADC_CONTR |= 0x08; // 置位ADC_START ：启动 AD 转换

    for (i = 0; i < 4; i++)
        _nop_(); // 对 ADC_CONTR 寄存器进行写操作后,要加 4 个空操作延时,才能够正确读到ADC_CONTR 的值

    while (ADC_CONTR & 0x10 == 0)
        ; // 等待ADC_FLAG：等待AD 转换完成

    ADC_CONTR &= 0xE7;        // 清除ADC_FLAG, ADC_START ：停止 A/D 转换
    digital_buffer = ADC_RES; // 记录采样值
}

// 固定信号波形发生计算器
void fixed_wave_generate(int mode, int amp, int fre) // mode是波形选择标志，amp控制幅度（1-4），fre控制频率（1、2、4、8）
{
    unsigned char gen_result = 0; // 生成的波形值

    CHECK_5 = !CHECK_5; // P1.5 翻转，便于检测波形生成频率

    // 根据频率计算索引
    gen_fre_counter += fre;
    if (gen_fre_counter >= GEN_LEN)
    {
        gen_fre_counter = 0;
    }

    // 根据选择的波形标志查表
    switch (mode)
    {
    case 0: // 正弦波
        gen_result = sin_wave[gen_fre_counter];
        break;

    case 1: // 三角波
        gen_result = tri_wave[gen_fre_counter];
        break;

    case 2: // 矩形波
        gen_result = rec_wave[gen_fre_counter];
        break;

    case 3: // 锯齿波
        gen_result = saw_wave[gen_fre_counter];
        break;

    default:
        return; // 无效标志
    }

		if(mode==0){gen_result = BASE_LINE - 48* amp / AMP_NUM + gen_result * amp / AMP_NUM;}
			else{
    // 根据幅度调整输出值
    gen_result = BASE_LINE + gen_result * amp / AMP_NUM;}

    // 存储到数字信号缓冲区
    digital_buffer = gen_result;
}

void measure_wavedata() 
{
    amp_counter++; // 递增振幅计数器
    fre_counter++; // 递增频率计数器
    
    amp_old = amp; // 记录上一次的值
    amp = digital_buffer; // 获取当前值

    // 过零点判定：当前值和上次值在128（中值）两侧，视为过零
    if ((amp_old < 128 && amp >= 128) || (amp_old > 128 && amp <= 128)) 
    {
        if (fre_flag == 1) // 已经检测到过一次零点，计算周期
        {
            fre_measured = 1000 / fre_counter; // 计算频率，AD采样率1kHz
            fre_counter = 0; // 复位计数器
            fre_flag = 0; // 复位标志
        }
        else if (fre_flag == 0) 
        {
            fre_flag = 1; // 标记已检测到第一个零点
        }
    }

    // 记录最大和最小值（用于计算振幅）
    if (amp > amp_max) amp_max = amp;
    if (amp < amp_min) amp_min = amp;

    // 统计振幅，每250ms更新一次
    if (amp_counter >= AD_LEN) // AD_LEN=250，相当于250ms
    {
        amp_measured = (amp_max - amp_min) ; // 计算振幅，按比例缩放，单位V，但是利用了5/256约等于0.02，于是集合在main显示部分1000*0。02=20

        // 重置振幅计算相关变量
        amp_max = 0;
        amp_min = 256;
        amp_counter = 0;
    }
}


// // 测量频率：利用转折点计算
// float measure_fre()
// {
//     fre_counter++;

//     if ((AD_data > AD_data_old) && (rising == 0)) // 如果信号由下降沿转为上升沿
//     {
//         points++;        // 计算符合条件的点数
//         if (points >= 5) // 满足条件，认为进入上升沿
//         {
//             fre_measured = 2000 / fre_counter; // 计算频率，AD采样率2kHz
//             rising = 1;                        // 标记为上升沿
//             points = 0;                        // 重置点数
//             fre_counter = 0;                   // 重置计数器，开始新的周期
//         }
//     }
//     else if ((AD_data < AD_data_old) && (rising == 1)) // 如果信号由上升沿转为下降沿
//     {
//         points++;        // 计算符合条件的点数
//         if (points >= 5) // 满足条件，认为进入下降沿
//         {
//             rising = 0; // 标记为下降沿
//             points = 0; // 重置点数
//         }
//     }else{

//     }

//     return fre_measured;
// }
// // 测量振幅：一段时间内的最值之差（备份）
// float measure_amp()
// {
//     amp_counter++; // 每次调用时递增计数器
//     amp = digital_buffer;

//     if (amp > amp_max)
//     {
//         amp_max = amp;
//     }
//     else if (amp < amp_min)
//     {
//         amp_min = amp;
//     }

//     if (amp_counter >= AD_LEN)//若计数器达到设定的采样点数（这里ADLEN=1000，1000ms内总结一次最大值和最小值）
//     {
//         amp_measured = (amp_max - amp_min) * 5 / 256; // 计算振幅，并按比例缩放，单位V

//         amp_max = 0;//重置最大最小值
//         amp_min = 256;
//         amp_counter = 0;
//     }

//     return amp_measured;
// }