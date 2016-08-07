#include "lcd.h"
#include "stdlib.h"
#include "font.h" 
#include "delay.h"	 


u16 POINT_COLOR=0x0000;//LCD�Ļ�����ɫ��Ĭ��Ϊ��ɫ��------------------------------------------
u16 BACK_COLOR=0xFFFF;//LCD�ı���ɫ��Ĭ��Ϊ��ɫ��--------------------------------------------- 


_lcd_dev lcddev;//����LCD��Ҫ������Ĭ��Ϊ������


//-------д�Ĵ���������data:�Ĵ���ֵ��-------
void LCD_WR_REG(u16 data)
{ 
	LCD_RS_CLR;//д��ַ  
 	LCD_CS_CLR; 
	DATAOUT(data); 
	LCD_WR_CLR; 
	LCD_WR_SET; 
 	LCD_CS_SET;   
}


//------д���ݺ�����data:�Ĵ���ֵ��------
//�������LCD_WR_DATAX��,��ʱ�任�ռ�
void LCD_WR_DATAX(u16 data)
{
	LCD_RS_SET;
	LCD_CS_CLR;
	DATAOUT(data);
	LCD_WR_CLR;
	LCD_WR_SET;
	LCD_CS_SET;
}


//-----��LCD���ݣ�����ֵ:������ֵ��---------
u16 LCD_RD_DATA(void)
{										   
	u16 t;
 	GPIOB->CRL=0X88888888; //PB0-7��������
	GPIOB->CRH=0X88888888; //PB8-15��������
	GPIOB->ODR=0X0000;//ȫ�����0

	LCD_RS_SET;
	LCD_CS_CLR;
	//��ȡ����(���Ĵ���ʱ,������Ҫ��2��)
	LCD_RD_CLR;
	if(lcddev.id==0X8989)
		delay_ms(1);//FOR 8989,��ʱ1ms					   
	t=DATAIN;  
	LCD_RD_SET;
	LCD_CS_SET; 

	GPIOB->CRL=0X33333333;//PB0-7�������
	GPIOB->CRH=0X33333333;//PB8-15�������
	GPIOB->ODR=0XFFFF;//ȫ�������
	return t;  
}


//-----------------д�Ĵ���-----------------
void LCD_WriteReg(u16 LCD_Reg,u16 LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);//LCD_Reg:�Ĵ������ 
	LCD_WR_DATA(LCD_RegValue);//LCD_RegValue:Ҫд���ֵ	    		 
} 


//-----------------���Ĵ���-----------------
u16 LCD_ReadReg(u16 LCD_Reg)
{										   
 	LCD_WR_REG(LCD_Reg);//д��Ҫ���ļĴ�����  
	return LCD_RD_DATA();//����ֵ:������ֵ 
} 


//-----------------��ʼдGRAM-----------------
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
} 


//-----------------LCDдGRAM-----------------
void LCD_WriteRAM(u16 RGB_Code)
{							    
	LCD_WR_DATA(RGB_Code);//дʮ��λGRAM��RGB_Code:��ɫֵ��
}


//-----------------ͨ���ú���ת��-----------------
//��ILI93xx����������ΪGBR��ʽ��������д���ʱ��ΪRGB��ʽ��
u16 LCD_BGR2RGB(u16 c)
{
	u16  r,g,b,rgb;   
	b=(c>>0)&0x1f;//c:GBR��ʽ����ɫֵ
	g=(c>>5)&0x3f;
	r=(c>>11)&0x1f;	 
	rgb=(b<<11)+(g<<5)+(r<<0);		 
	return(rgb);//����ֵ��RGB��ʽ����ɫֵ
}


