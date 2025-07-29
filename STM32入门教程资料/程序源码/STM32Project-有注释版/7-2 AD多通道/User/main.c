#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "AD.h"

uint16_t AD0, AD1, AD2, AD3;	//定义AD值变量

int main(void)
{
	/*模块初始化*/
	OLED_Init();				//OLED初始化
	AD_Init();					//AD初始化
	
	/*显示静态字符串*/
	OLED_ShowString(1, 1, "AD0:");
	OLED_ShowString(2, 1, "AD1:");
	OLED_ShowString(3, 1, "AD2:");
	OLED_ShowString(4, 1, "AD3:");
	
	while (1)
	{
		AD0 = AD_GetValue(ADC_Channel_0);		//单次启动ADC，转换通道0
		AD1 = AD_GetValue(ADC_Channel_1);		//单次启动ADC，转换通道1
		AD2 = AD_GetValue(ADC_Channel_2);		//单次启动ADC，转换通道2
		AD3 = AD_GetValue(ADC_Channel_3);		//单次启动ADC，转换通道3
		
		OLED_ShowNum(1, 5, AD0, 4);				//显示通道0的转换结果AD0
		OLED_ShowNum(2, 5, AD1, 4);				//显示通道1的转换结果AD1
		OLED_ShowNum(3, 5, AD2, 4);				//显示通道2的转换结果AD2
		OLED_ShowNum(4, 5, AD3, 4);				//显示通道3的转换结果AD3
		
		Delay_ms(100);			//延时100ms，手动增加一些转换的间隔时间
	}
}
