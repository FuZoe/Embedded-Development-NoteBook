/*----------------------------------------------------------------------------
 *      
 *----------------------------------------------------------------------------
 *      Name:    app.C
 *      Purpose: 
 *----------------------------------------------------------------------------
 *      
 *      Copyright (c) 2004-2009          All rights reserved.
 *---------------------------------------------------------------------------*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/	
#include <includes.h>


/*
*********************************************************************************************************
*                                      extern task create FUNCTION
*********************************************************************************************************
*/

#if (OS_VIEW_MODULE == DEF_ENABLED)
extern void  App_OSViewTaskCreate (void);
#endif

extern void App_BlinkTaskCreate(void);
extern void App_MicrochipGUITaskCreate(void);



/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static  OS_STK         App_TaskStartStk[APP_TASK_START_STK_SIZE];

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  App_TaskCreate		(void);
static  void  App_EventCreate		(void);
static  void  App_TaskStart			(void		*p_arg);  

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

INT32S main (void)
{
    CPU_INT08U  os_err;
	os_err = os_err; /* prevent warning... */

	/* Note:  由于使用UCOS, 在OS运行之前运行,注意别使能任何中断. */
	CPU_IntDis();                    /* Disable all ints until we are ready to accept them.  */

    OSInit();                        /* Initialize "uC/OS-II, The Real-Time Kernel".         */
	

	os_err = OSTaskCreateExt((void (*)(void *)) App_TaskStart,  /* Create the start task.                               */
                             (void          * ) 0,
                             (OS_STK        * )&App_TaskStartStk[APP_TASK_START_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_START_PRIO,
                             (INT16U          ) APP_TASK_START_PRIO,
                             (OS_STK        * )&App_TaskStartStk[0],
                             (INT32U          ) APP_TASK_START_STK_SIZE,
                             (void          * )0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));

	

#if OS_TASK_NAME_EN > 0
    OSTaskNameSet(APP_TASK_START_PRIO, (CPU_INT08U *)"Start Task", &os_err);
#endif


	OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II).  */

	return (0);
}


/*
*********************************************************************************************************
*                                          App_TaskStart()
*
* Description : The startup task.  The uC/OS-II ticker should only be initialize once multitasking starts.
*
* Argument(s) : p_arg       Argument passed to 'App_TaskStart()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************
*/	  
static  void  App_TaskStart (void *p_arg)
{
	(void)p_arg;
                                                /* Initialize BSP functions.                            */
	
	/***************  Init hardware ***************/
	
	HAL_ChipInternalInit();

	GOLInit();
	
	Touchpad_Init(); 
	Touchpad_Calibration();

	/***************************************************/

    OS_CPU_SysTickInit();                                       /* Initialize the SysTick.                              */

#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                               /* Determine CPU capacity.                              */
#endif

    App_EventCreate();                                       /* Create application events.                           */
    App_TaskCreate();                                        /* Create application tasks.                            */

   	
	
	for(;;)
   	{
      	OSTimeDlyHMSM(0, 1, 0, 0);
    }
	
}






/*
*********************************************************************************************************
*                                             App_EventCreate()
*
* Description : Create the application events.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : App_TaskStart().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  App_EventCreate (void)
{

}


/*
*********************************************************************************************************
*                                            App_TaskCreate()
*
* Description : Create the application tasks.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : App_TaskStart().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  App_TaskCreate (void)
{
#if (OS_VIEW_MODULE == DEF_ENABLED)
	App_OSViewTaskCreate();
#endif	
	
	App_BlinkTaskCreate();
	
	App_MicrochipGUITaskCreate();

}


