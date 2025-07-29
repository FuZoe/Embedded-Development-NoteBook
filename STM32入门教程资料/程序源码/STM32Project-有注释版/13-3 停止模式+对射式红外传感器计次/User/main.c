#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "CountSensor.h"

int main(void)
{
	/*模块初始化*/
	OLED_Init();			//OLED初始化
	CountSensor_Init();		//计数传感器初始化
	
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		//开启PWR的时钟
															//停止模式和待机模式一定要记得开启
	
	/*显示静态字符串*/
	OLED_ShowString(1, 1, "Count:");
	
	while (1)
	{
		OLED_ShowNum(1, 7, CountSensor_Get(), 5);			//OLED不断刷新显示CountSensor_Get的返回值
		
		OLED_ShowString(2, 1, "Running");					//OLED闪烁Running，指示当前主循环正在运行
		Delay_ms(100);
		OLED_ShowString(2, 1, "       ");
		Delay_ms(100);
		
		PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);	//STM32进入停止模式，并等待中断唤醒
		SystemInit();										//唤醒后，要重新配置时钟
	}
}
