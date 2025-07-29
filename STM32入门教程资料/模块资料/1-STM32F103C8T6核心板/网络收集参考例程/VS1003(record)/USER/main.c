/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The VS1003(record) application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2011-2-27
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
#include "stm32f10x.h"
#include "vs1003.h"
#include "ff.h"

/* Private variables ---------------------------------------------------------*/

/* first part of RIFF Header, insert 444 zeroes after this */  
const uint8_t RIFFHeader0[] = 
{ 
'R' , 'I' , 'F' , 'F' , /* Chunk ID (RIFF) */
0x70, 0x70, 0x70, 0x70, /* Chunk payload size (calculate after rec!) */ 
'W' , 'A' , 'V' , 'E' , /* RIFF resource format type */ 
'f' , 'm' , 't' , ' ' , /* Chunk ID (fmt ) */ 
0x14, 0x00, 0x00, 0x00, /* Chunk payload size (0x14 = 20 bytes) */ 
0x11, 0x00,             /* Format Tag (IMA ADPCM) */ 
0x01, 0x00,             /* Channels (1) */ 
0x40, 0x1f, 0x00, 0x00, /* Sample Rate, 0x1f40 = 8.0kHz */ 
0xd7, 0x0f, 0x00, 0x00, /* Average Bytes Per Second */
0x00, 0x01,             /* Data Block Size (256 bytes) */  
0x04, 0x00,             /* ADPCM encoded bits per sample (4 bits) */ 
0x02, 0x00,             /* Extra data (2 bytes) */ 
0xf9, 0x01,             /* Samples per Block (505 samples) */ 
'f' , 'a' , 'c' , 't' , /* Chunk ID (fact) */
0xc8, 0x01, 0x00, 0x00, /* Chunk payload size (456 bytes (zeropad!)) */ 
0xff, 0xff, 0xff, 0xff  /* Number of Samples (calculate after rec!) */
};                      /* Insert 448 zeroes here! */ 

/* add 452 zeros after */
const uint8_t RIFFHeader504[] = 
{
'd' , 'a' , 't' , 'a' , /* Chunk ID (data) */
0x70, 0x70, 0x70, 0x70  /* Chunk payload size (calculate after rec!) */ 
}; 

FATFS fs;         /* Work area (file system object) for logical drive */
FIL fsrc;         /* file objects */   
FRESULT res;
UINT br;

uint8_t wavheader[512];
uint8_t buffer[512];   /*data buffer for saving to disk */ 
uint8_t blockNumber;
uint32_t SectorCount;    
uint32_t SEC_CAL; 
uint8_t recording;
uint8_t* dataBufPtr;  

void GPIO_Configuration(void);
void NVIC_Configuration(void);
void EXTI_Configuration(void);

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
  VS1003_Init();
  VS1003_Reset();
  VS1003_SoftReset();
  GPIO_Configuration();
  NVIC_Configuration();
  EXTI_Configuration();

  recording = 0;

  while( recording != 1 );  /* 开始录音 */
  VS1003_Record_Init();

  f_mount(0, &fs); 
  f_open(&fsrc, "voice.wav", FA_CREATE_ALWAYS | FA_WRITE); 

  for( br = 0; br < 56; br++ ) 
  {  
     wavheader[br] = RIFFHeader0[br]; 
  } 
  for( br = 52; br < 504; br++ ) 
  { 
     wavheader[br] = 0; 
  } 
  for( br = 504; br < 512; br++ ) 
  { 
     wavheader[br] = RIFFHeader504[br-504]; 
  } 

  f_write( &fsrc, wavheader, 512 , &br );

  /* Wait until buffer level restarts from 0 */
  while( VS1003_ReadReg(SPI_HDAT1) >> 8 ); /* lots of data in buffer */ 
 
  SectorCount = 0;
  blockNumber = 0;
  dataBufPtr = buffer;

  while( recording != 2 )  /* 停止录音 */
  {	       
    if( VS1003_ReadReg(SPI_HDAT1) >= 128 )  
    {  
        for( br = 0; br < 128; br++ )  
        {   
           SEC_CAL = VS1003_ReadReg(SPI_HDAT0);          
           *dataBufPtr++  = (uint8_t)( SEC_CAL >> 8 ); 
           *dataBufPtr++  = (uint8_t)( SEC_CAL & 0xff ); 
        }
		blockNumber++;    
    }
    if( blockNumber == 2 ) 
    {  
       blockNumber=0;  
       f_write( &fsrc, buffer, sizeof(buffer) , &br );  
       SectorCount++;  
	   dataBufPtr = buffer;
	}
  }  
 
  SEC_CAL = ( SectorCount - 1 ) * 1010;  
  wavheader[48] = (  SEC_CAL & 0xff );  
  wavheader[49] = ( (SEC_CAL >> 8) & 0xff );  
  wavheader[50] = ( (SEC_CAL >> 16) & 0xff );  
  wavheader[51] = ( (SEC_CAL >> 24) & 0xff );  

  /* 总长度,RIFF块后(整个wav文件大小-8) */ 
  SEC_CAL = ( SectorCount * 512 ) - 8;  
  wavheader[4] = (  SEC_CAL & 0xff );  
  wavheader[5] = ( (SEC_CAL >> 8) & 0xff );  
  wavheader[6] = ( (SEC_CAL >> 16) & 0xff );  
  wavheader[7] = ( (SEC_CAL >> 24) & 0xff );  

  /* 真实录音数据大小 Data块 */ 
  SEC_CAL = ( SectorCount - 1 ) * 512; 
  wavheader[508] = (  SEC_CAL & 0xff );  
  wavheader[509] = ( (SEC_CAL >> 8) & 0xff );  
  wavheader[510] = ( (SEC_CAL >> 16) & 0xff );  
  wavheader[511] = ( (SEC_CAL >> 24) & 0xff );  

  f_lseek(&fsrc,0);  
  f_write(&fsrc, wavheader, 512 , &br );  
  f_close(&fsrc);  

  VS1003_Reset();
  VS1003_SoftReset();  

  while( recording != 3 );	/* 开始播放 */
  
  f_open(&fsrc, "voice.wav", FA_OPEN_EXISTING | FA_READ ); 
  
  while(1)
  {	 	 
     if( fsrc.fptr == fsrc.fsize )
	 {
        f_lseek(&fsrc,0); 
	 } 
     res = f_read(&fsrc, buffer, sizeof(buffer), &br ); 

     for( br = 0; br < 16; br++ )
     {	 
        while(  MP3_DREQ == 0 );     /* 等待空闲 */
        VS1003_WriteData( buffer + br * 32 );   
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
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC , ENABLE); 						 
  /**
  *  Tamper Button -> PC13 , Wakeup Button -> PA0 , User Button -> PA8
  */				
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_8 ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : EXTI_Configuration
* Description    : Configures the different EXTI lines.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void EXTI_Configuration(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13);

  EXTI_ClearITPendingBit(EXTI_Line0);
  EXTI_ClearITPendingBit(EXTI_Line8);
  EXTI_ClearITPendingBit(EXTI_Line13);

  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line13;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_Line = EXTI_Line8;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures the nested vectored interrupt controller.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure; 

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);  													
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;   
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	      
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);	 

  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;	 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 

  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;	 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 
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

