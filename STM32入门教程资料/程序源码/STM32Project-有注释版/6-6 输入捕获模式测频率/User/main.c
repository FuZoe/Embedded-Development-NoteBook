#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "PWM.h"
#include "IC.h"

int main(void)
{
	/*模块初始化*/
	OLED_Init();		//OLED初始化
	PWM_Init();			//PWM初始化
	IC_Init();			//输入捕获初始化
	
	/*显示静态字符串*/
	OLED_ShowString(1, 1, "Freq:00000Hz");		//1行1列显示字符串Freq:00000Hz
	
	/*使用PWM模块提供输入捕获的测试信号*/
	PWM_SetPrescaler(720 - 1);					//PWM频率Freq = 72M / (PSC + 1) / 100
	PWM_SetCompare1(50);						//PWM占空比Duty = CCR / 100
	
	while (1)
	{
		OLED_ShowNum(1, 6, IC_GetFreq(), 5);	//不断刷新显示输入捕获测得的频率
	}
}
