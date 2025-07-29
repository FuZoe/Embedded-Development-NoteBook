#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"

int main(void)
{
	/*模块初始化*/
	OLED_Init();						//OLED初始化
	
	Serial_Init();						//串口初始化
	
	/*串口基本函数*/
	Serial_SendByte(0x41);				//串口发送一个字节数据0x41
	
	uint8_t MyArray[] = {0x42, 0x43, 0x44, 0x45};	//定义数组
	Serial_SendArray(MyArray, 4);		//串口发送一个数组
	
	Serial_SendString("\r\nNum1=");		//串口发送字符串
	
	Serial_SendNumber(111, 3);			//串口发送数字
	
	/*下述3种方法可实现printf的效果*/
	
	/*方法1：直接重定向printf，但printf函数只有一个，此方法不能在多处使用*/
	printf("\r\nNum2=%d", 222);			//串口发送printf打印的格式化字符串
										//需要重定向fputc函数，并在工程选项里勾选Use MicroLIB
	
	/*方法2：使用sprintf打印到字符数组，再用串口发送字符数组，此方法打印到字符数组，之后想怎么处理都可以，可在多处使用*/
	char String[100];					//定义字符数组
	sprintf(String, "\r\nNum3=%d", 333);//使用sprintf，把格式化字符串打印到字符数组
	Serial_SendString(String);			//串口发送字符数组（字符串）
	
	/*方法3：将sprintf函数封装起来，实现专用的printf，此方法就是把方法2封装起来，更加简洁实用，可在多处使用*/
	Serial_Printf("\r\nNum4=%d", 444);	//串口打印字符串，使用自己封装的函数实现printf的效果
	Serial_Printf("\r\n");
	
	while (1)
	{
		
	}
}
