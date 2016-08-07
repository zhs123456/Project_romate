#include "key.h"



//---------------KEY按键GPIO口定义-------------------
void KEY_Confriguration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;//KEY1-->PA.15 端口配置
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//GPIOA15上拉输入
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;//IO口速度为50MHz(作为输入时不用设置速率)
	GPIO_Init(GPIOA,&GPIO_InitStructure);//根据设定参数初始化GPIOA.15
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;//KEY0-->PC.5 端口配置
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//GPIOC5上拉输入
	GPIO_Init(GPIOC,&GPIO_InitStructure);//根据设定参数初始化GPIOC5
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;//WK_UP-->PA.0 端口配置
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;//GPIOA0下拉输入
	GPIO_Init(GPIOA,&GPIO_InitStructure);//根据设定参数初始化GPIOA.0
	
}








