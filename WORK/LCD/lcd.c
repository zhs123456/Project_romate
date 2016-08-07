#include "lcd.h"
#include "stdlib.h"
#include "font.h" 
#include "delay.h"	 


u16 POINT_COLOR=0x0000;//LCD的画笔颜色（默认为黑色）------------------------------------------
u16 BACK_COLOR=0xFFFF;//LCD的背景色（默认为白色）--------------------------------------------- 


_lcd_dev lcddev;//管理LCD重要参数（默认为竖屏）


//-------写寄存器函数（data:寄存器值）-------
void LCD_WR_REG(u16 data)
{ 
	LCD_RS_CLR;//写地址  
 	LCD_CS_CLR; 
	DATAOUT(data); 
	LCD_WR_CLR; 
	LCD_WR_SET; 
 	LCD_CS_SET;   
}


//------写数据函数（data:寄存器值）------
//可以替代LCD_WR_DATAX宏,拿时间换空间
void LCD_WR_DATAX(u16 data)
{
	LCD_RS_SET;
	LCD_CS_CLR;
	DATAOUT(data);
	LCD_WR_CLR;
	LCD_WR_SET;
	LCD_CS_SET;
}


//-----读LCD数据（返回值:读到的值）---------
u16 LCD_RD_DATA(void)
{										   
	u16 t;
 	GPIOB->CRL=0X88888888; //PB0-7上拉输入
	GPIOB->CRH=0X88888888; //PB8-15上拉输入
	GPIOB->ODR=0X0000;//全部输出0

	LCD_RS_SET;
	LCD_CS_CLR;
	//读取数据(读寄存器时,并不需要读2次)
	LCD_RD_CLR;
	if(lcddev.id==0X8989)
		delay_ms(1);//FOR 8989,延时1ms					   
	t=DATAIN;  
	LCD_RD_SET;
	LCD_CS_SET; 

	GPIOB->CRL=0X33333333;//PB0-7上拉输出
	GPIOB->CRH=0X33333333;//PB8-15上拉输出
	GPIOB->ODR=0XFFFF;//全部输出高
	return t;  
}


//-----------------写寄存器-----------------
void LCD_WriteReg(u16 LCD_Reg,u16 LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);//LCD_Reg:寄存器编号 
	LCD_WR_DATA(LCD_RegValue);//LCD_RegValue:要写入的值	    		 
} 


//-----------------读寄存器-----------------
u16 LCD_ReadReg(u16 LCD_Reg)
{										   
 	LCD_WR_REG(LCD_Reg);//写入要读的寄存器号  
	return LCD_RD_DATA();//返回值:读到的值 
} 


//-----------------开始写GRAM-----------------
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
} 


//-----------------LCD写GRAM-----------------
void LCD_WriteRAM(u16 RGB_Code)
{							    
	LCD_WR_DATA(RGB_Code);//写十六位GRAM（RGB_Code:颜色值）
}


//-----------------通过该函数转换-----------------
//从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
u16 LCD_BGR2RGB(u16 c)
{
	u16  r,g,b,rgb;   
	b=(c>>0)&0x1f;//c:GBR格式的颜色值
	g=(c>>5)&0x3f;
	r=(c>>11)&0x1f;	 
	rgb=(b<<11)+(g<<5)+(r<<0);		 
	return(rgb);//返回值：RGB格式的颜色值
}


//-----------------读取个某点的颜色值	----------------- 
u16 LCD_ReadPoint(u16 x,u16 y)
{
 	u16 r,g,b;
	if(x>=lcddev.width||y>=lcddev.height)return 0;//超过了范围,直接返回（返回值:此点的颜色）		   
	LCD_SetCursor(x,y);//x,y:坐标
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310)LCD_WR_REG(0X2E);//9341/6804/5310发送读GRAM指令
	else if(lcddev.id==0X5510)LCD_WR_REG(0X2E00);	//5510 发送读GRAM指令
	else LCD_WR_REG(R34);//其他IC发送读GRAM指令
	GPIOB->CRL=0X88888888;//PB0-7上拉输入
	GPIOB->CRH=0X88888888;//PB8-15上拉输入
	GPIOB->ODR=0XFFFF;//全部输出高

	LCD_RS_SET;
	LCD_CS_CLR;	    
	
	LCD_RD_CLR;//读取数据(读GRAM时,第一次为假读)	
  delay_ms(1);//延时1ms					   
	LCD_RD_SET;
 	
	LCD_RD_CLR;//dummy READ				   
	delay_ms(1);//延时1ms					   
 	r=DATAIN;//实际坐标颜色
	LCD_RD_SET;
 	if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X5510)	//9341/NT35310/NT35510要分2次读出
	{	 
		LCD_RD_CLR;					   
		b=DATAIN;//读取蓝色值  	  
	 	LCD_RD_SET;
		g=r&0XFF;//对于9341,第一次读取的是RG的值,R在前,G在后,各占8位
		g<<=8;
	}else if(lcddev.id==0X6804)
	{
		LCD_RD_CLR;					   
	 	LCD_RD_SET;
		r=DATAIN;//6804第二次读取的才是真实值 
	}	 
	LCD_CS_SET;
	GPIOB->CRL=0X33333333; //PB0-7  上拉输出
	GPIOB->CRH=0X33333333; //PB8-15 上拉输出
	GPIOB->ODR=0XFFFF; //全部输出高  
	if(lcddev.id==0X9325||lcddev.id==0X4535||lcddev.id==0X4531||lcddev.id==0X8989||lcddev.id==0XB505)return r;	//这几种IC直接返回颜色值
	else if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X5510)return (((r>>11)<<11)|((g>>10)<<5)|(b>>11));//ILI9341/NT35310/NT35510需要公式转换一下
	else return LCD_BGR2RGB(r);//其他IC
}