//-----------------��ȡ��ĳ�����ɫֵ	----------------- 
u16 LCD_ReadPoint(u16 x,u16 y)
{
 	u16 r,g,b;
	if(x>=lcddev.width||y>=lcddev.height)return 0;//�����˷�Χ,ֱ�ӷ��أ�����ֵ:�˵����ɫ��		   
	LCD_SetCursor(x,y);//x,y:����
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310)LCD_WR_REG(0X2E);//9341/6804/5310���Ͷ�GRAMָ��
	else if(lcddev.id==0X5510)LCD_WR_REG(0X2E00);	//5510 ���Ͷ�GRAMָ��
	else LCD_WR_REG(R34);//����IC���Ͷ�GRAMָ��
	GPIOB->CRL=0X88888888;//PB0-7��������
	GPIOB->CRH=0X88888888;//PB8-15��������
	GPIOB->ODR=0XFFFF;//ȫ�������

	LCD_RS_SET;
	LCD_CS_CLR;	    
	
	LCD_RD_CLR;//��ȡ����(��GRAMʱ,��һ��Ϊ�ٶ�)	
  delay_ms(1);//��ʱ1ms					   
	LCD_RD_SET;
 	
	LCD_RD_CLR;//dummy READ				   
	delay_ms(1);//��ʱ1ms					   
 	r=DATAIN;//ʵ��������ɫ
	LCD_RD_SET;
 	if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X5510)	//9341/NT35310/NT35510Ҫ��2�ζ���
	{	 
		LCD_RD_CLR;					   
		b=DATAIN;//��ȡ��ɫֵ  	  
	 	LCD_RD_SET;
		g=r&0XFF;//����9341,��һ�ζ�ȡ����RG��ֵ,R��ǰ,G�ں�,��ռ8λ
		g<<=8;
	}else if(lcddev.id==0X6804)
	{
		LCD_RD_CLR;					   
	 	LCD_RD_SET;
		r=DATAIN;//6804�ڶ��ζ�ȡ�Ĳ�����ʵֵ 
	}	 
	LCD_CS_SET;
	GPIOB->CRL=0X33333333; //PB0-7  �������
	GPIOB->CRH=0X33333333; //PB8-15 �������
	GPIOB->ODR=0XFFFF; //ȫ�������  
	if(lcddev.id==0X9325||lcddev.id==0X4535||lcddev.id==0X4531||lcddev.id==0X8989||lcddev.id==0XB505)return r;	//�⼸��ICֱ�ӷ�����ɫֵ
	else if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X5510)return (((r>>11)<<11)|((g>>10)<<5)|(b>>11));//ILI9341/NT35310/NT35510��Ҫ��ʽת��һ��
	else return LCD_BGR2RGB(r);//����IC
}


//-----------------LCD������ʾ-----------------
void LCD_DisplayOn(void)
{	
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310)LCD_WR_REG(0X29);//������ʾ
	else if(lcddev.id==0X5510)LCD_WR_REG(0X2900);//������ʾ
	else LCD_WriteReg(R7,0x0173); //������ʾ
}	


//-----------------LCD�ر���ʾ-----------------
void LCD_DisplayOff(void)
{	   
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310)LCD_WR_REG(0X28);//�ر���ʾ
	else if(lcddev.id==0X5510)LCD_WR_REG(0X2800);//�ر���ʾ
	else LCD_WriteReg(R7, 0x0);//�ر���ʾ 
}


//-------���ù��λ�ã�Xpos:�����꣬Ypos:�����꣩--------
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
 	if(lcddev.id==0X9341||lcddev.id==0X5310)
	{		    
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(Xpos>>8); 
		LCD_WR_DATA(Xpos&0XFF);	 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(Ypos>>8); 
		LCD_WR_DATA(Ypos&0XFF);
	}
	else if(lcddev.id==0X6804)
	{
		if(lcddev.dir==1)Xpos=lcddev.width-1-Xpos;//����ʱ����
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(Xpos>>8); 
		LCD_WR_DATA(Xpos&0XFF);	 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(Ypos>>8); 
		LCD_WR_DATA(Ypos&0XFF);
	}
	else if(lcddev.id==0X5510)
	{
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(Xpos>>8); 
		LCD_WR_REG(lcddev.setxcmd+1); 
		LCD_WR_DATA(Xpos&0XFF);	 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(Ypos>>8); 
		LCD_WR_REG(lcddev.setycmd+1); 
		LCD_WR_DATA(Ypos&0XFF);		
	}
	else
	{
		if(lcddev.dir==1)Xpos=lcddev.width-1-Xpos;//������ʵ���ǵ�תx,y����
		LCD_WriteReg(lcddev.setxcmd, Xpos);
		LCD_WriteReg(lcddev.setycmd, Ypos);
	}	 	 
}


