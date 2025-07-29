#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "CountSensor.h"

int main(void)
{
	/*模块初始化*/
	OLED_Init();			//OLED初始化
	CountSensor_Init();		//计数传感器初始化
	
	/*显示静态字符串*/
	OLED_ShowString(1, 1, "Count:");	//1行1列显示字符串Count:
	
	while (1)
	{
		OLED_ShowNum(1, 7, CountSensor_Get(), 5);		//OLED不断刷新显示CountSensor_Get的返回值
	}
}
