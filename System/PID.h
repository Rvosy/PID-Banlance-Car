/******************************** Copyright (c) ***************************************
 * @file: PID.h
 * @author: Rvosy
 * @date: 2024-05-30
 * @version: V1.0
 * @brief: 
 * @note: 
 *************************************************************************************/
#ifndef __PID_H
#define __PID_H

//PID参数宏定义
#define AKp 360 //600
#define AKd -500 //-800
#define VKp -0//-2
#define VKi -0//-0.01

float Vertical_PID_value(float Roll1);
float Velocity_PID_value(int speed);
void xianfu(int max);


#endif // __PID_H