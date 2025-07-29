#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "MyRTC.h"

int main(void)
{
	/*模块初始化*/
	OLED_Init();		//OLED初始化
	MyRTC_Init();		//RTC初始化
	
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		//开启PWR的时钟
															//停止模式和待机模式一定要记得开启
	
	/*显示静态字符串*/
	OLED_ShowString(1, 1, "CNT :");
	OLED_ShowString(2, 1, "ALR :");
	OLED_ShowString(3, 1, "ALRF:");
	
	/*使能WKUP引脚*/
	PWR_WakeUpPinCmd(ENABLE);						//使能位于PA0的WKUP引脚，WKUP引脚上升沿唤醒待机模式
	
	/*设定闹钟*/
	uint32_t Alarm = RTC_GetCounter() + 10;			//闹钟为唤醒后当前时间的后10s
	RTC_SetAlarm(Alarm);							//写入闹钟值到RTC的ALR寄存器
	OLED_ShowNum(2, 6, Alarm, 10);					//显示闹钟值
	
	while (1)
	{
		OLED_ShowNum(1, 6, RTC_GetCounter(), 10);	//显示32位的秒计数器
		OLED_ShowNum(3, 6, RTC_GetFlagStatus(RTC_FLAG_ALR), 1);		//显示闹钟标志位
		
		OLED_ShowString(4, 1, "Running");			//OLED闪烁Running，指示当前主循环正在运行
		Delay_ms(100);
		OLED_ShowString(4, 1, "       ");
		Delay_ms(100);
		
		OLED_ShowString(4, 9, "STANDBY");			//OLED闪烁STANDBY，指示即将进入待机模式
		Delay_ms(1000);
		OLED_ShowString(4, 9, "       ");
		Delay_ms(100);
		
		OLED_Clear();								//OLED清屏，模拟关闭外部所有的耗电设备，以达到极度省电
		
		PWR_EnterSTANDBYMode();						//STM32进入停止模式，并等待指定的唤醒事件（WKUP上升沿或RTC闹钟）
		/*待机模式唤醒后，程序会重头开始运行*/
	}
}
