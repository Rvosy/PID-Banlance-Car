/******************************** Copyright (c) ***************************************
 * @file: Motor.c
 * @author: Rvosy
 * @date: 2024-05-10
 * @version: V1.0
 * @brief: 电机的驱动控制文件
 * @note: 
 *************************************************************************************/
#include "stm32f10x.h"
#include "Motor.h"
#include "sys.h"


/**
  * @brief  初始化四个GPIO用于控制两个电机的正反转
  * @note 
  */
void Motor_Init(void){
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能PB端口时钟
    GPIO_InitStructure.GPIO_Pin = AIN1|AIN2|BIN1|BIN2;	  //端口配置 PB12, PB13, PB14, PB15
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      //推挽输出
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
  * @brief 初始化PWM参数用于控制电机的快慢
  * @param arr 自动重装值
  * @param psc 预分频系数 - 0为不分频
  * @note 频率 = CK_Psc/(psc+1)/(arr+1)---CK_Psc一般为72M
  */
void PWM_Init(u16 arr,u16 psc){
	  GPIO_InitTypeDef GPIO_InitStructure;
	  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	  TIM_OCInitTypeDef  TIM_OCInitStructure;
    Motor_Init();
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
 	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);

    //设置该引脚为复用输出功能,用于输出TIM1 CH1 CH4的PWM脉冲波形
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_11; //TIM_CH1 //TIM_CH4
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 
    TIM_TimeBaseStructure.TIM_Prescaler = psc; //设置用来作为TIMx时钟频率除数的预分频值  不分频
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

  
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_Pulse = 0;                            //设置待装入捕获比较寄存器的脉冲值
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //输出极性:TIM输出比较极性高
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx

    TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE 主输出使能	
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH1预装载使能	 
    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH4预装载使能	 
    TIM_ARRPreloadConfig(TIM1, ENABLE); //使能TIMx在ARR上的预装载寄存器
    
    TIM_Cmd(TIM1, ENABLE);  //使能TIM1
} 

/**
  * @brief 设置左右电机转动的速度
  * @param motor_left 
  * @param motor_right 
  * @note 通过改变占空比(TIM1->CCR的值)来改变速度
  */
void Set_Pwm(int motor_left,int motor_right)
{
    //左轮
    if(motor_left>0){
        GPIO_ResetBits(GPIOB, AIN2);
        GPIO_SetBits(GPIOB, AIN1);
    }else{
        GPIO_ResetBits(GPIOB, AIN1);
        GPIO_SetBits(GPIOB, AIN2);
    } PWMA=myabs(motor_left);

    //右轮
    if(motor_right>0){
        GPIO_ResetBits(GPIOB, BIN2);
        GPIO_SetBits(GPIOB, BIN1);
    }else{
        GPIO_ResetBits(GPIOB, BIN1);
        GPIO_SetBits(GPIOB, BIN2);
    } PWMB=myabs(motor_right);

}

/**
  * @brief 限制PWM的数值
  * @param IN PWM初始值
  * @param max 限幅最大值
  * @param min 限幅最小值
  * @retval PWM限幅后的数值
  */
float PWM_Limit(float IN,int max,int min){
	float OUT = IN;
	if(OUT>max) OUT = max;
	if(OUT<min) OUT = min;
	return OUT;
}