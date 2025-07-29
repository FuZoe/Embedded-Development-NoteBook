/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               uctsk_MicrochipGUI.c
** Descriptions:            The MicrochipGUI application function
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
#include <menu.h>

/* Private variables ---------------------------------------------------------*/
GOL_MSG msg;       /* GOL message structure to interact with GOL */
static  OS_STK  AppMicrochipGUITaskStk[APP_TASK_MICROCHIP_GUI_STK_SIZE];
extern  OS_FLAG_GRP    *Sem_F;	   /* 事件标志 */

/* Private function prototypes -----------------------------------------------*/
static  void    uctsk_MicrochipGUI(void *pdata);


/*******************************************************************************
* Function Name  : App_MicrochipGUITaskCreate
* Description    : 创建GUI任务
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void  App_MicrochipGUITaskCreate (void)
{
	CPU_INT08U  os_err;

	os_err = os_err; /* prevent warning... */
    
    os_err = OSTaskCreateExt((void (*)(void *)) uctsk_MicrochipGUI,
                             (void          * ) 0,
                             (OS_STK        * )&AppMicrochipGUITaskStk[APP_TASK_MICROCHIP_GUI_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_MICROCHIP_GUI_PRIO,
                             (INT16U          ) APP_TASK_MICROCHIP_GUI_PRIO,
                             (OS_STK        * )&AppMicrochipGUITaskStk[0],
                             (INT32U          ) APP_TASK_MICROCHIP_GUI_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_MICROCHIP_GUI_PRIO, "Task MicrochipGUI", &os_err);
	#endif

}

static  void  uctsk_MicrochipGUI(void *pdata)
{
	INT8U   err;

	(void)pdata;	                    /* Prevent compiler warning */

	GOLInit(); 	
	
	OSFlagPend( Sem_F,(OS_FLAGS) 1,OS_FLAG_WAIT_SET_ALL,0,&err );  /* 等待触摸屏校准完成 */	  
	
	StartMenu();

	for(;;)
   	{	
		if ( GOLDraw() )				/* Draw GOL object */
      	{				               
         	GOLMsg(&msg);        		/* Process message */
		}  
		OSTimeDlyHMSM(0, 0, 0, 50);
    }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

