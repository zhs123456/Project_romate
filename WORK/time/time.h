#ifndef _TIME_H
#define _TIME_H


#include "stm32f10x.h"

extern u8 change;//�����ж����������ز������½��ز���
extern u16 spill_count;//�����ۼӵ�ʱ����ڶ�ʱ�������
extern u16 value;
extern float time;

void TIM5_Confriguration(u16 arr3,u16 psc3);//TIM3�ĳ�ʼ������








#endif

