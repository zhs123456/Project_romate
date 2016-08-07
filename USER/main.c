#include "sys.h"
#include "delay.h"
#include "time.h"
#include "stdio.h"
#include "lcd.h"
#include "remote.h"


int main(void)
{
	u8 change=0;
	u8 key;																													//��ʾ������ֵ�ı���
	u8 *words=0;																										//��ʾҪ��ʾ���ַ�����
	LCD_Init();
	Remote_Confriguration(65535,71);																//����TIM5��ʼ������
	LED_Confriguration();																						//LED�ĳ�ʼ������
	LCD_Display_Dir(0);																							//Ĭ��Ϊ������0����ʾ������1:��ʾ������
	LCD_Clear(RED);																									//��Ļ����ɫ����Ϊ��ɫ
	POINT_COLOR=BLUE;																								//LCD�Ļ�����ɫ	����Ϊ��ɫ  
	BACK_COLOR=YELLOW;																							//LCD��Ļ�ı���ɫ����Ϊ��ɫ
	LCD_ShowString(53,40,200,24,24,"Zhao HuiShou");	
	LCD_ShowString(105,230,100,24,24,"OK");
	BACK_COLOR=RED;																									//LCD��Ļ�ı���ɫ����Ϊ��ɫ
	POINT_COLOR=YELLOW;																							//LCD�Ļ�����ɫ	����Ϊ��ɫ
	LCD_ShowString(10,64,300,16,16,"****************************");	
	LCD_ShowString(10,214,300,16,16,"****************************");	
	BACK_COLOR=GREEN;																								//LCD��Ļ�ı���ɫ����Ϊ��ɫ
	POINT_COLOR=BLUE;																								//LCD�Ļ�����ɫ	����Ϊ��ɫ
	LCD_ShowString(20,90,200,24,24,"THE_CAPTURE_TEST!");
	LCD_ShowString(60,140,200,16,16,"The key value:   ");	
	LCD_ShowString(60,170,120,16,16,"The key value:   ");	
  while(1)
	{
		key = Remote_Scan();
		if(key)
		{	   
			switch(key)																									//��ʾ����Ӧ�İ�������
			{
				case 0:		words = "ERROR";		break;			   
				case 162:	
					words = "POWER";		
					DS0_ON;
					DS1_ON;
					break;	    
				case 98:	words = "UP";				break;	    
				case 2:		words = "PLAY";			break;		 
				case 226:	
					words =	"ALIENTEK";	
					DS0_OFF;
					DS1_OFF;
					break;		  
				case 194:	words =	"RIGHT";		break;	   
				case 34:	words = "LEFT";			break;		  
				case 224:	words = "VOL-";			break;		  
				case 168:	words = "DOWN";			break;		   
				case 144:	words = "VOL+";			break;		    
				case 104:	words = "1";				break;		  
				case 152:	words = "2";				break;	   
				case 176:	words = "3";				break;	    
				case 48:	words = "4";				break;		    
				case 24:	words = "5";				break;		    
				case 122:	words = "6";				break;		  
				case 16:	words = "7";				break;			   					
				case 56:	words = "8";				break;	 
				case 90:	words = "9";				break;
				case 66:	words = "0";				break;
				case 82:	words = "DELETE";		break;		 
			}
			LCD_ShowxNum(175,140,key,3,16,0);														//��ʾ������������
			if(change != key)
			{
				change = key;
				LCD_ShowString(175,170,50,16,16,"        ");								//����������
			}
			LCD_ShowString(175,170,50,16,16,words);											//��ʾ��Ӧ��������			
		}
	}
}
