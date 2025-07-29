/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            WWDG窗口看门狗实验
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
void NVIC_Configuration(void);
void WWDG_Configuration(void);
void USART_Configuration(void);

/* Private variables ---------------------------------------------------------*/
FunctionalState Feed_Dog=DISABLE;

/*******************************************************************************
* Function Name  : main
* Description    : Main Programme
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int main(void)
{
    delay_init();
    GPIO_Configuration();
	NVIC_Configuration();
	WWDG_Configuration();
	USART_Configuration();
	
    printf("*****************************************************************\r\n");
    printf("*                                                               *\r\n");
    printf("*  Thank you for using HY-RedBull V3.0 Development Board ! ^_^  *\r\n");
    printf("*                                                               *\r\n");
    printf("*****************************************************************\r\n");
		
	if(RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET)  /* 如果系统以为看门狗复位 */
	{	
	  printf("WWDG RESET \r\n");
	  RCC_ClearFlag();
	}
		  
    /* Infinite loop */
    while (1)
	{

	  Feed_Dog = ENABLE;
	  
      if( GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) )     /* WAKEUP */
      {
		delay_ms(1000);  
      }	

      if( !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) )   /* TAMPER */
      {
		delay_ms(1000); 
	  }	
		
      if( !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) )    /* USER */
      {
		WWDG_SetCounter(0x7F); 
	  }
							
    }
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configure GPIO Pin
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

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures the nested vectored interrupt controller.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* Function Name  : WWDG_Configuration
* Description    : Configures WWDG.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void WWDG_Configuration(void)
{
	/* 窗口看门狗时钟允许 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
	
	/* 看门狗节拍 = (36M/4096)/8 = 1098 Hz*/
	WWDG_SetPrescaler(WWDG_Prescaler_8);
	
	/* 窗口值用0x42*/
	WWDG_SetWindowValue(0x42);
	
	/*看门狗使能并初始化定时器为0x7f*/
	WWDG_Enable(0x7F);
	
	/* Clear EWI flag */
	WWDG_ClearFlag();
	
	/* Enable EW interrupt */
	WWDG_EnableIT();
}

/*******************************************************************************
* Function Name  : WWDG_IRQHandler
* Description    : This function handles WWDG interrupt request.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void WWDG_IRQHandler(void)
{
	static uint8_t LED_Status;
	
	/* 喂狗 */
	if(Feed_Dog==ENABLE)
	{
		WWDG_SetCounter(0x7F);
		Feed_Dog = DISABLE;
	}
	
	WWDG_ClearFlag();	 /* 清中断标志*/
  	
	/* 每进入此中断,LED翻转一次 */
	if(LED_Status)
	{
	  GPIO_SetBits(GPIOF , GPIO_Pin_6);
	  GPIO_SetBits(GPIOF , GPIO_Pin_7);
	  GPIO_SetBits(GPIOF , GPIO_Pin_8);
	  GPIO_SetBits(GPIOF , GPIO_Pin_9);
	}
	else
	{
	  GPIO_ResetBits(GPIOF , GPIO_Pin_6);
	  GPIO_ResetBits(GPIOF , GPIO_Pin_7);
	  GPIO_ResetBits(GPIOF , GPIO_Pin_8);
	  GPIO_ResetBits(GPIOF , GPIO_Pin_9);
	}
    LED_Status = ~LED_Status;
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
