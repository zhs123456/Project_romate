#include "sys.h"



void LED_Confriguration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD,ENABLE);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;																		//DS0-->PA.8 端口配置
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;														//GPIOA8推挽输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;														//IO口速度为50MHz
	GPIO_Init(GPIOA,&GPIO_InitStructure);																			//根据设定参数初始化GPIOA.8
	GPIO_SetBits(GPIOA,GPIO_Pin_8);																						//PA8设置为高电平
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;																		//DS1-->PD.2 端口配置
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;														//GPIOD2推挽输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;														//IO口速度为50MHz
	GPIO_Init(GPIOD,&GPIO_InitStructure);																			//根据设定参数初始化GPIOC5
	GPIO_SetBits(GPIOD,GPIO_Pin_2);																						//PD2设置为高电平
}











