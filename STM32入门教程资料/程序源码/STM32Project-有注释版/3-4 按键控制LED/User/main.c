#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
#include "Key.h"

uint8_t KeyNum;		//定义用于接收按键键码的变量

int main(void)
{
	/*模块初始化*/
	LED_Init();		//LED初始化
	Key_Init();		//按键初始化
	
	while (1)
	{
		KeyNum = Key_GetNum();		//获取按键键码
		
		if (KeyNum == 1)			//按键1按下
		{
			LED1_Turn();			//LED1翻转
		}
		
		if (KeyNum == 2)			//按键2按下
		{
			LED2_Turn();			//LED2翻转
		}
	}
}