//-------����LCD���Զ�ɨ�跽��(9341/6804������ע��)-----------
void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	u16 dirreg=0;
	u16 temp;  
	if(lcddev.dir==1&&lcddev.id!=0X6804)//����ʱ����6804���ı�ɨ�跽��
	{			   
		switch(dir)//����ת��
		{
			case 0:dir=6;break;
			case 1:dir=7;break;
			case 2:dir=4;break;
			case 3:dir=5;break;
			case 4:dir=1;break;
			case 5:dir=0;break;
			case 6:dir=3;break;
			case 7:dir=2;break;
			default:break;
		}
	}
	if(lcddev.id==0x9341||lcddev.id==0X6804||lcddev.id==0X5310||lcddev.id==0X5510)//9341/6804/5310/5510,������
	{
		switch(dir)
		{
			case L2R_U2D://������,���ϵ���
				regval|=(0<<7)|(0<<6)|(0<<5);break; 
				
			case L2R_D2U://������,���µ���
				regval|=(1<<7)|(0<<6)|(0<<5);break; 
				
			case R2L_U2D://���ҵ���,���ϵ���
				regval|=(0<<7)|(1<<6)|(0<<5);break; 
				
			case R2L_D2U://���ҵ���,���µ���
				regval|=(1<<7)|(1<<6)|(0<<5);break; 
					 
			case U2D_L2R://���ϵ���,������
				regval|=(0<<7)|(0<<6)|(1<<5);break; 
				
			case U2D_R2L://���ϵ���,���ҵ���
				regval|=(0<<7)|(1<<6)|(1<<5);break; 
				
			case D2U_L2R://���µ���,������
				regval|=(1<<7)|(0<<6)|(1<<5);break;
				
			case D2U_R2L://���µ���,���ҵ���
				regval|=(1<<7)|(1<<6)|(1<<5);break; 
					
			default:break;
		}
		if(lcddev.id==0X5510)dirreg=0X3600;
		else dirreg=0X36;
 		if((lcddev.id!=0X5310)&&(lcddev.id!=0X5510))regval|=0X08;//5310/5510����ҪBGR   
		if(lcddev.id==0X6804)regval|=0x02;//6804��BIT6��9341�ķ���	   
		LCD_WriteReg(dirreg,regval);
 		if(regval&0X20)
		{
			if(lcddev.width<lcddev.height)//����X,Y
			{
				temp=lcddev.width;
				lcddev.width=lcddev.height;
				lcddev.height=temp;
 			}
		}
		else  
		{
			if(lcddev.width>lcddev.height)//����X,Y
			{
				temp=lcddev.width;
				lcddev.width=lcddev.height;
				lcddev.height=temp;
 			}
		}  
		if(lcddev.id==0X5510)
		{
			LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setxcmd+2);LCD_WR_DATA((lcddev.width-1)>>8); 
			LCD_WR_REG(lcddev.setxcmd+3);LCD_WR_DATA((lcddev.width-1)&0XFF); 
			LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setycmd+2);LCD_WR_DATA((lcddev.height-1)>>8); 
			LCD_WR_REG(lcddev.setycmd+3);LCD_WR_DATA((lcddev.height-1)&0XFF);
		}
		else
		{
			LCD_WR_REG(lcddev.setxcmd); 
			LCD_WR_DATA(0);LCD_WR_DATA(0);
			LCD_WR_DATA((lcddev.width-1)>>8);LCD_WR_DATA((lcddev.width-1)&0XFF);
			LCD_WR_REG(lcddev.setycmd); 
			LCD_WR_DATA(0);LCD_WR_DATA(0);
			LCD_WR_DATA((lcddev.height-1)>>8);LCD_WR_DATA((lcddev.height-1)&0XFF);  
		}
  }
	else 
	{
		switch(dir)
		{
			case L2R_U2D://������,���ϵ���
				regval|=(1<<5)|(1<<4)|(0<<3); break;
				
			case L2R_D2U://������,���µ���
				regval|=(0<<5)|(1<<4)|(0<<3); break;
				
			case R2L_U2D://���ҵ���,���ϵ���
				regval|=(1<<5)|(0<<4)|(0<<3);break;
				
			case R2L_D2U://���ҵ���,���µ���
				regval|=(0<<5)|(0<<4)|(0<<3);break; 
					 
			case U2D_L2R://���ϵ���,������
				regval|=(1<<5)|(1<<4)|(1<<3);break; 
				
			case U2D_R2L://���ϵ���,���ҵ���
				regval|=(1<<5)|(0<<4)|(1<<3);break; 
				
			case D2U_L2R://���µ���,������
				regval|=(0<<5)|(1<<4)|(1<<3);break; 
				
			case D2U_R2L://���µ���,���ҵ���
				regval|=(0<<5)|(0<<4)|(1<<3);break;
				
			default:break;
		}
		if(lcddev.id==0x8989)//8989 IC
		{
			dirreg=0X11;
			regval|=0X6040;	//65K   
	 	}
		else//��������IC		  
		{
			dirreg=0X03;
			regval|=1<<12;  
		}
		LCD_WriteReg(dirreg,regval);
	}
} 


