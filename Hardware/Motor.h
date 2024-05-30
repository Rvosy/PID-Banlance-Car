/******************************** Copyright (c) ***************************************
 * @file: Motor.h
 * @author: Rvosy
 * @date: 2024-05-10
**************************************************************************************/
#ifndef __MOTOR_H
#define __MOTOR_H

#define PWMA   TIM1->CCR1  //PA8
#define PWMB   TIM1->CCR4  //PA11
#define AIN1   GPIO_Pin_14
#define AIN2   GPIO_Pin_15
#define BIN1   GPIO_Pin_13
#define BIN2   GPIO_Pin_12

void Motor_Init(void);
void PWM_Init(u16 arr,u16 psc);
void Set_Pwm(int motor_left,int motor_right);
float PWM_Limit(float IN,int max,int min);

#endif