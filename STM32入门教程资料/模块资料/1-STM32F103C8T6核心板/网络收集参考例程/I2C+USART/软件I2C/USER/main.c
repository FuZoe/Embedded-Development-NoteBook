/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The EEPROM 24C02 application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-10-29
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
#include "24C02.h"
#include <stdio.h>
#include <string.h>

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private function prototypes -----------------------------------------------*/
void GPIO_Configuration(void);
void USART_Configuration(void);

/*******************************************************************************
* Function Name  : Delay
* Description    : Delay Time
* Input          : - nCount: Delay Time
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void  Delay (uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

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
    uint16_t Addr;
	uint8_t WriteBuffer[256],ReadBuffer[256];
	
	GPIO_Configuration();
	USART_Configuration();
	I2C_Configuration();
	printf("\r\n");
	printf("******************************************************************\r\n");
        printf("*                                                                *\r\n");
	printf("*   Thank you for using HY-RedBull V3.0 Development Board ！^_^  *\r\n");
	printf("*                                                                *\r\n");
	printf("******************************************************************\r\n");

	for(Addr=0; Addr<256; Addr++)
	WriteBuffer[Addr]=Addr;	 /* 填充WriteBuffer */

	/* 开始向EEPROM写数据 */
   	printf("\r\n EEPROM 24C02 Write Test \r\n");
	for(Addr=0; Addr<256; Addr++)
	while( !I2C_WriteByte(WriteBuffer[Addr], Addr,  ADDR_24LC02) );
   	printf("\r\n EEPROM Write Test OK \r\n");

   	/* EEPROM读数据 */
	printf("\r\n EEPROM 24C02 Read Test \r\n");
	I2C_ReadByte(ReadBuffer, sizeof(WriteBuffer),0, ADDR_24LC02);

	if(  memcmp(WriteBuffer,ReadBuffer,sizeof(WriteBuffer)) == 0 ) /* 匹配数据 */
	printf("\r\n EEPROM 24C02 Read Test OK\r\n");
	else
	printf("\r\n EEPROM 24C02 Read Test False\r\n");

    /* Infinite loop */
    while (1)
	{
	  /*====LED-ON=======*/
	  GPIO_SetBits(GPIOF , GPIO_Pin_6);
	  GPIO_SetBits(GPIOF , GPIO_Pin_7);
	  GPIO_SetBits(GPIOF , GPIO_Pin_8);
	  GPIO_SetBits(GPIOF , GPIO_Pin_9);
	  Delay(0xfffff);
	  Delay(0xfffff);
	  /*====LED-OFF=======*/ 
	  GPIO_ResetBits(GPIOF , GPIO_Pin_6);
	  GPIO_ResetBits(GPIOF , GPIO_Pin_7);
	  GPIO_ResetBits(GPIOF , GPIO_Pin_8);
	  GPIO_ResetBits(GPIOF , GPIO_Pin_9);
	  Delay(0xfffff);
	  Delay(0xfffff);
    }
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOF , ENABLE); 						 
/**
 *  LED1 -> PF6 , LED2 -> PF7 , LED3 -> PF8 , LED4 -> PF9
 */					 
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOF, &GPIO_InitStructure);
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

  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
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