//-----------------����-----------------
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);//���ù��λ��(x,y:����)
	LCD_WriteRAM_Prepare();	//��ʼд��GRAM
	LCD_WR_DATA(POINT_COLOR); //POINT_COLOR:�˵����ɫ
}


//-----------------���ٻ���-----------------
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)//x,y:����,color:��ɫ
{	   
	if(lcddev.id==0X9341||lcddev.id==0X5310)
	{
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(x>>8); 
		LCD_WR_DATA(x&0XFF);	 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(y>>8); 
		LCD_WR_DATA(y&0XFF);
	}
	else if(lcddev.id==0X5510)
	{
		LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(x>>8);  
		LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(x&0XFF);	  
		LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(y>>8);  
		LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(y&0XFF); 
	}
	else if(lcddev.id==0X6804)
	{		    
		if(lcddev.dir==1)x=lcddev.width-1-x;//����ʱ����
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(x>>8); 
		LCD_WR_DATA(x&0XFF);	 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(y>>8); 
		LCD_WR_DATA(y&0XFF);
	}
	else
	{
 		if(lcddev.dir==1)x=lcddev.width-1-x;//������ʵ���ǵ�תx,y����
		LCD_WriteReg(lcddev.setxcmd,x);
		LCD_WriteReg(lcddev.setycmd,y);
	}	
	LCD_RS_CLR;
 	LCD_CS_CLR; 
	DATAOUT(lcddev.wramcmd);//дָ��  
	LCD_WR_CLR; 
	LCD_WR_SET; 
 	LCD_CS_SET; 
	LCD_WR_DATA(color);		//д����
}


