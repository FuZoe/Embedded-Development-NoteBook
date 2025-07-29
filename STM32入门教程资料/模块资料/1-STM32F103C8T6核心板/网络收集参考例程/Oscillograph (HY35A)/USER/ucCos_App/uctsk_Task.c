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
#include "dso.h"
#include "key.h"

/* Private variables ---------------------------------------------------------*/
static  OS_STK         App_Task_DSO_RUN_Stk      [APP_TASK_DSO_RUN_STK_SIZE];
static  OS_STK         App_Task_Key_Stk          [APP_TASK_KEY_STK_SIZE];
static  OS_STK         App_Task_Blink_Stk        [APP_TASK_BLINK_STK_SIZE];
static  OS_STK         App_Task_DisplayDso_Stk	 [APP_TASK_DISPLAYDSO_STK_SIZE];
static  OS_STK         App_Task_LCDRefresh_Stk	 [APP_TASK_LCDREFRESH_STK_SIZE];
static  OS_STK         App_Task_DSO_HELP_Stk	 [APP_TASK_DSO_HELP_STK_SIZE];

extern  DSO   g_DSO;
/* OS计数信号量 */	
OS_EVENT *DSOHelp;
OS_EVENT *DSORun;
OS_EVENT *LCDRefresh;	

/* Private function prototypes -----------------------------------------------*/
static void uctsk_DSO_Run        (void);
static void uctsk_Key              (void);
static void uctsk_Blink            (void);
static void uctsk_LCDRefresh       (void); 
static void uctsk_DisplayDso       (void);
static void uctsk_UCGUI_DSO_Help   (void);
static void LED_GPIO_Configuration (void);


void  App_DSO_TaskCreate (void)
{
    CPU_INT08U  os_err;

	os_err = os_err; 

	os_err = OSTaskCreate((void (*)(void *)) uctsk_DSO_Run,				
                          (void          * ) 0,							
                          (OS_STK        * )&App_Task_DSO_RUN_Stk[APP_TASK_DSO_RUN_STK_SIZE - 1],		
                          (INT8U           ) APP_TASK_DSO_RUN_PRIO  );							

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_DSO_RUN_PRIO, "Task DSO Run", &os_err);
	#endif
   
    os_err = OSTaskCreate((void (*)(void *)) uctsk_Key,				
                          (void          * ) 0,							
                          (OS_STK        * )&App_Task_Key_Stk[APP_TASK_KEY_STK_SIZE - 1],		
                          (INT8U           ) APP_TASK_KEY_PRIO  );

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_KEY_PRIO, "TASK Key", &os_err);
	#endif


	os_err = OSTaskCreate((void (*)(void *)) uctsk_Blink,				
                          (void          * ) 0,							
                          (OS_STK        * )&App_Task_Blink_Stk[APP_TASK_BLINK_STK_SIZE - 1],		
                          (INT8U           ) APP_TASK_BLINK_PRIO  );							

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_BLINK_PRIO, "Task LED Blink", &os_err);
	#endif

	os_err = OSTaskCreate((void (*)(void *)) uctsk_DisplayDso,				
                          (void          * ) 0,							
                          (OS_STK        * )&App_Task_DisplayDso_Stk[APP_TASK_DISPLAYDSO_STK_SIZE - 1],		
                          (INT8U           ) APP_TASK_DISPLAYDSO_PRIO  );							

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_DISPLAYDSO_PRIO, "Task DisplayDso", &os_err);
	#endif



	os_err = OSTaskCreate((void (*)(void *)) uctsk_LCDRefresh,				
                          (void          * ) 0,							
                          (OS_STK        * )&App_Task_LCDRefresh_Stk[APP_TASK_LCDREFRESH_STK_SIZE - 1],		
                          (INT8U           ) APP_TASK_LCDREFRESH_PRIO  );							

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_LCDREFRESH_PRIO, "Task LCDRefresh", &os_err);
	#endif

	os_err = OSTaskCreate((void (*)(void *)) uctsk_UCGUI_DSO_Help,				
                          (void          * ) 0,							
                          (OS_STK        * )&App_Task_DSO_HELP_Stk[APP_TASK_DSO_HELP_STK_SIZE - 1],		
                          (INT8U           ) APP_TASK_DSO_HELP_PRIO  );							

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_DSO_HELP_PRIO, "Task DSO Help", &os_err);
	#endif

    DSOHelp = OSSemCreate(0);
    DSORun = OSSemCreate(0);
	LCDRefresh = OSSemCreate(0); 
}	

static void uctsk_LCDRefresh (void) 
{ 
  INT8U   err;
  
  GUI_Init();
  for(;;)
  {  
    OSSemPend(LCDRefresh,0,&err);	/* 获取信号量 */
	if (g_DSO.HoldEn == 1)
	{
	   Display_DSO();
	}
  }
}						

static void uctsk_UCGUI_DSO_Help (void) 
{ 
  INT8U   err;
  
  for(;;)
  {  
    OSSemPend(DSOHelp,0,&err);	/* 获取信号量 */
	DSO_Help();	                /* 显示帮助 */
  }
}						

static void uctsk_DSO_Run (void) 
{   
  for(;;)
  {  
    DSO_Run();	 /* 全速采集，实时显示 */
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
	  OSTimeDlyHMSM(0, 0, 0, 500);	  /* 500 MS  */ 
	  /*====LED-OFF=======*/ 
	  GPIO_ResetBits(GPIOF , GPIO_Pin_6);
	  GPIO_ResetBits(GPIOF , GPIO_Pin_7);
	  GPIO_ResetBits(GPIOF , GPIO_Pin_8);
	  GPIO_ResetBits(GPIOF , GPIO_Pin_9);
      OSTimeDlyHMSM(0, 0, 0, 500);	  /* 500 MS  */	 
    }
}

static void uctsk_Key (void) 
{  
  Key_Initializtion();  
   	for(;;)
   	{  
	   KeyPro();		
	   OSTimeDlyHMSM(0, 0, 0, 10);	 /* 10 MS  */
    }
}

static void uctsk_DisplayDso (void) 
{ 

  OSTimeDlyHMSM(0, 0, 0, 100);	

  for(;;)
  {  
    /* 运行状态 刷新波形 */
	if ( g_DSO.HoldEn == 0 )
	{
		Stop_ADC();	    /* 暂停采样 */
		Display_DSO();
		Start_ADC();	/* 开始采样 */
	}
	OSTimeDlyHMSM(0, 0, 0, 50);	 /* 50 MS  */
  }
}

/*******************************************************************************
* Function Name  : LED_GPIO_Configuration
* Description    : Configure GPIO Pin
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LED_GPIO_Configuration(void)
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
