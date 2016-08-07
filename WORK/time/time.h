#ifndef _TIME_H
#define _TIME_H


#include "stm32f10x.h"

extern u8 change;//用来判断是是上升沿捕获还是下降沿捕获
extern u16 spill_count;//用来累加当时间大于定时器的溢出
extern u16 value;
extern float time;

void TIM5_Confriguration(u16 arr3,u16 psc3);//TIM3的初始化函数








#endif

