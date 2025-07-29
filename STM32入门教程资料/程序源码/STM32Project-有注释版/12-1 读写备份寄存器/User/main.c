#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Key.h"

uint8_t KeyNum;					//定义用于接收按键键码的变量

uint16_t ArrayWrite[] = {0x1234, 0x5678};	//定义要写入数据的测试数组
uint16_t ArrayRead[2];						//定义要读取数据的测试数组

int main(void)
{
	/*模块初始化*/
	OLED_Init();				//OLED初始化
	Key_Init();					//按键初始化
	
	/*显示静态字符串*/
	OLED_ShowString(1, 1, "W:");
	OLED_ShowString(2, 1, "R:");
	
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		//开启PWR的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);		//开启BKP的时钟
	
	/*备份寄存器访问使能*/
	PWR_BackupAccessCmd(ENABLE);							//使用PWR开启对备份寄存器的访问
	
	while (1)
	{
		KeyNum = Key_GetNum();		//获取按键键码
		
		if (KeyNum == 1)			//按键1按下
		{
			ArrayWrite[0] ++;		//测试数据自增
			ArrayWrite[1] ++;
			
			BKP_WriteBackupRegister(BKP_DR1, ArrayWrite[0]);	//写入测试数据到备份寄存器
			BKP_WriteBackupRegister(BKP_DR2, ArrayWrite[1]);
			
			OLED_ShowHexNum(1, 3, ArrayWrite[0], 4);		//显示写入的测试数据
			OLED_ShowHexNum(1, 8, ArrayWrite[1], 4);
		}
		
		ArrayRead[0] = BKP_ReadBackupRegister(BKP_DR1);		//读取备份寄存器的数据
		ArrayRead[1] = BKP_ReadBackupRegister(BKP_DR2);
		
		OLED_ShowHexNum(2, 3, ArrayRead[0], 4);				//显示读取的备份寄存器数据
		OLED_ShowHexNum(2, 8, ArrayRead[1], 4);
	}
}
