#include "sys.h"
#include "usart.h"	  



//-------------------串口1初始化函数-------------------- 
void USART1_Confriguration(u32 bound)//bound:波特率
{
	GPIO_InitTypeDef	GPIO_InitStructure;//GPIO的变量声明
	USART_InitTypeDef	USART_InitStructure;//USART的变量声明
	NVIC_InitTypeDef NVIC_InitStructure;//NVIC的申媒结构体变量
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA,ENABLE);//使能USART1和GPIOA时钟
	
	USART_DeInit(USART1);//复位串口1
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TXD-->PA.9发送端口配置
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;//IO口速度为50MHz
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_Init(GPIOA,&GPIO_InitStructure);//根据设定参数初始化GPIOA.9
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//RXD-->PA.10接收端口配置
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA,&GPIO_InitStructure);//根据设定参数初始化 GPIOA.10
	
	USART_InitStructure.USART_BaudRate=bound;//设置波特率
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits=USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity=USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;//收发模式
	USART_Init(USART1,&USART_InitStructure);//根据设置的参数初始化串口
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);//优先级组3 
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//选择USART1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ 通道使能
	NVIC_Init(&NVIC_InitStructure);//根据指定的参数初始化NVIC寄存器
	
//	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//开启中断
	USART_Cmd(USART1,ENABLE);//使能串口
}

