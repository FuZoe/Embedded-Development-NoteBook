#include "stm32f10x.h"                  // Device header
#include "PWM.h"

/**
  * 函    数：舵机初始化
  * 参    数：无
  * 返 回 值：无
  */
void Servo_Init(void)
{
	PWM_Init();									//初始化舵机的底层PWM
}

/**
  * 函    数：舵机设置角度
  * 参    数：Angle 要设置的舵机角度，范围：0~180
  * 返 回 值：无
  */
void Servo_SetAngle(float Angle)
{
	PWM_SetCompare2(Angle / 180 * 2000 + 500);	//设置占空比
												//将角度线性变换，对应到舵机要求的占空比范围上
}
