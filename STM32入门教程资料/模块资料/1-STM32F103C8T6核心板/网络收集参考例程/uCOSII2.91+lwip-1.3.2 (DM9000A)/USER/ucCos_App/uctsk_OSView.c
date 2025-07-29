/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               uctsk_OSView.c
** Descriptions:            The uctsk_OSView application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-9
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


#if (OS_VIEW_MODULE == DEF_ENABLED)


/* Private variables ---------------------------------------------------------*/
static  OS_STK  OSViewTerminalTaskStk[APP_TASK_OSVIEW_TERMINAL_STK_SIZE];
static  OS_EVENT   *eventAppOSViewTerminalRxMbox;

/* Private function prototypes -----------------------------------------------*/
static  void    uctsk_OSViewTerminal(void *pdata);
static  void    AppOSViewTerminalRx(CPU_INT08U   rx_data);



void  App_OSViewTaskCreate (void)
{
	CPU_INT08U  os_err;

	os_err = os_err; /* prevent warning... */
    	
	OSView_Init(OS_VIEW_BAUDRATE);                           /* OSView Init, baud rate = OS_VIEW_BAUDRATE                      */
    OSView_TerminalRxSetCallback(AppOSViewTerminalRx);
    OSView_RxIntEn();                                        /* Enable Rx Interrupts                                 */
	eventAppOSViewTerminalRxMbox = OSMboxCreate((void *)0);
  
    os_err = OSTaskCreateExt((void (*)(void *)) uctsk_OSViewTerminal,
                             (void          * ) 0,
                             (OS_STK        * )&OSViewTerminalTaskStk[APP_TASK_OSVIEW_TERMINAL_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_OSVIEW_TERMINAL_PRIO,
                             (INT16U          ) APP_TASK_OSVIEW_TERMINAL_PRIO,
                             (OS_STK        * )&OSViewTerminalTaskStk[0],
                             (INT32U          ) APP_TASK_OSVIEW_TERMINAL_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_OSVIEW_TERMINAL_PRIO, "Task OSView Terminal", &os_err);
	#endif

}

/*
*********************************************************************************************************
*                                          AppTerminalRx()
*
* Description : Callback function for uC/OS-View
*
* Argument(s) : rx_data     The received data.
*
* Return(s)   : none.
*********************************************************************************************************
*/

static void  AppOSViewTerminalRx (CPU_INT08U rx_data)
{

	OSMboxPost(eventAppOSViewTerminalRxMbox, &rx_data);
}


static void  uctsk_OSViewTerminal(void *pdata)
{
    INT8U      s[100];
    INT8U      *key;
    INT8U      err;

    (void)pdata;	/* Prevent compiler warning                 */
    OSView_TxStr("\n?: Help (This menu)\n", 0);
    while (1) {
        key = (INT8U *)OSMboxPend(eventAppOSViewTerminalRxMbox, 0, &err);
        switch (*key) {
            case '1':
                 sprintf((char *)s, "\nCPU Usage = %3u%%\n", OSCPUUsage);
                 OSView_TxStr(s, 1);
                 break;

            case '2':
                 sprintf((char *)s, "\n#Tasks    = %3u\n", OSTaskCtr);
                 OSView_TxStr(s, 1);
                 break;

            default:
                 OSView_TxStr("\n\nMicrium, Inc.",       1);
                 OSView_TxStr("\n1: CPU Usage (%)",      1);
                 OSView_TxStr("\n2: #Tasks",             1);
                 OSView_TxStr("\n?: Help (This menu)\n", 1);

                 break;
        }
    }
}


#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