//-----------------LCD开启显示-----------------
void LCD_DisplayOn(void)
{	
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310)LCD_WR_REG(0X29);//开启显示
	else if(lcddev.id==0X5510)LCD_WR_REG(0X2900);//开启显示
	else LCD_WriteReg(R7,0x0173); //开启显示
}	


//-----------------LCD关闭显示-----------------
void LCD_DisplayOff(void)
{	   
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310)LCD_WR_REG(0X28);//关闭显示
	else if(lcddev.id==0X5510)LCD_WR_REG(0X2800);//关闭显示
	else LCD_WriteReg(R7, 0x0);//关闭显示 
}


//-------设置光标位置（Xpos:横坐标，Ypos:纵坐标）--------
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
		if(lcddev.dir==1)Xpos=lcddev.width-1-Xpos;//横屏时处理
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
		if(lcddev.dir==1)Xpos=lcddev.width-1-Xpos;//横屏其实就是调转x,y坐标
		LCD_WriteReg(lcddev.setxcmd, Xpos);
		LCD_WriteReg(lcddev.setycmd, Ypos);
	}	 	 
}


//-------设置LCD的自动扫描方向(9341/6804两个须注意)-----------
void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	u16 dirreg=0;
	u16 temp;  
	if(lcddev.dir==1&&lcddev.id!=0X6804)//横屏时，对6804不改变扫描方向！
	{			   
		switch(dir)//方向转换
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
	if(lcddev.id==0x9341||lcddev.id==0X6804||lcddev.id==0X5310||lcddev.id==0X5510)//9341/6804/5310/5510,很特殊
	{
		switch(dir)
		{
			case L2R_U2D://从左到右,从上到下
				regval|=(0<<7)|(0<<6)|(0<<5);break; 
				
			case L2R_D2U://从左到右,从下到上
				regval|=(1<<7)|(0<<6)|(0<<5);break; 
				
			case R2L_U2D://从右到左,从上到下
				regval|=(0<<7)|(1<<6)|(0<<5);break; 
				
			case R2L_D2U://从右到左,从下到上
				regval|=(1<<7)|(1<<6)|(0<<5);break; 
					 
			case U2D_L2R://从上到下,从左到右
				regval|=(0<<7)|(0<<6)|(1<<5);break; 
				
			case U2D_R2L://从上到下,从右到左
				regval|=(0<<7)|(1<<6)|(1<<5);break; 
				
			case D2U_L2R://从下到上,从左到右
				regval|=(1<<7)|(0<<6)|(1<<5);break;
				
			case D2U_R2L://从下到上,从右到左
				regval|=(1<<7)|(1<<6)|(1<<5);break; 
					
			default:break;
		}
		if(lcddev.id==0X5510)dirreg=0X3600;
		else dirreg=0X36;
 		if((lcddev.id!=0X5310)&&(lcddev.id!=0X5510))regval|=0X08;//5310/5510不需要BGR   
		if(lcddev.id==0X6804)regval|=0x02;//6804的BIT6和9341的反了	   
		LCD_WriteReg(dirreg,regval);
 		if(regval&0X20)
		{
			if(lcddev.width<lcddev.height)//交换X,Y
			{
				temp=lcddev.width;
				lcddev.width=lcddev.height;
				lcddev.height=temp;
 			}
		}
		else  
		{
			if(lcddev.width>lcddev.height)//交换X,Y
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
			case L2R_U2D://从左到右,从上到下
				regval|=(1<<5)|(1<<4)|(0<<3); break;
				
			case L2R_D2U://从左到右,从下到上
				regval|=(0<<5)|(1<<4)|(0<<3); break;
				
			case R2L_U2D://从右到左,从上到下
				regval|=(1<<5)|(0<<4)|(0<<3);break;
				
			case R2L_D2U://从右到左,从下到上
				regval|=(0<<5)|(0<<4)|(0<<3);break; 
					 
			case U2D_L2R://从上到下,从左到右
				regval|=(1<<5)|(1<<4)|(1<<3);break; 
				
			case U2D_R2L://从上到下,从右到左
				regval|=(1<<5)|(0<<4)|(1<<3);break; 
				
			case D2U_L2R://从下到上,从左到右
				regval|=(0<<5)|(1<<4)|(1<<3);break; 
				
			case D2U_R2L://从下到上,从右到左
				regval|=(0<<5)|(0<<4)|(1<<3);break;
				
			default:break;
		}
		if(lcddev.id==0x8989)//8989 IC
		{
			dirreg=0X11;
			regval|=0X6040;	//65K   
	 	}
		else//其他驱动IC		  
		{
			dirreg=0X03;
			regval|=1<<12;  
		}
		LCD_WriteReg(dirreg,regval);
	}
} 


