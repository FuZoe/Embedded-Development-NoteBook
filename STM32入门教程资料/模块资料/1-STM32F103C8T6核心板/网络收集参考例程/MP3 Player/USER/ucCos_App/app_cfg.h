/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               app_cfg.h
** Descriptions:            ucosii configuration
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

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__
					  

/*
*********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
*********************************************************************************************************
*/
#define  OS_VIEW_MODULE                  DEF_DISABLED	     	/* DEF_ENABLED = Present, DEF_DISABLED = Not Present        */


/*
*********************************************************************************************************
*                                              TASKS NAMES
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/ 
#define  APP_TASK_START_PRIO                               2 
#define  APP_TASK_MP3DECODE_PRIO                           3
#define  APP_TASK_TOUCHSCREEN_PRIO                         4
#define  APP_TASK_MICROCHIP_GUI_PRIO					   5
#define  APP_TASK_READBUTTON_PRIO                          6
#define  APP_TASK_VS1003_PRIO							   7

#define  APP_TASK_OSVIEW_TERMINAL_PRIO	 (OS_LOWEST_PRIO - 6)
#define  OS_VIEW_TASK_PRIO               (OS_LOWEST_PRIO - 3)
#define  OS_TASK_TMR_PRIO                (OS_LOWEST_PRIO - 2)




/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  APP_TASK_START_STK_SIZE                         128u
#define  APP_TASK_MP3DECODE_STK_SIZE                   	 384u
#define  APP_TASK_MICROCHIP_GUI_STK_SIZE                 384u
#define  APP_TASK_TOUCHSCREEN_STK_SIZE					 256u
#define  APP_TASK_VS1003_STK_SIZE                        128u
#define  APP_TASK_READBUTTON_STK_SIZE                    128u

#define  APP_TASK_OSVIEW_TERMINAL_STK_SIZE   			 128u
#define  OS_VIEW_TASK_STK_SIZE                           128u




/*
*********************************************************************************************************
*                                                  LIB
*********************************************************************************************************
*/

#define  uC_CFG_OPTIMIZE_ASM_EN                 DEF_ENABLED
#define  LIB_STR_CFG_FP_EN                      DEF_DISABLED 

#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

