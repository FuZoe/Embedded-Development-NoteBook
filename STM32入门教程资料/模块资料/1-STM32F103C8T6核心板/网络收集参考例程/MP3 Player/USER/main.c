/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The MP3 Player application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2011-6-5
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


#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


/* Private variables ---------------------------------------------------------*/
static  OS_STK         App_TaskStartStk[APP_TASK_START_STK_SIZE];
OS_FLAG_GRP            *Sem_F;	   /* 事件标志 */

/* Private function prototypes -----------------------------------------------*/
#if (OS_VIEW_MODULE == DEF_ENABLED)
extern  void  App_OSViewTaskCreate        (void);
#endif
static  void  TargetInit                  (void);
static  void  App_TaskCreate              (void);
static  void  App_TaskStart               (void *p_arg);
static  void  GPIO_Configuration          (void);
static  void  USART_Configuration         (void);
static  void  NVIC_Configuration          (void);
extern  void  App_MicrochipGUITaskCreate  (void);
extern  void  App_TouchScreenTaskCreate   (void);
extern  void  App_ReadButtonTaskCreate    (void);
extern  void  App_MP3DecodeTaskCreate     (void);
extern  void  App_VS1003TaskCreate        (void);

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

	OSStart();               /* Start multitasking (i.e. give control to uC/OS-II).  */

	return (0);
}

/*******************************************************************************
* Function Name  : App_TaskStart
* Description    : The startup task.The uC/OS-II ticker should only be initialize once multitasking starts.
* Input          : - p_arg: Argument passed to 'App_TaskStart()' by 'OSTaskCreate()'.
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static  void  App_TaskStart (void *p_arg)
{   
    INT8U   err;

	(void)p_arg;
			
    OS_CPU_SysTickInit();                                    /* Initialize the SysTick.                              */

#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                            /* Determine CPU capacity.                              */
#endif
	
	Sem_F =  OSFlagCreate( 0,&err );                         /* 建立一个事件标志组，0初始化                          */

    App_TaskCreate();                                        /* Create application tasks.                            */
	
	OSTimeDlyHMSM(0, 0, 1, 0);	                             /* 1 second                                             */ 

	TargetInit();

	for(;;)
   	{

      	OSTimeDlyHMSM(0, 1, 0, 0);							 /* Delay One minute */
    }	
}

/*******************************************************************************
* Function Name  : App_TaskCreate
* Description    : Create the application tasks.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static  void  App_TaskCreate (void)
{
#if (OS_VIEW_MODULE == DEF_ENABLED)
	App_OSViewTaskCreate();
#endif		

	App_MicrochipGUITaskCreate();

	App_TouchScreenTaskCreate();

	App_ReadButtonTaskCreate();

	App_MP3DecodeTaskCreate();
	
	App_VS1003TaskCreate();
}

/*******************************************************************************
* Function Name  : TargetInit
* Description    : 初始化系统和外设 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void TargetInit(void)
{
	uint8_t HZLib[2];	INT8U   err;
	
	SPI_FLASH_Init();
	SPI_FLASH_Test();
	GPIO_Configuration();
	NVIC_Configuration();
	USART_Configuration();  
	
	printf("-- Basic MP3 Project %s \r\n", SOFTPACK_VERSION);
	printf("-- %s --\r\n", BOARD_NAME); 
	printf("-- Compiled: %s %s \r\n", __DATE__, __TIME__); 
	
	/* 检测SD卡存在 */
	LCD_Clear(Black);
	
	if( SD_Detect() == SD_PRESENT )
	{
		;
	}
	else
	{
		printf("-- Please connect a SD card \r\n");
		GUI_Text(36,100,"Please insert SD card",White,Black);
		while(SD_Detect()!=SD_PRESENT);
		printf("-- SD card connection detected \r\n");
	}
	
	GUI_Text(44,120,"SD card detected OK",White,Black);
	OSTimeDlyHMSM(0, 0, 1, 0);	 /* 1 second  */ 
	
	SearchMusic(MUSIC_DIRECTORY);
	
	df_read_open(0);
	df_read(HZLib,2);  
	/* 写入字库 */
	if( !( HZLib[0] == 0xaa && HZLib[1] == 0x55 ) )
	{   
		LCD_Clear(Black); 
		GUI_Text(60,100,"No Detect HZLib",White,Black);
		GUI_Text(68,120,"Writing HZLib",White,Black);
		df_write_open( HZLIB_ADDR );   
		f_mount(0,&fs);   /* 挂载文件系统 */ 
		res = f_open(&mp3FileObject , SYSTEM_FILE_HZLIB , FA_OPEN_EXISTING | FA_READ);
		if( res != 0 )
		{
			GUI_Text(76,140,"HZLib Error",White,Black);	 
			while(1);
		}
		SSTF016B_Erase( HZLIB_ADDR/SEC_SIZE , mp3FileObject.fsize/SEC_SIZE + 1 );	
		do
		{
			res = f_read(&mp3FileObject,readBuf,READBUF_SIZE,&n_Read);
			df_write(readBuf,n_Read);  
		}	  
		while( n_Read == READBUF_SIZE && res == 0 );

		if( mp3FileObject.fptr != mp3FileObject.fsize )
		{
			GUI_Text(76,140,"HZLib Error",White,Black);	 
			while(1);
		}
		GUI_Text(88,140,"HZLib OK",White,Black);	 
		/* 字库存在标志 */
		HZLib[0] = 0xaa;  HZLib[1] = 0x55;
		df_write_open(0);   
		df_write(HZLib,2);
		OSTimeDlyHMSM(0, 0, 1, 0);	 /* 1 second  */ 
	}
	SPI_FLASH_SetSpeed(SPI_SPEED_LOW);
	OSFlagPost(Sem_F, 2, OS_FLAG_SET, &err);	  /* TargetIni初始化完成 */ 
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configure GPIO Pin
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( SD_DETECT_GPIO_CLK, ENABLE); 						 
  /**
  *	Configure SD_CD pin: SD Card detect pin
  */	
  GPIO_InitStructure.GPIO_Pin = SD_DETECT_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(SD_DETECT_GPIO_PORT, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configure USART1 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void USART_Configuration(void)
{ 
#if !(OS_VIEW_MODULE == DEF_ENABLED)

  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure; 

  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
  /*
  *  USART1_TX -> PA9 , USART1_RX ->	PA10
  */				
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);		   

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	        
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART1, &USART_InitStructure); 
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  USART_Cmd(USART1, ENABLE);

#endif
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures SDIO IRQ channel.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure the NVIC Preemption Priority Bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
#if !(OS_VIEW_MODULE == DEF_ENABLED)

  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}

#endif

  return ch;
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

