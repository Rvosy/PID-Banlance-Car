/******************************** Copyright (c) ***************************************
 * @file: encoder.h
 * @author: Rvosy
 * @date: 2024-05-10
**************************************************************************************/
#ifndef __ENCODER_H
#define __ENCODER_H

#define PI 3.14159265							//PI圆周率
#define Control_Frequency  100.0	//编码器读取频率
#define Diameter_67  67.0 				//轮子直径67mm 
#define EncoderMultiples   4.0 		//编码器倍频数
#define Encoder_precision  13.0 	//编码器精度 13线
#define Reduction_Ratio  30.0			//减速比30
#define Perimeter  210.4867 			//周长，单位mm
#define ENCODER_TIM_PERIOD (u16)(65535)   //不可大于65535 因为F103的定时器是16位的。



void Encoder_Init_TIM3(void);
void Encoder_Init_TIM4(void);
int Read_Encoder(u8 TIMX);//读取编码器计数值
void Get_Velocity_Form_Encoder(int encoder_left,int encoder_right);//计数值换算为速度

void TIM3_IRQHandler(void);
void TIM4_IRQHandler(void);

#endif