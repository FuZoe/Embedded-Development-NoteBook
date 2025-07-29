/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The systick application function
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

/* Private function prototypes -----------------------------------------------*/
void GPIO_Configuration(void);


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
    GPIO_Configuration();
    delay_init();	  
    /* Infinite loop */
    while (1){
		/*====LED-ON=======*/
		GPIO_SetBits(GPIOF , GPIO_Pin_6);
		GPIO_SetBits(GPIOF , GPIO_Pin_7);
		GPIO_SetBits(GPIOF , GPIO_Pin_8);
		GPIO_SetBits(GPIOF , GPIO_Pin_9);
		delay_ms(1000);   /* delay 1000ms */
		/*====LED-OFF=======*/
		GPIO_ResetBits(GPIOF , GPIO_Pin_6);
		GPIO_ResetBits(GPIOF , GPIO_Pin_7);
		GPIO_ResetBits(GPIOF , GPIO_Pin_8);
		GPIO_ResetBits(GPIOF , GPIO_Pin_9);
		delay_us(1000*1000);  /* delay 1000ms */					
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
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOF , ENABLE); 						 
/**
 *  LED1 -> PF6 , LED2 -> PF7 , LED3 -> PF8 , LED4 -> PF9
 */					 
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOF, &GPIO_InitStructure);
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
