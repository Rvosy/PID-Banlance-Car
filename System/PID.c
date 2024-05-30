/******************************** Copyright (c) ***************************************
 * @file: PID.c
 * @author: Rvosy
 * @date: 2024-05-30
 * @version: 
 * @brief: 
 * @note: 
 *************************************************************************************/

#include "stm32f10x.h"                  // Device header
#include "sys.h"
#include "PID.h"

float ek,ek_last,last_filt_Velocity,filt_Velocity,Velocity_sum;
float target_angle = -1.5;//机械中值
/**
 * @brief 直立环PD计算
 *
 * @param Roll1 车身倾斜角度
 * @return 直立环计算值
 */
float Vertical_PID_value(float Roll1){
		float ek_difference;
		ek = target_angle-Roll1;
		ek_difference= myabs(ek-ek_last);
		ek_last = ek;
		if(Roll1>40 || Roll1<-40) return 0;//角度大于40度,小车可能已经倒下,关闭电机保证安全
		return (AKp*ek+AKd*ek_difference);
}
/**
 * @brief 速度环PI计算
 *
 * @param speed 小车当前速度
 * @return 速度环计算值
 */
float Velocity_PID_value(int speed){
	float a= 0.3;
	filt_Velocity = a*speed + (1-a)*last_filt_Velocity;
	last_filt_Velocity = filt_Velocity;
	Velocity_sum += filt_Velocity;
	xianfu(2000);
	return filt_Velocity * VKp+VKi*Velocity_sum;
}
/**
 * @brief 用于速度环的I限幅
 *
 * @param max I的最大值
 * @note 
 */
void xianfu(int max){
	if(Velocity_sum>max) Velocity_sum = max;
	if(Velocity_sum<-max) Velocity_sum = -max;
}