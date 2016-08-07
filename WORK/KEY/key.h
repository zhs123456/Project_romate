#ifndef _KEY_H
#define _KEY_H


#include "sys.h"

void KEY_Confriguration(void);//KEY的初始化函数


#define KEY0 GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)//KEY0的输入端口
#define KEY1 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)//KEY1的输入端口
#define WK_UP GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//WK_UP的输入端口









#endif




