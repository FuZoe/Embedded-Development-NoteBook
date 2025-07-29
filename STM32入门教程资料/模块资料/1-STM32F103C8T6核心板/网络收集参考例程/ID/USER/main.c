/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The ID application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-10-30
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <string.h>
#include <stdio.h>

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private function prototypes -----------------------------------------------*/
void GetLockCode(void);	 
void USART_Configuration(void);	
FunctionalState SetLockCode(void);
FunctionalState USART_Scanf(void);
FunctionalState TryGetLockCode(void);


/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int main(void)
{
	USART_Configuration();
    printf("*****************************************************************\r\n");
    printf("*                                                               *\r\n");
    printf("*  Thank you for using HY-RedBull V3.0 Development Board ! ^_^  *\r\n");
    printf("*                                                               *\r\n");
    printf("*****************************************************************\r\n");

	GetLockCode();		/*运行加密算法得到ID密码 */
	if( TryGetLockCode() == ENABLE )
	{
		printf("此系统加密字符合! \r\n");
		while(1);
	}
	else
	{
		printf("此系统没有加密字! \r\n");
		/* 程序到达这之后,就是处理盗版系统的办法,可由用户自由发挥 */
		/* ...................................................... */
	}

	printf("请输入加密命令 code或CODE \r\n");
							   
    /* Infinite loop */
    while (1)
	{
	  while( !USART_Scanf() );	  /* 通过串口发送加密命令code或CODE 到HY-Smart,则启动加密字 */
	
	  if( SetLockCode() == ENABLE )
	  {
		printf("已经为此系统设置加密字!请复位系统 \r\n");
		while(1);
	  }
	  else
	  {
	    printf("设置加密字失败! \r\n");
	  }
    }
}

/*******************************************************************************
* Function Name  : USART_Scanf
* Description    : USART Receive
* Input          : None
* Output         : None
* Return         : 返回1成功 返回0失败
* Attention		 : None
*******************************************************************************/
FunctionalState USART_Scanf(void)
{
  uint16_t index = 0;
  uint8_t tmp[4] = {0, 0, 0, 0};

  while (index < 4 )
  {
    /* Loop until RXNE = 1 */
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) ;
  
    tmp[index++] = (USART_ReceiveData(USART1));

	if( tmp[index - 1] == 0x0D ) { index--; continue; }
  }

  if ( !( memcmp(tmp,"code",4)==0 || memcmp(tmp,"CODE",4)==0 ) )
  {
	printf("请输入加密命令 code或CODE \r\n");
    return DISABLE;
  }

  return ENABLE;
}


/* Private variables ---------------------------------------------------------*/
uint32_t CpuID[3];   /* CPU唯一ID  96BIT */
uint32_t Lock_Code;

/* Private define ------------------------------------------------------------*/
#define LOCK_ADDR	0x0801F000		/* 加密字的存储位置Flash页首地址 128K */


/*******************************************************************************
* Function Name  : GetLockCode
* Description    : 设置加密字
* Input          : None
* Output         : None
* Return         : None
* Attention		 : 一般在程序开始的时候调用,如要把加密模式复杂化，建议改进此函数
*******************************************************************************/
void GetLockCode(void)
{
	/* 获取CPU唯一ID */
	CpuID[0]=*(vu32*)(0x1ffff7e8);
	CpuID[1]=*(vu32*)(0x1ffff7ec);
	CpuID[2]=*(vu32*)(0x1ffff7f0);
	/* 加密算法,很简单的加密算法 */
	/* 这段程序比较容易被反汇编，如果需要更复杂的加密方式，应该尽量避免暴露ID位置和加密公式 */
	Lock_Code=(CpuID[0]>>1)+(CpuID[1]>>2)+(CpuID[2]>>3);
}

/*******************************************************************************
* Function Name  : TryGetLockCode
* Description    : 获取加密字
* Input          : None
* Output         : None
* Return         : 返回1成功 返回0失败
* Attention		 : None
*******************************************************************************/
FunctionalState TryGetLockCode(void)
{
	uint32_t CPUID;
	/* 获取加密位 */
	CPUID=*(vu32*)(LOCK_ADDR);
	if(CPUID!=Lock_Code)
	{
		return(DISABLE);
	}
	else
	{
		return(ENABLE);
	}	
}

/*******************************************************************************
* Function Name  : SetLockCode
* Description    : 设置加密字
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
FunctionalState SetLockCode(void)
{
	uint32_t CPUID;

	/* 获取加密位 */
	CPUID=*(vu32*)(LOCK_ADDR);
	/* 写加密位 */
	if(CPUID!=Lock_Code)
	{
		FLASH_Unlock();
		if(CPUID!=0xffffffff)	/* 如果要写入的地址非空,则需要刷除 */
		{
			FLASH_ErasePage(LOCK_ADDR);
		}
		
		FLASH_ProgramWord(LOCK_ADDR,Lock_Code);
		FLASH_Lock();
		
		CPUID=*(vu32*)(LOCK_ADDR);
		if(CPUID==Lock_Code)
		{
			return(ENABLE);
		}
		else
			return(DISABLE);
	}
	else
	{
		return(ENABLE);
	}
}

/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configure USART1 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void USART_Configuration(void)
{ 
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure; 

  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);
  /*
  *  USART1_TX -> PA9 , USART1_RX ->	PA10
  */				
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);		   

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	        
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART1, &USART_InitStructure); 
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  USART_ClearFlag(USART1,USART_FLAG_TC);
  USART_Cmd(USART1, ENABLE);
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