//-----------------画点-----------------
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);//设置光标位置(x,y:坐标)
	LCD_WriteRAM_Prepare();	//开始写入GRAM
	LCD_WR_DATA(POINT_COLOR); //POINT_COLOR:此点的颜色
}


//-----------------快速画点-----------------
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)//x,y:坐标,color:颜色
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
		if(lcddev.dir==1)x=lcddev.width-1-x;//横屏时处理
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(x>>8); 
		LCD_WR_DATA(x&0XFF);	 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(y>>8); 
		LCD_WR_DATA(y&0XFF);
	}
	else
	{
 		if(lcddev.dir==1)x=lcddev.width-1-x;//横屏其实就是调转x,y坐标
		LCD_WriteReg(lcddev.setxcmd,x);
		LCD_WriteReg(lcddev.setycmd,y);
	}	
	LCD_RS_CLR;
 	LCD_CS_CLR; 
	DATAOUT(lcddev.wramcmd);//写指令  
	LCD_WR_CLR; 
	LCD_WR_SET; 
 	LCD_CS_SET; 
	LCD_WR_DATA(color);		//写数据
}


//-----------------设置LCD显示方向-----------------
void LCD_Display_Dir(u8 dir)//dir:0,竖屏；1,横屏
{
	if(dir==0)//竖屏
	{
		lcddev.dir=0;//竖屏
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
	else//横屏(即dir==1)
	{	  				
		lcddev.dir=1;//横屏
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
	LCD_Scan_Dir(DFT_SCAN_DIR);//默认扫描方向
}


//-------设置窗口,并自动设置画点坐标到窗口左上角(sx,sy)---------
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height)//sx,sy:窗口起始坐标(左上角),width,height:窗口宽度和高度,必须大于0
{   
	u8 hsareg,heareg,vsareg,veareg;
	u16 hsaval,heaval,vsaval,veaval; 
	width=sx+width-1;
	height=sy+height-1;
	if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X6804)//6804横屏不支持
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
	else//其他驱动IC
	{
		if(lcddev.dir==1)//横屏
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
			hsareg=0X44;heareg=0X44;//水平方向窗口寄存器 (1289的由一个寄存器控制)
			hsaval|=(heaval<<8);//得到寄存器值.
			heaval=hsaval;
			vsareg=0X45;veareg=0X46;//垂直方向窗口寄存器	  
		}
		else//其他驱动IC
		{
			hsareg=0X50;heareg=0X51;//水平方向窗口寄存器
			vsareg=0X52;veareg=0X53;//垂直方向窗口寄存器	  
		}								  
		LCD_WriteReg(hsareg,hsaval);//设置寄存器值
		LCD_WriteReg(heareg,heaval);
		LCD_WriteReg(vsareg,vsaval);//设置寄存器值
		LCD_WriteReg(veareg,veaval);		
		LCD_SetCursor(sx,sy);	//设置光标位置
	}
}


