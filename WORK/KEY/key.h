#ifndef _KEY_H
#define _KEY_H


#include "sys.h"

void KEY_Confriguration(void);//KEY�ĳ�ʼ������


#define KEY0 GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)//KEY0������˿�
#define KEY1 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)//KEY1������˿�
#define WK_UP GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//WK_UP������˿�









#endif




