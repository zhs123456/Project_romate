#ifndef _REMOTE_H
#define _REMOTE_H

#include "stm32f10x.h"
#include "sys.h"

#define GPIOA_IDR_Addr  (GPIOA_BASE+8) 									//0x40010808 
#define GPIOA_ODR_Addr  (GPIOA_BASE+12) 								//0x4001080C 
#define REMOTE_ID 0 																		//表示遥控器的型号
#define RDATA 	PAin(1)	 																//红外数据输入脚

static u8  RmtSta;																			//标志标量(每一位代表不同的意义)  	  
static u16 Dval;																				//下降沿时计数器的值
static u32 RmtRec;																			//红外接收到的数据	   		    
static u8  RmtCnt;																			//按键按下的次数	 


void Remote_Confriguration(u16 arr3,u16 psc3);					//红外控制的初始化函数(实际上是初始化TIM5)
u8 Remote_Scan(void);




#endif