//-----------------初始化LCD-----------------
void LCD_Init(void)//该初始化函数可以初始化多种的LCD液晶屏
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);//使能GPIOB,C时钟和AFIO时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);//开启SWD，失能JTAG
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_9|GPIO_Pin_8|GPIO_Pin_7|GPIO_Pin_6;//GPIOC6~10复用推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure); //根据设置的参数初始化GPIOC	

	GPIO_SetBits(GPIOC,GPIO_Pin_10|GPIO_Pin_9|GPIO_Pin_8|GPIO_Pin_7|GPIO_Pin_6);//将GPIOC6~10设置成高电平

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;//GPIOB推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);//根据参数设置GPIOB
 
	GPIO_SetBits(GPIOB,GPIO_Pin_All);//将GPIOB设置为高电平

	LCD_WriteReg(0x0000,0x0001);
  lcddev.id = LCD_ReadReg(0x0000);   
	if(lcddev.id<0XFF||lcddev.id==0XFFFF||lcddev.id==0X9300)//读到ID不正确,新增lcddev.id==0X9300判断，因为9341在未被复位的情况下会被读成9300
	{	
		LCD_WR_REG(0XD3);	//尝试9341 ID的读取					   
		LCD_RD_DATA();//dummy read 	
 		LCD_RD_DATA();//读到0X00
		lcddev.id=LCD_RD_DATA();//读取93								   
 		lcddev.id<<=8;
		lcddev.id|=LCD_RD_DATA();//读取41 	   			   
 		if(lcddev.id!=0X9341)//非9341,尝试是不是6804
		{	
 			LCD_WR_REG(0XBF);				   
			LCD_RD_DATA();//dummy read 	 
	 		LCD_RD_DATA();//读回0X01			   
	 		LCD_RD_DATA();//读回0XD0 			  	
	  	lcddev.id=LCD_RD_DATA();//这里读回0X68 
			lcddev.id<<=8;
	  	lcddev.id|=LCD_RD_DATA();//这里读回0X04	  
 		}  	
	}
	if(lcddev.id==0X9341)//9341初始化
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
		LCD_WR_REG(0xC0);//Power控制
		LCD_WR_DATAX(0x1B);//VRH[5:0] 
		LCD_WR_REG(0xC1);//Power控制
		LCD_WR_DATAX(0x01);
		LCD_WR_REG(0xC5);//VCM控制
		LCD_WR_DATAX(0x30);
		LCD_WR_DATAX(0x30);
		LCD_WR_REG(0xC7);//VCM控制2 
		LCD_WR_DATAX(0XB7); 
		LCD_WR_REG(0x36);//存储器的存取控制
		LCD_WR_DATAX(0x48); 
		LCD_WR_REG(0x3A);   
		LCD_WR_DATAX(0x55); 
		LCD_WR_REG(0xB1);   
		LCD_WR_DATAX(0x00);   
		LCD_WR_DATAX(0x1A); 
		LCD_WR_REG(0xB6);//显示功能控制
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
	else if(lcddev.id==0x9320)//9320初始化
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
	LCD_Display_Dir(0);//默认为竖屏---------------------------------------------------------------------
	LCD_LED=1;//点亮背光
	LCD_Clear(WHITE);//清屏时的颜色设置为白色---------------------------------------------------------
}  		  


//-----------------清屏函数-----------------
void LCD_Clear(u16 color)//color:要清屏的填充色-----------------------------------------------------
{
	u32 index=0;      
	u32 totalpoint=lcddev.width;
	totalpoint*=lcddev.height;//得到总点数
	if((lcddev.id==0X6804)&&(lcddev.dir==1))//6804横屏的时候特殊处理  
	{						    
 		lcddev.dir=0;	 
 		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  	 			
		LCD_SetCursor(0x00,0x0000);//设置光标位置  
 		lcddev.dir=1;	 
  	lcddev.setxcmd=0X2B;
		lcddev.setycmd=0X2A;  	 
 	}
	else LCD_SetCursor(0x00,0x0000);//设置光标位置 
	LCD_WriteRAM_Prepare();//开始写入GRAM	  	  
	for(index=0;index<totalpoint;index++)LCD_WR_DATA(color);	
} 



//------------在指定位置显示一个字符------------
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24
//mode:0：表示不填充(即非叠加显示);1:表示填充（即叠加显示）
void LCD_ShowChar(u16 x,u16 y,char num,u8 size,u8 mode)//----------------------------------------------
{  							  
  u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);//得到字体一个字符对应点阵集所占的字节数	
	//设置窗口		   
	num=num-' ';//得到偏移后的值
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=asc2_1206[num][t];//调用1206字体
		else if(size==16)temp=asc2_1608[num][t];//调用1608字体
		else if(size==24)temp=asc2_2412[num][t];//调用2412字体
		else return;//没有的字库
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(x>=lcddev.width)return;//超区域了
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	//超区域了
				break;
			}
		}  	 
	}  	
}


//------------m^n函数------------
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;//返回值:m^n次方
}	


//------------显示数字,高位为0,则不显示------------
//x,y :起点坐标,	len :数字的位数 ,size:字体大小
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


//------------显示数字,高位为0,还是显示------------
//x,y:起点坐标,len:长度(即要显示的位数),size:字体大小
//mode:0：表示不填充(即非叠加显示);1:表示填充（即叠加显示）
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


//------------显示字符串------------
//*p:字符串起始地址		  
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)//----------------------------------
{         
	u8 x0=x;
	width+=x;
	height+=y;
  while((*p<='~')&&(*p>=' '))//判断是不是非法字符
  {       
		if(x>=width){x=x0;y+=size;}
    if(y>=height)break;//退出
    LCD_ShowChar(x,y,*p,size,0);
    x+=size/2;
    p++;
  }  
}
















