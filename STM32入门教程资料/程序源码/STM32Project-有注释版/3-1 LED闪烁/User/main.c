  #include "stm32f10x.h"                  // Device header
#include "Delay.h"

int main(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//开启GPIOA的时钟
															//使用各个外设前必须开启时钟，否则对外设的操作无效
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;					//定义结构体变量
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//GPIO模式，赋值为推挽输出模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				//GPIO引脚，赋值为第0号引脚
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//GPIO速度，赋值为50MHz
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//将赋值后的构体变量传递给GPIO_Init函数
															//函数内部会自动根据结构体的参数配置相应寄存器
															//实现GPIOA的初始化
	
	/*主循环，循环体内的代码会一直循环执行*/
	while (1)
	{
		/*设置PA0引脚的高低电平，实现LED闪烁，下面展示3种方法*/
		
		/*方法1：GPIO_ResetBits设置低电平，GPIO_SetBits设置高电平*/
		GPIO_ResetBits(GPIOA, GPIO_Pin_0);					//将PA0引脚设置为低电平
		Delay_ms(500);										//延时500ms
		GPIO_SetBits(GPIOA, GPIO_Pin_0);					//将PA0引脚设置为高电平
		Delay_ms(500);										//延时500ms
		
		/*方法2：GPIO_WriteBit设置低/高电平，由Bit_RESET/Bit_SET指定*/
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_RESET);		//将PA0引脚设置为低电平
		Delay_ms(500);										//延时500ms
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);			//将PA0引脚设置为高电平
		Delay_ms(500);										//延时500ms
		
		/*方法3：GPIO_WriteBit设置低/高电平，由数据0/1指定，数据需要强转为BitAction类型*/
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, (BitAction)0);		//将PA0引脚设置为低电平
		Delay_ms(500);										//延时500ms
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, (BitAction)1);		//将PA0引脚设置为高电平
		Delay_ms(500);										//延时500ms
	}
}
