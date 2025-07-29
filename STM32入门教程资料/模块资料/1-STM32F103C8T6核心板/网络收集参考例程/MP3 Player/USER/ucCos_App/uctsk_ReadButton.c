/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               uctsk_ReadButton.c
** Descriptions:            The uctsk_ReadButton application function
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
static  OS_STK         App_TaskReadButtonStk[APP_TASK_READBUTTON_STK_SIZE];
extern  OS_FLAG_GRP    *Sem_F;	   /* 事件标志 */
/* 播放MP3邮箱 */
extern OS_EVENT *mp3Mbox;		
/* OS计数信号量 */	
extern OS_EVENT *StopMP3Decode;
/* MP3播放状态 */
extern AUDIO_Playback_status_enum AUDIO_Playback_status ;

/* Private function prototypes -----------------------------------------------*/
static void uctsk_ReadButton   (void);
static void GPIO_Configuration (void);
static void GUI_ReadButton     (void);


/*******************************************************************************
* Function Name  : App_ReadButtonTaskCreate
* Description    : 创建读开发板按键任务
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void  App_ReadButtonTaskCreate (void)
{
    CPU_INT08U  os_err;

	os_err = os_err; /* prevent warning... */

	os_err = OSTaskCreate((void (*)(void *)) uctsk_ReadButton,				
                          (void          * ) 0,							
                          (OS_STK        * )&App_TaskReadButtonStk[APP_TASK_READBUTTON_STK_SIZE - 1],		
                          (INT8U           ) APP_TASK_READBUTTON_PRIO  );							

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_READBUTTON_PRIO, "Task ReadButton", &os_err);
	#endif

}

static void uctsk_ReadButton (void) {  
  INT8U   err;               

  GPIO_Configuration();
  OSFlagPend( Sem_F,(OS_FLAGS) 1,OS_FLAG_WAIT_SET_ALL,0,&err );  /* 等待触摸屏校准完成 */
   for(;;)
   {   
	  GUI_ReadButton();

      OSTimeDlyHMSM(0, 0, 0, 100);
   }
}

/*******************************************************************************
* Function Name  : GUI_ReadButton
* Description    : synchronization GUI
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void GUI_ReadButton (void)
{  
   LISTBOX  *pLb;
   SLIDER   *pSld;

   pLb =(LISTBOX*)GOLFindObject(ID_LISTBOX1);

   if( !GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_14) )	   /* JOY_RIGHT is press */
   {
	  OSTimeDlyHMSM(0, 0, 0, 100);                      /* 按键防抖动        */
	  if( !GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_14) )  /* 列表框箭头下      */
	  {
	      pSld =(SLIDER*)GOLFindObject(ID_SLIDER3);	

		  LbSetFocusedItem(pLb,LbGetFocusedItem(pLb)+1);
          SetState(pLb, LB_DRAW_ITEMS);

          SldSetPos(pSld,SldGetPos(pSld)-1);
          SetState(pSld, SLD_DRAW_THUMB);
	  }
   }

   if( !GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_13) )	    /* JOY_LEFT is press */
   {
	  OSTimeDlyHMSM(0, 0, 0, 100);                      /* 按键防抖动         */
	  if( !GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_13) )  /* 列表框箭头上      */
	  {
		     pSld =(SLIDER*)GOLFindObject(ID_SLIDER3); 

	         LbSetFocusedItem(pLb,LbGetFocusedItem(pLb)-1);
    	     SetState(pLb, LB_DRAW_ITEMS);

             SldSetPos(pSld,SldGetPos(pSld)+1);
             SetState(pSld, SLD_DRAW_THUMB);
	  }
   }

   if( !GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_8) )	   /* JOY_DOWN is press    */
   {
	  OSTimeDlyHMSM(0, 0, 0, 100);                     /* 按键防抖动         */
	  if( !GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_8) )  /* 快退               */
	  {
	      if (LbGetSel(pLb,NULL) != NULL && !( outOfData == 1 ))	 /* 列表框没有选项 */
		  {
	         pSld = (SLIDER*)GOLFindObject(ID_SLIDER2);

		     SldSetPos(pSld,SldGetPos(pSld)-2);

	         /* 跳转READBUF_SIZE个整数倍*/
		     MP3_Data_Index = ( (uint32_t)( ( (float)SldGetPos(pSld)/200 ) * ( mp3FileObject.fsize - mp3_info.data_start) ) - \
			                  ( (uint32_t)( ( (float)SldGetPos(pSld)/200 ) * ( mp3FileObject.fsize - mp3_info.data_start) ) % READBUF_SIZE ) );

		     res = f_lseek(&mp3FileObject, mp3_info.data_start + MP3_Data_Index );		
		     SetState(pSld,SLD_DRAW_THUMB); 
		  }
	  }
   }

   if( !GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_11) )	    /* JOY_UP is press  */
   {
	  OSTimeDlyHMSM(0, 0, 0, 100);                      /* 按键防抖动         */
	  if( !GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_11) )  /* 快进               */
	  {
	      if (LbGetSel(pLb,NULL) != NULL && !( outOfData == 1 ))	 /* 列表框没有选项 */
	      {
	         pSld = (SLIDER*)GOLFindObject(ID_SLIDER2);

		     SldSetPos(pSld,SldGetPos(pSld)+2);

	         /* 跳转READBUF_SIZE个整数倍*/
		     MP3_Data_Index = ( (uint32_t)( ( (float)SldGetPos(pSld)/200 ) * ( mp3FileObject.fsize - mp3_info.data_start) ) - \
			                  ( (uint32_t)( ( (float)SldGetPos(pSld)/200 ) * ( mp3FileObject.fsize - mp3_info.data_start) ) % READBUF_SIZE ) );

		     res = f_lseek(&mp3FileObject, mp3_info.data_start + MP3_Data_Index );		
		     SetState(pSld,SLD_DRAW_THUMB); 
		  }
	  }
   }

   if( !GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_7) )	   /* JOY_SEL is press   */
   {
	  OSTimeDlyHMSM(0, 0, 0, 100);                     /* 按键防抖动         */
	  if( !GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_7) )  /* 列表框focus确定    */
	  {

	  }
   }
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;  

  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG, ENABLE); 						 
  /**
  *  JOY_UP -> PG11 , JOY_DOWN -> PG8 , JOY_LEFT -> PG13 , JOY_RIGHT -> PG14 , JOY_SEL -> PG7
  *  Tamper Button -> PC13 , Wakeup Button -> PA0 , User Button -> PA8
  */				
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  GPIO_Init(GPIOG, &GPIO_InitStructure);
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