//-----------------����LCD��ʾ����-----------------
void LCD_Display_Dir(u8 dir)//dir:0,������1,����
{
	if(dir==0)//����
	{
		lcddev.dir=0;//����
		lcddev.width=240;
		lcddev.height=320;
		if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310)
		{
			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B;  	 
			if(lcddev.id==0X6804||lcddev.id==0X5310)
			{
				lcddev.width=320;
				lcddev.height=480;
			}
		}
		else if(lcddev.id==0X8989)
		{
			lcddev.wramcmd=R34;
	 		lcddev.setxcmd=0X4E;
			lcddev.setycmd=0X4F;  
		}
		else if(lcddev.id==0x5510)
		{
			lcddev.wramcmd=0X2C00;
	 		lcddev.setxcmd=0X2A00;
			lcddev.setycmd=0X2B00; 
			lcddev.width=480;
			lcddev.height=800;
		}
		else
		{
			lcddev.wramcmd=R34;
	 		lcddev.setxcmd=R32;
			lcddev.setycmd=R33;  
		}
	}
	else//����(��dir==1)
	{	  				
		lcddev.dir=1;//����
		lcddev.width=320;
		lcddev.height=240;
		if(lcddev.id==0X9341||lcddev.id==0X5310)
		{
			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B;  	 
		}
		else if(lcddev.id==0X6804)	 
		{
 			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2B;
			lcddev.setycmd=0X2A; 
		}
		else if(lcddev.id==0X8989)
		{
			lcddev.wramcmd=R34;
	 		lcddev.setxcmd=0X4F;
			lcddev.setycmd=0X4E;   
		}
		else if(lcddev.id==0x5510)
		{
			lcddev.wramcmd=0X2C00;
	 		lcddev.setxcmd=0X2A00;
			lcddev.setycmd=0X2B00; 
			lcddev.width=800;
			lcddev.height=480;
		}
		else
		{
			lcddev.wramcmd=R34;
	 		lcddev.setxcmd=R33;
			lcddev.setycmd=R32;  
		}
		if(lcddev.id==0X6804||lcddev.id==0X5310)
		{ 	 
			lcddev.width=480;
			lcddev.height=320; 			
		}
	} 
	LCD_Scan_Dir(DFT_SCAN_DIR);//Ĭ��ɨ�跽��
}


//-------���ô���,���Զ����û������굽�������Ͻ�(sx,sy)---------
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height)//sx,sy:������ʼ����(���Ͻ�),width,height:���ڿ�Ⱥ͸߶�,�������0
{   
	u8 hsareg,heareg,vsareg,veareg;
	u16 hsaval,heaval,vsaval,veaval; 
	width=sx+width-1;
	height=sy+height-1;
	if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X6804)//6804������֧��
	{
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(sx>>8); 
		LCD_WR_DATA(sx&0XFF);	 
		LCD_WR_DATA(width>>8); 
		LCD_WR_DATA(width&0XFF);  
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(sy>>8); 
		LCD_WR_DATA(sy&0XFF); 
		LCD_WR_DATA(height>>8); 
		LCD_WR_DATA(height&0XFF); 
	}
	else if(lcddev.id==0X5510)
	{
		LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(sx>>8);  
		LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(sx&0XFF);	  
		LCD_WR_REG(lcddev.setxcmd+2);LCD_WR_DATA(width>>8);   
		LCD_WR_REG(lcddev.setxcmd+3);LCD_WR_DATA(width&0XFF);   
		LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(sy>>8);   
		LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(sy&0XFF);  
		LCD_WR_REG(lcddev.setycmd+2);LCD_WR_DATA(height>>8);   
		LCD_WR_REG(lcddev.setycmd+3);LCD_WR_DATA(height&0XFF);  
	}
	else//��������IC
	{
		if(lcddev.dir==1)//����
		{
			hsaval=sy;				
			heaval=height;
			vsaval=lcddev.width-width-1;
			veaval=lcddev.width-sx-1;				
		}
		else
		{ 
			hsaval=sx;				
			heaval=width;
			vsaval=sy;
			veaval=height;
		}
	 	if(lcddev.id==0X8989)//8989 IC
		{
			hsareg=0X44;heareg=0X44;//ˮƽ���򴰿ڼĴ��� (1289����һ���Ĵ�������)
			hsaval|=(heaval<<8);//�õ��Ĵ���ֵ.
			heaval=hsaval;
			vsareg=0X45;veareg=0X46;//��ֱ���򴰿ڼĴ���	  
		}
		else//��������IC
		{
			hsareg=0X50;heareg=0X51;//ˮƽ���򴰿ڼĴ���
			vsareg=0X52;veareg=0X53;//��ֱ���򴰿ڼĴ���	  
		}								  
		LCD_WriteReg(hsareg,hsaval);//���üĴ���ֵ
		LCD_WriteReg(heareg,heaval);
		LCD_WriteReg(vsareg,vsaval);//���üĴ���ֵ
		LCD_WriteReg(veareg,veaval);		
		LCD_SetCursor(sx,sy);	//���ù��λ��
	}
}


