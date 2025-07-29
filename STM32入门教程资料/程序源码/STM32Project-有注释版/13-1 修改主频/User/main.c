#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"

int main(void)
{
	OLED_Init();							//OLED初始化
	
	OLED_ShowString(1, 1, "SYSCLK:");		//显示静态字符串
	OLED_ShowNum(1, 8, SystemCoreClock, 8);	//显示SystemCoreClock变量
											//SystemCoreClock的值表示当前的系统主频频率
	
	while (1)
	{
		OLED_ShowString(2, 1, "Running");	//闪烁Running，指示当前主循环运行的快慢
		Delay_ms(500);
		OLED_ShowString(2, 1, "       ");
		Delay_ms(500);
	}
}
