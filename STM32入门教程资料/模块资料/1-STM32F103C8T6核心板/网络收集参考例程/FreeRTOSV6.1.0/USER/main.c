/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The FreeRTOS application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-8
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
#include "FreeRTOS.h"
#include "task.h"


/* Private define ------------------------------------------------------------*/
#define LED_TASK_STACK_SIZE			( configMINIMAL_STACK_SIZE )

#define LED_TASK_PRIORITY			( tskIDLE_PRIORITY + 1 )

/* Private function prototypes -----------------------------------------------*/
static void prvSetupHardware( void );
void GPIO_Configuration(void);
void vLEDTask(void * pvArg);

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
  prvSetupHardware();
  xTaskCreate( vLEDTask , ( signed char * ) NULL , LED_TASK_STACK_SIZE , NULL , LED_TASK_PRIORITY , NULL );
  /* Start the scheduler. */
  vTaskStartScheduler();	

  return 0;
}

/*******************************************************************************
* Function Name  : vLEDTask
* Description    : LED Task
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void vLEDTask(void * pvArg)
{
  GPIO_Configuration();  
  while(1)
  {
    /*====LED-ON=======*/
	GPIO_SetBits(GPIOF , GPIO_Pin_6);
	GPIO_SetBits(GPIOF , GPIO_Pin_7);
	GPIO_SetBits(GPIOF , GPIO_Pin_8);
	GPIO_SetBits(GPIOF , GPIO_Pin_9);
    vTaskDelay(500);	

	/*====LED-OFF=======*/ 
	GPIO_ResetBits(GPIOF , GPIO_Pin_6);
	GPIO_ResetBits(GPIOF , GPIO_Pin_7);
	GPIO_ResetBits(GPIOF , GPIO_Pin_8);
	GPIO_ResetBits(GPIOF , GPIO_Pin_9);
    vTaskDelay(500);	 
  }
}

/*******************************************************************************
* Function Name  : prvSetupHardware
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void prvSetupHardware( void )
{
  /* Configure HCLK clock as SysTick clock source. */
  SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK );
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
