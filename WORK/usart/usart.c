#include "sys.h"
#include "usart.h"	  



//-------------------����1��ʼ������-------------------- 
void USART1_Confriguration(u32 bound)//bound:������
{
	GPIO_InitTypeDef	GPIO_InitStructure;//GPIO�ı�������
	USART_InitTypeDef	USART_InitStructure;//USART�ı�������
	NVIC_InitTypeDef NVIC_InitStructure;//NVIC����ý�ṹ�����
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA,ENABLE);//ʹ��USART1��GPIOAʱ��
	
	USART_DeInit(USART1);//��λ����1
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TXD-->PA.9���Ͷ˿�����
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;//IO���ٶ�Ϊ50MHz
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//�����������
	GPIO_Init(GPIOA,&GPIO_InitStructure);//�����趨������ʼ��GPIOA.9
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//RXD-->PA.10���ն˿�����
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA,&GPIO_InitStructure);//�����趨������ʼ�� GPIOA.10
	
	USART_InitStructure.USART_BaudRate=bound;//���ò�����
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits=USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity=USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;//�շ�ģʽ
	USART_Init(USART1,&USART_InitStructure);//�������õĲ�����ʼ������
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);//���ȼ���3 
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//ѡ��USART1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ ͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);//����ָ���Ĳ�����ʼ��NVIC�Ĵ���
	
//	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//�����ж�
	USART_Cmd(USART1,ENABLE);//ʹ�ܴ���
}

