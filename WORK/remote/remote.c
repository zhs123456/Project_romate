#include "remote.h"

//----------------TIM5的初始化函数-----------------
void Remote_Confriguration(u16 arr,u16 psc)													//真正的自动重装值为arr+1,真正的分频系数为psc+1
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;										//初始化定时器参数的结构体变量初始化
	TIM_ICInitTypeDef TIM5_ICInitStructure;														//定时器5通道1输入捕获配置的结构体变量初始化
	NVIC_InitTypeDef NVIC_InitStructure;															//中断分组参数设置的结构体变量初始化
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);								//使能TIM5定时器时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);							//使能GPIO端口时钟
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;												//设置PA1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;											//PA1设置为下拉输入  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;									//速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);														//更具参数初始化PA1
	GPIO_SetBits(GPIOA,GPIO_Pin_1);																		//PA1设置为高电平
	
	TIM_TimeBaseStructure.TIM_Period = arr;														//设置自动重装载寄存器周期的值,保证每次更新中断间隔为10ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc;													//设置时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;						//分频1模式
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;				//向上计数模式
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);										//根据设置的参数初始化定时器
	
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_2;									//选择输入端IC2映射到TI5上
	TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;			//上升沿捕获
	TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;	//直接映射到TI1上
	TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;						//配置输入分频,不分频
	TIM5_ICInitStructure.TIM_ICFilter = 0x03;													//IC1F=0011 配置输入滤波器8个定时器时钟周期滤波
	TIM_ICInit(TIM5, &TIM5_ICInitStructure);													//根据设置的参数配置定时器2通道1输入捕获
	TIM_ClearFlag(TIM5,TIM_FLAG_Update);															//注意：TIM_TimeBaseInit函数会引发一个UG事件,因此后面的中断标志位一定要被清楚
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);										//优先级组说明了抢占优先级所用的位数
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;										//TIM5中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;					//先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;								//从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;										//使能中断
	NVIC_Init(&NVIC_InitStructure);																		//初始化外设NVIC寄存器
	
	TIM_ITConfig(TIM5,TIM_IT_Update|TIM_IT_CC2,ENABLE);								//允许更新中断和CC2IE捕获中断
	TIM_Cmd(TIM5, ENABLE);																						//使能 TIM5外设（即开启TIM5定时器）
}






void TIM5_IRQHandler(void){
	if(TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
	{ 
		if(RmtSta&0X80)																									//上次接收到了引导码
		{
			RmtSta&=~(1<<7);																							//清空引导标识
		}
	}
	if(TIM_GetITStatus(TIM5, TIM_IT_CC2) != RESET)										//捕获1发生捕获事件
	{
		if(RDATA)																												//表示上升沿
		{
			TIM_SetCounter(TIM5,0);																				//遇到上升沿时清除计数器的值
			TIM_OC2PolarityConfig(TIM5,TIM_ICPolarity_Falling);						//将输入捕获设置成下降沿捕获
			RmtSta|=0X10;																									//标记上升沿已经被捕获
		}
		else																														//表示下降沿
		{
			Dval=TIM_GetCapture2(TIM5);																		//读取TIM5数据寄存器的值
			TIM_OC2PolarityConfig(TIM5,TIM_ICPolarity_Rising); 						//设置为上升沿捕获
			if(RmtSta&0X10)																								//完成一次高电平捕获 
			{																																										
 				if(RmtSta&0X80)																							//接收到了引导码
				{				
					if(Dval>300&&Dval<800)																		//560为标准值,560us的高电平
					{
						RmtRec<<=1;																							//左移一位.
						RmtRec|=0;																							//表示接收到0	   
					}else if(Dval>1400&&Dval<1800)														//1680为标准值,1680us的高电平
					{
						RmtRec<<=1;																							//左移一位.
						RmtRec|=1;																							//表示接收到1
					}else if(Dval>2200&&Dval<2600)														//按键连续按下的信息 2500为标准值2.5ms的高电平
					{
						RmtCnt++; 																							//按键次数增加1次
						RmtSta&=0XF0;																						//清空计时器		
					}
 				}else if(Dval>4200&&Dval<4700)															//4500为标准值4.5ms的高电平
				{
					RmtSta|=1<<7;																							//标记成功接收到了引导码
					RmtCnt=0;																									//清除按键次数计数器
				}						 
			}
			RmtSta&=~(1<<4);																							//清除高电平捕获标志	
		}				 		     	    					   
	}
	TIM_ClearITPendingBit(TIM5, TIM_IT_CC2|TIM_IT_Update);						//清除中断标志位
}


u8 Remote_Scan(void)
{        
	u8 sta=0; 																												//用于返回的变量     
	u8 addr1=0,addr2=0;  																									//存储地址的变量
	u8 data1=0,data2=0;
	addr1=RmtRec>>24;																									//得到地址码
	addr2=(RmtRec>>16)&0xff;																					//得到地址反码 
 	if((addr1==(u8)~addr2)&&addr1==REMOTE_ID)													//检验遥控识别码(ID)及地址是否正确 
	{ 
	  data1=RmtRec>>8;																								//获得的数据码
	  data2=RmtRec; 																									//获得的数据的反码
	  if(data1==(u8)~data2)sta=data1;																	//键值正确	 
	}   
  return sta;
}

