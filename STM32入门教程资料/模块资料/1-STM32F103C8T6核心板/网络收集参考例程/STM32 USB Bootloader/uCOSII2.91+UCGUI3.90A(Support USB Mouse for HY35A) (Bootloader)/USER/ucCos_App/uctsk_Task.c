/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               uctsk_Task.c
** Descriptions:            The uctsk_Task application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-24
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
#include <includes.h>            
#include "SL811mouse.h"

/* Private variables ---------------------------------------------------------*/
static  OS_STK         App_Task_UCGUI_Stk        [APP_TASK_UCGUI_STK_SIZE];
static  OS_STK         App_Task_TouchPanel_Stk   [APP_TASK_TouchPanel_STK_SIZE];
static  OS_STK         App_TaskBlinkStk          [APP_TASK_BLINK_STK_SIZE];
static  OS_STK         App_TaskMouseStk          [APP_TASK_MOUSE_STK_SIZE];

/* Private function prototypes -----------------------------------------------*/
       void MainTask               (void);
static void uctsk_UCGUI            (void);
static void uctsk_TouchPanel       (void);
static void uctsk_Blink            (void);
static void uctsk_Mouse            (void);
static void LED_GPIO_Configuration (void);
extern void _ExecCalibration       (void);
 
void  App_UCGUI_TaskCreate (void)
{
    CPU_INT08U  os_err;

	os_err = os_err; 

	os_err = OSTaskCreate((void (*)(void *)) uctsk_UCGUI,				
                          (void          * ) 0,							
                          (OS_STK        * )&App_Task_UCGUI_Stk[APP_TASK_UCGUI_STK_SIZE - 1],		
                          (INT8U           ) APP_TASK_UCGUI_PRIO  );							

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_UCGUI_PRIO, "Task UCGUI", &os_err);
	#endif
   
    os_err = OSTaskCreate((void (*)(void *)) uctsk_TouchPanel,				
                          (void          * ) 0,							
                          (OS_STK        * )&App_Task_TouchPanel_Stk[APP_TASK_TouchPanel_STK_SIZE - 1],		
                          (INT8U           ) APP_TASK_TouchPanel_PRIO  );

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_TouchPanel_PRIO, "TASK TouchPanel Messages", &os_err);
	#endif


	os_err = OSTaskCreate((void (*)(void *)) uctsk_Blink,				
                          (void          * ) 0,							
                          (OS_STK        * )&App_TaskBlinkStk[APP_TASK_BLINK_STK_SIZE - 1],		
                          (INT8U           ) APP_TASK_BLINK_PRIO  );							

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_BLINK_PRIO, "Task LED Blink", &os_err);
	#endif

	os_err = OSTaskCreate((void (*)(void *)) uctsk_Mouse,				
                          (void          * ) 0,							
                          (OS_STK        * )&App_TaskMouseStk[APP_TASK_MOUSE_STK_SIZE - 1],		
                          (INT8U           ) APP_TASK_MOUSE_PRIO  );							

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_MOUSE_PRIO, "Task Mouse", &os_err);
	#endif
}							 

static void uctsk_UCGUI (void) 
{ 
   GUI_Init();
   _ExecCalibration();	  /* ´¥ÃþÆÁÐ£×¼   */
   	for(;;)
   	{  
	  MainTask(); 
    }
}

static void uctsk_Blink (void) 
{    
    LED_GPIO_Configuration();
	             
    for(;;)
   	{  
	    /*====LED-ON=======*/
		GPIO_SetBits(GPIOF , GPIO_Pin_6);
		GPIO_SetBits(GPIOF , GPIO_Pin_7);
		GPIO_SetBits(GPIOF , GPIO_Pin_8);
		GPIO_SetBits(GPIOF , GPIO_Pin_9);
		OSTimeDlyHMSM(0, 0, 0, 100);	 /* 100 MS  */ 
		/*====LED-OFF=======*/
		GPIO_ResetBits(GPIOF , GPIO_Pin_6);
		GPIO_ResetBits(GPIOF , GPIO_Pin_7);
		GPIO_ResetBits(GPIOF , GPIO_Pin_8);
		GPIO_ResetBits(GPIOF , GPIO_Pin_9);
		OSTimeDlyHMSM(0, 0, 0, 100);	 /* 100 MS  */
    }
}

static void uctsk_TouchPanel (void) 
{  
    TP_Init();			             /* ´¥ÃþÆÁ³õÊ¼»¯ */ 
    
   	for(;;)
   	{  
	   GUI_TOUCH_Exec();
	   OSTimeDlyHMSM(0, 0, 0, 20);	 /* 20 MS  */
    }
}

static void uctsk_Mouse (void) 
{  
    MouseTask();
}

/*******************************************************************************
* Function Name  : LED_GPIO_Configuration
* Description    : Configure GPIO Pin
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void LED_GPIO_Configuration(void)
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

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
