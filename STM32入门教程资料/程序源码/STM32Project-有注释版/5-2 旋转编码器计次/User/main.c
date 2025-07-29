#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Encoder.h"

int16_t Num;			//定义待被旋转编码器调节的变量

int main(void)
{
	/*模块初始化*/
	OLED_Init();		//OLED初始化
	Encoder_Init();		//旋转编码器初始化
	
	/*显示静态字符串*/
	OLED_ShowString(1, 1, "Num:");			//1行1列显示字符串Num:
	
	while (1)
	{
		Num += Encoder_Get();				//获取自上此调用此函数后，旋转编码器的增量值，并将增量值加到Num上
		OLED_ShowSignedNum(1, 5, Num, 5);	//显示Num
	}
}
