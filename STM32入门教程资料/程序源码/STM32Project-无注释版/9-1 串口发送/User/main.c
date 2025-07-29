#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"

int main(void)
{
	OLED_Init();
	
	Serial_Init();
	
	Serial_SendByte(0x41);
	
	uint8_t MyArray[] = {0x42, 0x43, 0x44, 0x45};
	Serial_SendArray(MyArray, 4);
	
	Serial_SendString("\r\nNum1=");
	
	Serial_SendNumber(111, 3);
	
	printf("\r\nNum2=%d", 222);
	
	char String[100];
	sprintf(String, "\r\nNum3=%d", 333);
	Serial_SendString(String);
	
	Serial_Printf("\r\nNum4=%d", 444);
	Serial_Printf("\r\n");
	
	while (1)
	{
		
	}
}