//-----------------��ʼ��LCD-----------------
void LCD_Init(void)//�ó�ʼ���������Գ�ʼ�����ֵ�LCDҺ����
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);//ʹ��GPIOB,Cʱ�Ӻ�AFIOʱ��
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);//����SWD��ʧ��JTAG
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_9|GPIO_Pin_8|GPIO_Pin_7|GPIO_Pin_6;//GPIOC6~10�����������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure); //�������õĲ�����ʼ��GPIOC	

	GPIO_SetBits(GPIOC,GPIO_Pin_10|GPIO_Pin_9|GPIO_Pin_8|GPIO_Pin_7|GPIO_Pin_6);//��GPIOC6~10���óɸߵ�ƽ

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;//GPIOB�������
	GPIO_Init(GPIOB, &GPIO_InitStructure);//���ݲ�������GPIOB
 
	GPIO_SetBits(GPIOB,GPIO_Pin_All);//��GPIOB����Ϊ�ߵ�ƽ

	LCD_WriteReg(0x0000,0x0001);
  lcddev.id = LCD_ReadReg(0x0000);   
	if(lcddev.id<0XFF||lcddev.id==0XFFFF||lcddev.id==0X9300)//����ID����ȷ,����lcddev.id==0X9300�жϣ���Ϊ9341��δ����λ������»ᱻ����9300
	{	
		LCD_WR_REG(0XD3);	//����9341 ID�Ķ�ȡ					   
		LCD_RD_DATA();//dummy read 	
 		LCD_RD_DATA();//����0X00
		lcddev.id=LCD_RD_DATA();//��ȡ93								   
 		lcddev.id<<=8;
		lcddev.id|=LCD_RD_DATA();//��ȡ41 	   			   
 		if(lcddev.id!=0X9341)//��9341,�����ǲ���6804
		{	
 			LCD_WR_REG(0XBF);				   
			LCD_RD_DATA();//dummy read 	 
	 		LCD_RD_DATA();//����0X01			   
	 		LCD_RD_DATA();//����0XD0 			  	
	  	lcddev.id=LCD_RD_DATA();//�������0X68 
			lcddev.id<<=8;
	  	lcddev.id|=LCD_RD_DATA();//�������0X04	  
 		}  	
	}
	if(lcddev.id==0X9341)//9341��ʼ��
	{	 
		LCD_WR_REG(0xCF);  
		LCD_WR_DATAX(0x00); 
		LCD_WR_DATAX(0xC1); 
		LCD_WR_DATAX(0X30); 
		LCD_WR_REG(0xED);  
		LCD_WR_DATAX(0x64); 
		LCD_WR_DATAX(0x03); 
		LCD_WR_DATAX(0X12); 
		LCD_WR_DATAX(0X81); 
		LCD_WR_REG(0xE8);  
		LCD_WR_DATAX(0x85); 
		LCD_WR_DATAX(0x10); 
		LCD_WR_DATAX(0x7A); 
		LCD_WR_REG(0xCB);  
		LCD_WR_DATAX(0x39); 
		LCD_WR_DATAX(0x2C); 
		LCD_WR_DATAX(0x00); 
		LCD_WR_DATAX(0x34); 
		LCD_WR_DATAX(0x02); 
		LCD_WR_REG(0xF7);  
		LCD_WR_DATAX(0x20); 
		LCD_WR_REG(0xEA);  
		LCD_WR_DATAX(0x00); 
		LCD_WR_DATAX(0x00); 
		LCD_WR_REG(0xC0);//Power����
		LCD_WR_DATAX(0x1B);//VRH[5:0] 
		LCD_WR_REG(0xC1);//Power����
		LCD_WR_DATAX(0x01);
		LCD_WR_REG(0xC5);//VCM����
		LCD_WR_DATAX(0x30);
		LCD_WR_DATAX(0x30);
		LCD_WR_REG(0xC7);//VCM����2 
		LCD_WR_DATAX(0XB7); 
		LCD_WR_REG(0x36);//�洢���Ĵ�ȡ����
		LCD_WR_DATAX(0x48); 
		LCD_WR_REG(0x3A);   
		LCD_WR_DATAX(0x55); 
		LCD_WR_REG(0xB1);   
		LCD_WR_DATAX(0x00);   
		LCD_WR_DATAX(0x1A); 
		LCD_WR_REG(0xB6);//��ʾ���ܿ���
		LCD_WR_DATAX(0x0A); 
		LCD_WR_DATAX(0xA2); 
		LCD_WR_REG(0xF2);
		LCD_WR_DATAX(0x00); 
		LCD_WR_REG(0x26);
		LCD_WR_DATAX(0x01); 
		LCD_WR_REG(0xE0);
		LCD_WR_DATAX(0x0F); 
		LCD_WR_DATAX(0x2A); 
		LCD_WR_DATAX(0x28); 
		LCD_WR_DATAX(0x08); 
		LCD_WR_DATAX(0x0E); 
		LCD_WR_DATAX(0x08); 
		LCD_WR_DATAX(0x54); 
		LCD_WR_DATAX(0XA9); 
		LCD_WR_DATAX(0x43); 
		LCD_WR_DATAX(0x0A); 
		LCD_WR_DATAX(0x0F); 
		LCD_WR_DATAX(0x00); 
		LCD_WR_DATAX(0x00); 
		LCD_WR_DATAX(0x00); 
		LCD_WR_DATAX(0x00); 		 
		LCD_WR_REG(0XE1);
		LCD_WR_DATAX(0x00); 
		LCD_WR_DATAX(0x15); 
		LCD_WR_DATAX(0x17); 
		LCD_WR_DATAX(0x07); 
		LCD_WR_DATAX(0x11); 
		LCD_WR_DATAX(0x06); 
		LCD_WR_DATAX(0x2B); 
		LCD_WR_DATAX(0x56); 
		LCD_WR_DATAX(0x3C); 
		LCD_WR_DATAX(0x05); 
		LCD_WR_DATAX(0x10); 
		LCD_WR_DATAX(0x0F); 
		LCD_WR_DATAX(0x3F); 
		LCD_WR_DATAX(0x3F); 
		LCD_WR_DATAX(0x0F); 
		LCD_WR_REG(0x2B); 
		LCD_WR_DATAX(0x00);
		LCD_WR_DATAX(0x00);
		LCD_WR_DATAX(0x01);
		LCD_WR_DATAX(0x3f);
		LCD_WR_REG(0x2A); 
		LCD_WR_DATAX(0x00);
		LCD_WR_DATAX(0x00);
		LCD_WR_DATAX(0x00);
		LCD_WR_DATAX(0xef);	 
		LCD_WR_REG(0x11);
		delay_ms(50);
		LCD_WR_REG(0x29);
	}
	else if(lcddev.id==0x9320)//9320��ʼ��
	{
		LCD_WriteReg(0x00,0x0000);
		LCD_WriteReg(0x01,0x0100);
		LCD_WriteReg(0x02,0x0700);
		LCD_WriteReg(0x03,0x1030);
		
		LCD_WriteReg(0x04,0x0000);
		LCD_WriteReg(0x08,0x0202);
		LCD_WriteReg(0x09,0x0000);
		LCD_WriteReg(0x0a,0x0000);
		LCD_WriteReg(0x0c,(1<<0));
		LCD_WriteReg(0x0d,0x0000);
		LCD_WriteReg(0x0f,0x0000);    
		delay_ms(50); 
		LCD_WriteReg(0x07,0x0101);
		delay_ms(50); 								  
		LCD_WriteReg(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));
		LCD_WriteReg(0x11,0x0007);
		LCD_WriteReg(0x12,(1<<8)|(1<<4)|(0<<0));
		LCD_WriteReg(0x13,0x0b00);
		LCD_WriteReg(0x29,0x0000);
	
		LCD_WriteReg(0x2b,(1<<14)|(1<<4));	    
		LCD_WriteReg(0x50,0);
		
		LCD_WriteReg(0x51,239);
		LCD_WriteReg(0x52,0);
		LCD_WriteReg(0x53,319);
	
		LCD_WriteReg(0x60,0x2700);
		LCD_WriteReg(0x61,0x0001);
		LCD_WriteReg(0x6a,0x0000);
	
		LCD_WriteReg(0x80,0x0000);
		LCD_WriteReg(0x81,0x0000);
		LCD_WriteReg(0x82,0x0000);
		LCD_WriteReg(0x83,0x0000);
		LCD_WriteReg(0x84,0x0000);
		LCD_WriteReg(0x85,0x0000);
	
		LCD_WriteReg(0x90,(0<<7)|(16<<0));
		LCD_WriteReg(0x92,0x0000);
		LCD_WriteReg(0x93,0x0001);
		LCD_WriteReg(0x95,0x0110);
		LCD_WriteReg(0x97,(0<<8));
		LCD_WriteReg(0x98,0x0000);   
		LCD_WriteReg(0x07,0x0173);
	}
	LCD_Display_Dir(0);//Ĭ��Ϊ����---------------------------------------------------------------------
	LCD_LED=1;//��������
	LCD_Clear(WHITE);//����ʱ����ɫ����Ϊ��ɫ---------------------------------------------------------
}  		  


