/******************************** Copyright (c) ***************************************
 * @file: main.c
 * @author: Rvosy
 * @date: 2024-05-10
 * @version: V1.0
 * @brief: 
 * @note: 
 *************************************************************************************/
#include "stm32f10x.h"                  // Device header
#include "inv_mpu.h"
#include "Delay.h"
#include "Motor.h"
#include "Encoder.h"
#include "OLED.h"
#include "sys.h"
#include "MyI2C.h"
#include "exti.h"

#define AKp 0 //650
#define AKd  0 //-400
#define VKp 0
#define VKi 0
float ek,ek_last,last_filt_Velocity,filt_Velocity,Velocity_sum;
extern float Velocity_Left,Velocity_Right;
float Pitch,Roll,Yaw;
float target_angle = 0;
float Vertical_PID_value(float Roll1);
void xianfu(int max);
float Velocity_PID_value(int speed);
int main(void)
{	
	Encoder_Init_TIM3();
	Encoder_Init_TIM4();

	delay_init();
	PWM_Init(7199,0);
	OLED_Init();
	MiniBalance_EXTI_Init();
	Motor_Init();
	while(MPU6050_DMP_Init());
	while (1)
	{
		
		OLED_ShowNumber(10,20,Roll,5,12);
		OLED_ShowNumber(40,40,Velocity_sum,5,12);
		OLED_Refresh_Gram();	

		
	}
}

int EXTI15_10_IRQHandler(void) {
	if(EXTI_GetITStatus(EXTI_Line12) == SET){	
		float pwmL,pwmR;	
		float Velocity;
		MPU6050_DMP_Get_Data(&Pitch,&Roll,&Yaw);
		Get_Velocity_Form_Encoder(Read_Encoder(3),Read_Encoder(4));//计数值换算为速度
		Velocity =(Velocity_Left + Velocity_Right)/2;
		

		pwmL = PWM_Limit(Vertical_PID_value(Roll)+Velocity_PID_value(Velocity), 6900, -6900);
		pwmR = PWM_Limit(Vertical_PID_value(Roll)+Velocity_PID_value(Velocity), 6900, -6900);
		Set_Pwm(pwmR,pwmL);
		EXTI_ClearITPendingBit(EXTI_Line12);//清除中断标志位   
	}                        					
	
}

float Vertical_PID_value(float Roll1){
		float ek_difference;
		ek = target_angle-Roll1;
		ek_difference= ek-ek_last;
		ek_last = ek;
		if(Roll1>40 || Roll1<-40) return 0;

		return (AKp*ek+AKd*ek_difference);
}

float Velocity_PID_value(int speed){
	float a= 0.3;
	filt_Velocity = a*speed + (1-a)*last_filt_Velocity;
	last_filt_Velocity = filt_Velocity;
	Velocity_sum += filt_Velocity;
	xianfu(1000);
	return filt_Velocity * VKp+VKi*Velocity_sum;
}

void xianfu(int max){

	if(Velocity_sum>max) Velocity_sum = max;
	if(Velocity_sum<-max) Velocity_sum = -max;
}
