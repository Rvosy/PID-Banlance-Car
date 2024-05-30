/******************************** Copyright (c) ***************************************
 * @file: encoder.c
 * @author: Rvosy
 * @date: 2024-05-10
 * @version: V1.0
 * @brief: 编码器驱动文件
 * @note: 
 *************************************************************************************/
#include "stm32f10x.h"
#include "Encoder.h"
#include "sys.h"
float Velocity_Left,Velocity_Right;     //车轮速度(mm/s)

/**
  * @brief 把TIM3初始化为编码器接口模式
  * @note 
  */
void Encoder_Init_TIM3(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
    TIM_ICInitTypeDef TIM_ICInitStructure;  
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//使能定时器3的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能PA端口时钟
        
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;	//端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);					      //根据设定参数初始化GPIOA
    
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0; // 预分频器 
    TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD; //设定计数器自动重装值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//选择时钟分频：不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM向上计数  
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//使用编码器模式3
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;	//滤波10
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);//清除TIM的更新标志位
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    
    TIM_SetCounter(TIM3,0);//初始化计数器为0
    TIM_Cmd(TIM3, ENABLE); 
}

/**
  * @brief 把TIM3初始化为编码器接口模式
  * @note 
  */
void Encoder_Init_TIM4(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
    TIM_ICInitTypeDef TIM_ICInitStructure;  
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//使能定时器4的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能PB端口时钟
        
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;	//端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);					      //根据设定参数初始化GPIOB
    
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // 预分频器 
    TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD; //设定计数器自动重装值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//选择时钟分频：不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM向上计数  
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//使用编码器模式3
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;
    TIM_ICInit(TIM4, &TIM_ICInitStructure);
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);//清除TIM的更新标志位
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

    TIM_SetCounter(TIM4,0);
    TIM_Cmd(TIM4, ENABLE); 
}

/**
  * @brief 单位时间读取编码器计数
  * @param TIMX 定时器编号 3~4
  * @retval 速度值
  * @note 
  */
int Read_Encoder(u8 TIMX)
{
   int Encoder_TIM;    
   switch(TIMX){
	   case 2:  Encoder_TIM= (short)TIM2 -> CNT;  TIM2 -> CNT=0;break;
		 case 3:  Encoder_TIM= (short)TIM3 -> CNT;  TIM3 -> CNT=0;break;	
		 case 4:  Encoder_TIM= (short)TIM4 -> CNT;  TIM4 -> CNT=0;break;	
		 default: Encoder_TIM=0;
	 }
		return Encoder_TIM;
}

/**
  * @brief 编码器读数转换为速度（mm/s）
  * @param encoder_left 左侧电机编码器读数
  * @param encoder_right 右侧电机编码器读数
  * @note 
  */
void Get_Velocity_Form_Encoder(int encoder_left,int encoder_right)
{ 	
	float Rotation_Speed_L,Rotation_Speed_R;						//电机转速  转速=编码器读数（5ms每次）*读取频率/倍频数/减速比/编码器精度
	Rotation_Speed_L = encoder_left*Control_Frequency/EncoderMultiples/Reduction_Ratio/Encoder_precision;
	Velocity_Left = Rotation_Speed_L*PI*Diameter_67;		//求出编码器速度=转速*周长
	Rotation_Speed_R = encoder_right*Control_Frequency/EncoderMultiples/Reduction_Ratio/Encoder_precision;
	Velocity_Right = Rotation_Speed_R*PI*Diameter_67;		//求出编码器速度=转速*周长
}

/**
  * @brief TIM3中断服务函数
  * @note 
  */
void TIM3_IRQHandler(void)
{ 		    		  			    
	if(TIM3->SR&0X0001)//溢出中断
	{    				   				     	    	
	}				   
	TIM3->SR&=~(1<<0);//清除中断标志位 	    
}

/**
  * @brief TIM4中断服务函数
  * @note 
  */
void TIM4_IRQHandler(void)
{ 		    		  			    
	if(TIM4->SR&0X0001)//溢出中断
	{    				   				     	    	
	}				   
	TIM4->SR&=~(1<<0);//清除中断标志位 	    
}



