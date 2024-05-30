/******************************** Copyright (c) ***************************************
 * @file: sys.c
 * @author: Rvosy
 * @date: 2024-05-10
 * @version: V0.0
 * @brief: 用于存放功能函数以及系统定义
 * @note: 
 *************************************************************************************/
#include "stm32f10x.h"
#include "sys.h"

/**
  * @brief 绝对值函数
  * @param a 需要计算绝对值的数
  * @retval a 的绝对值
  * @note 
  */
int myabs(int a){ 		   
	int temp;
	if(a<0)  temp=-a;  
	else temp=a;
	return temp;
}

/**************************************************************************
Function: Set JTAG mode
Input   : mode:JTAG, swd mode settings；00，all enable；01，enable SWD；10，Full shutdown
Output  : none
函数功能：设置JTAG模式
入口参数：mode:jtag,swd模式设置;00,全使能;01,使能SWD;10,全关闭;	
返回  值：无
**************************************************************************/
//#define JTAG_SWD_DISABLE   0X02
//#define SWD_ENABLE         0X01
//#define JTAG_SWD_ENABLE    0X00	
void JTAG_Set(u8 mode)
{
	u32 temp;
	temp=mode;
	temp<<=25;
	RCC->APB2ENR|=1<<0;     //开启辅助时钟	   

	AFIO->MAPR&=0XF8FFFFFF; //清除MAPR的[26:24]
	AFIO->MAPR|=temp;       //设置jtag模式
}


//stm32 通用外设初始化方案，方便不同芯片移植
void My_GPIO_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,GPIO_My_TypeDef mode,GPIO_My_TypeDef up_down,GPIO_My_TypeDef speed)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
   
	RCC_APB2PeriphClockCmd(power(2,((uint32_t)GPIOx-(uint32_t)GPIOA)/0x0400+2), ENABLE);
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin;        
 
	if(mode==GPIO_AD_IN) 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
	if(mode==GPIO_FK_IN && up_down==GPIO_P_NO) 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	if(mode==GPIO_FK_IN && up_down==GPIO_P_DOWN)
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  
	if(mode==GPIO_FK_IN && up_down==GPIO_P_UP)
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
	if(mode==GPIO_KL_OUT)
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	if(mode==GPIO_TW_OUT)
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	if(mode==GPIO_KL_AF_OUT)
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	if(mode==GPIO_TW_AF_OUT)		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

 	if(speed==GPIO_2MHz)
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	if(speed==GPIO_25MHz)
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	if(speed==GPIO_50MHz)
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;               

  GPIO_Init(GPIOx, &GPIO_InitStructure);         	
}

u32 power(u32 x,u32 y)
{
		u32 mul = 1;

		u32 i;
		for( i = 0;i<y;i++)
		{
				mul = mul * x;
		}
		return mul;
}

u32 log_2(u32 x)
{
		u8 i=0;
		while(1)
		{
				if(power(2,i)==x)
					return i;
				else
					i++;
		}
}

