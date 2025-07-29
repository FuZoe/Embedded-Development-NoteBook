/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            IWDG窗口看门狗实验
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
#include "systick.h"
#include <stdio.h>

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private function prototypes -----------------------------------------------*/
void GPIO_Configuration(void);
void IWDG_Configuration(void);
void USART_Configuration(void);


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
  delay_init();
  GPIO_Configuration();
  IWDG_Configuration(); 
  USART_Configuration();

  printf("*****************************************************************\r\n");
  printf("*                                                               *\r\n");
  printf("*  Thank you for using HY-RedBull V3.0 Development Board ! ^_^  *\r\n");
  printf("*                                                               *\r\n");
  printf("*****************************************************************\r\n");
												  
  /* System Reset */
  if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)  /* 如果系统以为看门狗复位 */
  {	
     printf("IWDG RESET \r\n");
	 RCC_ClearFlag();
  }

  /* Infinite loop */
  while(1)
  {
    /*====LED-ON=======*/
	GPIO_SetBits(GPIOF , GPIO_Pin_6);
	GPIO_SetBits(GPIOF , GPIO_Pin_7);
	GPIO_SetBits(GPIOF , GPIO_Pin_8);
	GPIO_SetBits(GPIOF , GPIO_Pin_9);
    delay_ms(500); /* delay 500ms */ 

	/*====LED-OFF=======*/ 
	GPIO_ResetBits(GPIOF , GPIO_Pin_6);
	GPIO_ResetBits(GPIOF , GPIO_Pin_7);
	GPIO_ResetBits(GPIOF , GPIO_Pin_8);
	GPIO_ResetBits(GPIOF , GPIO_Pin_9);
    delay_ms(500); /* delay 500ms */

    if( GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) )    /* WAKEUP */
    {
		IWDG_ReloadCounter();	   /* 喂狗 */
    }	

    if( !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) )   /* TAMPER */
    {
		IWDG_ReloadCounter();	   /* 喂狗 */
	}
		
    if( !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) )    /* USER */
    {
		IWDG_ReloadCounter();	   /* 喂狗 */
	}	
  }
}

/*******************************************************************************
* Function Name  : IWDG_Configuration
* Description    : 看门狗配置
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void IWDG_Configuration(void)
{
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); 	/* 写入0x5555,用于允许狗狗寄存器写入功能 */
  IWDG_SetPrescaler(IWDG_Prescaler_256);            /* 内部低速时钟256分频 40K/256=156HZ(6.4ms) */ 
  /* 看门狗定时器溢出时间 */
  IWDG_SetReload(781);							    /* 喂狗时间 5s/6.4MS=781 .注意不能大于0xfff*/
  IWDG_ReloadCounter();								/* 喂狗*/
  IWDG_Enable(); 									/* 使能看门狗*/
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

  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOF, ENABLE); 						 
  /**
  *  Tamper Button -> PC13 , Wakeup Button -> PA0 , User Button -> PA8
  */				
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
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