//-----------------��������-----------------
void LCD_Clear(u16 color)//color:Ҫ���������ɫ-----------------------------------------------------
{
	u32 index=0;      
	u32 totalpoint=lcddev.width;
	totalpoint*=lcddev.height;//�õ��ܵ���
	if((lcddev.id==0X6804)&&(lcddev.dir==1))//6804������ʱ�����⴦��  
	{						    
 		lcddev.dir=0;	 
 		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  	 			
		LCD_SetCursor(0x00,0x0000);//���ù��λ��  
 		lcddev.dir=1;	 
  	lcddev.setxcmd=0X2B;
		lcddev.setycmd=0X2A;  	 
 	}
	else LCD_SetCursor(0x00,0x0000);//���ù��λ�� 
	LCD_WriteRAM_Prepare();//��ʼд��GRAM	  	  
	for(index=0;index<totalpoint;index++)LCD_WR_DATA(color);	
} 



//------------��ָ��λ����ʾһ���ַ�------------
//x,y:��ʼ����
//num:Ҫ��ʾ���ַ�:" "--->"~"
//size:�����С 12/16/24
//mode:0����ʾ�����(���ǵ�����ʾ);1:��ʾ��䣨��������ʾ��
void LCD_ShowChar(u16 x,u16 y,char num,u8 size,u8 mode)//----------------------------------------------
{  							  
  u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);//�õ�����һ���ַ���Ӧ������ռ���ֽ���	
	//���ô���		   
	num=num-' ';//�õ�ƫ�ƺ��ֵ
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=asc2_1206[num][t];//����1206����
		else if(size==16)temp=asc2_1608[num][t];//����1608����
		else if(size==24)temp=asc2_2412[num][t];//����2412����
		else return;//û�е��ֿ�
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(x>=lcddev.width)return;//��������
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	//��������
				break;
			}
		}  	 
	}  	
}


//------------m^n����------------
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;//����ֵ:m^n�η�
}	


//------------��ʾ����,��λΪ0,����ʾ------------
//x,y :�������,	len :���ֵ�λ�� ,size:�����С
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)//-------------------------------------------------
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}
			else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
} 


//------------��ʾ����,��λΪ0,������ʾ------------
//x,y:�������,len:����(��Ҫ��ʾ��λ��),size:�����С
//mode:0����ʾ�����(���ǵ�����ʾ);1:��ʾ��䣨��������ʾ��
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)//----------------------------------------
{  
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);  
				else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);  
 				continue;
			}
			else enshow=1; 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
} 


//------------��ʾ�ַ���------------
//*p:�ַ�����ʼ��ַ		  
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)//----------------------------------
{         
	u8 x0=x;
	width+=x;
	height+=y;
  while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�
  {       
		if(x>=width){x=x0;y+=size;}
    if(y>=height)break;//�˳�
    LCD_ShowChar(x,y,*p,size,0);
    x+=size/2;
    p++;
  }  
}
















