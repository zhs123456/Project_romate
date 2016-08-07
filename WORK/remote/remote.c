#include "remote.h"

//----------------TIM5�ĳ�ʼ������-----------------
void Remote_Confriguration(u16 arr,u16 psc)													//�������Զ���װֵΪarr+1,�����ķ�Ƶϵ��Ϊpsc+1
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;										//��ʼ����ʱ�������Ľṹ�������ʼ��
	TIM_ICInitTypeDef TIM5_ICInitStructure;														//��ʱ��5ͨ��1���벶�����õĽṹ�������ʼ��
	NVIC_InitTypeDef NVIC_InitStructure;															//�жϷ���������õĽṹ�������ʼ��
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);								//ʹ��TIM5��ʱ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);							//ʹ��GPIO�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;												//����PA1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;											//PA1����Ϊ��������  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;									//�ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);														//���߲�����ʼ��PA1
	GPIO_SetBits(GPIOA,GPIO_Pin_1);																		//PA1����Ϊ�ߵ�ƽ
	
	TIM_TimeBaseStructure.TIM_Period = arr;														//�����Զ���װ�ؼĴ������ڵ�ֵ,��֤ÿ�θ����жϼ��Ϊ10ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc;													//����ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;						//��Ƶ1ģʽ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;				//���ϼ���ģʽ
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);										//�������õĲ�����ʼ����ʱ��
	
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_2;									//ѡ�������IC2ӳ�䵽TI5��
	TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;			//�����ز���
	TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;	//ֱ��ӳ�䵽TI1��
	TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;						//���������Ƶ,����Ƶ
	TIM5_ICInitStructure.TIM_ICFilter = 0x03;													//IC1F=0011 ���������˲���8����ʱ��ʱ�������˲�
	TIM_ICInit(TIM5, &TIM5_ICInitStructure);													//�������õĲ������ö�ʱ��2ͨ��1���벶��
	TIM_ClearFlag(TIM5,TIM_FLAG_Update);															//ע�⣺TIM_TimeBaseInit����������һ��UG�¼�,��˺�����жϱ�־λһ��Ҫ�����
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);										//���ȼ���˵������ռ���ȼ����õ�λ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;										//TIM5�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;					//��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;								//�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;										//ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);																		//��ʼ������NVIC�Ĵ���
	
	TIM_ITConfig(TIM5,TIM_IT_Update|TIM_IT_CC2,ENABLE);								//��������жϺ�CC2IE�����ж�
	TIM_Cmd(TIM5, ENABLE);																						//ʹ�� TIM5���裨������TIM5��ʱ����
}






void TIM5_IRQHandler(void){
	if(TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
	{ 
		if(RmtSta&0X80)																									//�ϴν��յ���������
		{
			RmtSta&=~(1<<7);																							//���������ʶ
		}
	}
	if(TIM_GetITStatus(TIM5, TIM_IT_CC2) != RESET)										//����1���������¼�
	{
		if(RDATA)																												//��ʾ������
		{
			TIM_SetCounter(TIM5,0);																				//����������ʱ�����������ֵ
			TIM_OC2PolarityConfig(TIM5,TIM_ICPolarity_Falling);						//�����벶�����ó��½��ز���
			RmtSta|=0X10;																									//����������Ѿ�������
		}
		else																														//��ʾ�½���
		{
			Dval=TIM_GetCapture2(TIM5);																		//��ȡTIM5���ݼĴ�����ֵ
			TIM_OC2PolarityConfig(TIM5,TIM_ICPolarity_Rising); 						//����Ϊ�����ز���
			if(RmtSta&0X10)																								//���һ�θߵ�ƽ���� 
			{																																										
 				if(RmtSta&0X80)																							//���յ���������
				{				
					if(Dval>300&&Dval<800)																		//560Ϊ��׼ֵ,560us�ĸߵ�ƽ
					{
						RmtRec<<=1;																							//����һλ.
						RmtRec|=0;																							//��ʾ���յ�0	   
					}else if(Dval>1400&&Dval<1800)														//1680Ϊ��׼ֵ,1680us�ĸߵ�ƽ
					{
						RmtRec<<=1;																							//����һλ.
						RmtRec|=1;																							//��ʾ���յ�1
					}else if(Dval>2200&&Dval<2600)														//�����������µ���Ϣ 2500Ϊ��׼ֵ2.5ms�ĸߵ�ƽ
					{
						RmtCnt++; 																							//������������1��
						RmtSta&=0XF0;																						//��ռ�ʱ��		
					}
 				}else if(Dval>4200&&Dval<4700)															//4500Ϊ��׼ֵ4.5ms�ĸߵ�ƽ
				{
					RmtSta|=1<<7;																							//��ǳɹ����յ���������
					RmtCnt=0;																									//�����������������
				}						 
			}
			RmtSta&=~(1<<4);																							//����ߵ�ƽ�����־	
		}				 		     	    					   
	}
	TIM_ClearITPendingBit(TIM5, TIM_IT_CC2|TIM_IT_Update);						//����жϱ�־λ
}


u8 Remote_Scan(void)
{        
	u8 sta=0; 																												//���ڷ��صı���     
	u8 addr1=0,addr2=0;  																									//�洢��ַ�ı���
	u8 data1=0,data2=0;
	addr1=RmtRec>>24;																									//�õ���ַ��
	addr2=(RmtRec>>16)&0xff;																					//�õ���ַ���� 
 	if((addr1==(u8)~addr2)&&addr1==REMOTE_ID)													//����ң��ʶ����(ID)����ַ�Ƿ���ȷ 
	{ 
	  data1=RmtRec>>8;																								//��õ�������
	  data2=RmtRec; 																									//��õ����ݵķ���
	  if(data1==(u8)~data2)sta=data1;																	//��ֵ��ȷ	 
	}   
  return sta;
}

