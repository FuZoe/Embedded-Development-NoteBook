#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "MyRTC.h"

int main(void)
{
	OLED_Init();
	MyRTC_Init();
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	OLED_ShowString(1, 1, "CNT :");
	OLED_ShowString(2, 1, "ALR :");
	OLED_ShowString(3, 1, "ALRF:");
	
	PWR_WakeUpPinCmd(ENABLE);
	
	uint32_t Alarm = RTC_GetCounter() + 10;
	RTC_SetAlarm(Alarm);
	OLED_ShowNum(2, 6, Alarm, 10);
	
	while (1)
	{
		OLED_ShowNum(1, 6, RTC_GetCounter(), 10);
		OLED_ShowNum(3, 6, RTC_GetFlagStatus(RTC_FLAG_ALR), 1);
		
		OLED_ShowString(4, 1, "Running");
		Delay_ms(100);
		OLED_ShowString(4, 1, "       ");
		Delay_ms(100);
		
		OLED_ShowString(4, 9, "STANDBY");
		Delay_ms(1000);
		OLED_ShowString(4, 9, "       ");
		Delay_ms(100);
		
		OLED_Clear();
		
		PWR_EnterSTANDBYMode();
	}
}
