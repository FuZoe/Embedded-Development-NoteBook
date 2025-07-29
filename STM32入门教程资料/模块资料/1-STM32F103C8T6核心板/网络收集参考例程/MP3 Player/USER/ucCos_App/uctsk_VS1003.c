/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               uctsk_VS1003.c
** Descriptions:            The VS1003 Task application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2011-6-10
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
static  OS_STK    App_TaskVS1003Stk[APP_TASK_VS1003_STK_SIZE];

OS_EVENT *mp3Data;				   /* MP3数据邮箱 */	  
OS_EVENT *MP3DecodeComplete;				   
OS_EVENT *MP3Continue; 
OS_EVENT *MP3Pause;

/* Private function prototypes -----------------------------------------------*/
static void uctsk_VS1003 (void);


/*******************************************************************************
* Function Name  : App_VS1003TaskCreate
* Description    : VS1003播放任务
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void  App_VS1003TaskCreate (void)
{
    CPU_INT08U  os_err;

	os_err = os_err; /* prevent warning... */

	os_err = OSTaskCreate((void (*)(void *)) uctsk_VS1003,				
                          (void          * ) 0,							
                          (OS_STK        * )&App_TaskVS1003Stk[APP_TASK_VS1003_STK_SIZE - 1],		
                          (INT8U           ) APP_TASK_VS1003_PRIO  );							

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_VS1003_PRIO, "Task uctsk_vs1003", &os_err);
	#endif

    MP3DecodeComplete = OSSemCreate(0);

	MP3Continue = OSSemCreate(0);  

	MP3Pause = OSSemCreate(0); 

    mp3Data = OSMboxCreate( (void *) 0);	/* 创建消息邮箱 */
}

static void uctsk_VS1003 (void) {                 
 
    INT8U   err,index;
    char    *ReadData;

    VS1003_Init();
    VS1003_Reset();
    VS1003_SoftReset();
    
   	for(;;)
   	{  
	   ReadData = (char*)OSMboxPend(mp3Data,0,&err);
       if( err == OS_ERR_NONE )
       {
     	   for( index = 0; index < 16; index ++ )
    	   {	 
	    	   while(  MP3_DREQ == 0 );     /* 等待空闲 */
	    	   VS1003_WriteData( (void*)( ReadData + index * 32 ) );   
 	       }        	     
	   }
       if( OSSemAccept(MP3Pause) )
	   {
	       OSSemPend(MP3Continue,0,&err);
	   }
	   OSSemPost(MP3DecodeComplete);	    /* 发送解码完成信号量 */ 
    }
}


/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
