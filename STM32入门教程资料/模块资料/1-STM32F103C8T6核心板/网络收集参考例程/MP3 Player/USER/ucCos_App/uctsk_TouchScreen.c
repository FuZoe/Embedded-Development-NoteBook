/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               uctsk_TouchScreen.c
** Descriptions:            The uctsk_TouchScreen application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-26
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


/* Private variables ---------------------------------------------------------*/
extern  GOL_MSG msg;               /* GOL message structure to interact with GOL */
extern  OS_FLAG_GRP   *Sem_F;	   /* 事件标志 */
static  OS_STK        App_TaskTouchScreenStk[APP_TASK_TOUCHSCREEN_STK_SIZE];

/* Private function prototypes -----------------------------------------------*/
static void uctsk_TouchScreen (void);


/*******************************************************************************
* Function Name  : App_TouchScreenTaskCreate
* Description    : 创建触摸屏任务
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void  App_TouchScreenTaskCreate (void)
{
    CPU_INT08U  os_err;

	os_err = os_err; /* prevent warning... */

	os_err = OSTaskCreate((void (*)(void *)) uctsk_TouchScreen,				
                          (void          * ) 0,							
                          (OS_STK        * )&App_TaskTouchScreenStk[APP_TASK_TOUCHSCREEN_STK_SIZE - 1],		
                          (INT8U           ) APP_TASK_TOUCHSCREEN_PRIO  );							

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_TOUCHSCREEN_PRIO, "Task TouchScreen", &os_err);
	#endif
}


static void uctsk_TouchScreen (void) {                 
  
   INT8U   err;

   TouchInit();

   OSFlagPend( Sem_F,(OS_FLAGS)2,OS_FLAG_WAIT_SET_ALL,0,&err );  /* 等待TargetIni初始化完成 */

   TouchCalibration();

   OSFlagPost(Sem_F, 1, OS_FLAG_SET, &err);	  /* 触摸屏校准完成 */

   	for(;;)
   	{   
	   OSTimeDlyHMSM(0, 0, 0, 15);	/* 15 MS */
	   TouchGetMsg(&msg);   		/* Get message from touch screen */
    }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
