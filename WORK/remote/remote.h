#ifndef _REMOTE_H
#define _REMOTE_H

#include "stm32f10x.h"
#include "sys.h"

#define GPIOA_IDR_Addr  (GPIOA_BASE+8) 									//0x40010808 
#define GPIOA_ODR_Addr  (GPIOA_BASE+12) 								//0x4001080C 
#define REMOTE_ID 0 																		//��ʾң�������ͺ�
#define RDATA 	PAin(1)	 																//�������������

static u8  RmtSta;																			//��־����(ÿһλ����ͬ������)  	  
static u16 Dval;																				//�½���ʱ��������ֵ
static u32 RmtRec;																			//������յ�������	   		    
static u8  RmtCnt;																			//�������µĴ���	 


void Remote_Confriguration(u16 arr3,u16 psc3);					//������Ƶĳ�ʼ������(ʵ�����ǳ�ʼ��TIM5)
u8 Remote_Scan(void);




#endif
