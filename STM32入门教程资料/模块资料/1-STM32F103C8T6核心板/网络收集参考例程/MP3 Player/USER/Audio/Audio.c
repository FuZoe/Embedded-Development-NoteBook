/****************************************Copyright (c)**************************************************                         
**
**                                 http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			Audio.C
** Descriptions:		None
**
**------------------------------------------------------------------------------------------------------
** Created by:			AVRman
** Created date:		2010-11-18
** Version:				1.0
** Descriptions:		The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:			
** Modified date:	
** Version:
** Descriptions:		
********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <includes.h>

/* Private variables ---------------------------------------------------------*/
AUDIO_Playback_status_enum AUDIO_Playback_status ;
/* file system */
FATFS    fs;                      /* Work area (file system object) for logical drive */
FIL      mp3FileObject;           /* file objects */
FRESULT  res;                     /* FatFs function common result code	*/
UINT     n_Read;                  /* File R/W count */
uint8_t  readBuf[READBUF_SIZE];   /* Read buffer where data from SD card is read to */
/* MP3已播放字节 */
uint32_t MP3_Data_Index;

uint32_t outOfData = (uint32_t)-1;  

/* OS计数信号量 */	
extern OS_EVENT *mp3Data;				   /* MP3数据邮箱 */
extern OS_EVENT *StopMP3Decode;
extern OS_EVENT *MP3DecodeComplete;				   

/*******************************************************************************
* Function Name  : AUDIO_Playback_Stop
* Description    : Stop the playback by stopping the DMA transfer
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void AUDIO_Playback_Stop(void)
{
   AUDIO_Playback_status = NO_SOUND;       
   MP3_Data_Index = 0;
}

/*******************************************************************************
* Function Name  : PlayAudioFile
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void PlayAudioFile(FIL *FileObject,char *path)
{
   INT8U   err;

   outOfData = 0;

   res = f_open(FileObject , path , FA_OPEN_EXISTING | FA_READ);		
   
   memset(&mp3_info,0,sizeof(mp3_info));
      			
   Read_ID3V2(FileObject,&mp3_info);
   
   Read_ID3V1(FileObject,&mp3_info);
    
   MP3_Data_Index = 0;

   mp3_info.data_start = 0;

   /* 播放时间 */
   if( (strstr(path,"MP3") !=NULL) || (strstr(path,"mp3") !=NULL) )
   {																					   
	   if( (strstr(path,"MP3") !=NULL) )
	   {
	      mp3_info.duration = ( ( path[ strstr(strstr(path,"MP3") + 5,":") - path - 2 ] - '0' ) *10  + \
	                            ( path[ strstr(strstr(path,"MP3") + 5,":") - path - 1 ] - '0' )  ) * 60; /* 分钟 */

	      mp3_info.duration = mp3_info.duration + ( path[ strstr(strstr(path,"MP3") + 5,":") - path + 1 ] - '0' ) *10 + \
	                                              ( path[ strstr(strstr(path,"MP3") + 5,":") - path + 2 ] - '0' );	 /* 秒   */
	   }
	   else if( (strstr(path,"mp3") !=NULL) )
	   {
	      mp3_info.duration = ( ( path[ strstr(strstr(path,"mp3") + 5,":") - path - 2 ] - '0' ) *10  + \
	                            ( path[ strstr(strstr(path,"mp3") + 5,":") - path - 1 ] - '0' )  ) * 60; /* 分钟 */

	      mp3_info.duration = mp3_info.duration + ( path[ strstr(strstr(path,"mp3") + 5,":") - path + 1 ] - '0' ) *10 + \
	                                              ( path[ strstr(strstr(path,"mp3") + 5,":") - path + 2 ] - '0' );	 /* 秒   */
	   }	   
   }

   if ( strlen(mp3_info.title) ==0 )
   {
      strcpy( mp3_info.title, "找不到歌曲名");  
   }

   if ( strlen(mp3_info.title) > 20 )
   {
      strcpy( mp3_info.title, "歌曲名太长、无法显示");  
   }

   mp3_info.position = -1;

   res = f_lseek(FileObject,mp3_info.data_start); 

   /* Start the playback */
   AUDIO_Playback_status = IS_PLAYING;   

   n_Read = ( UINT ) -1 ;

   while( ( res == FR_OK && n_Read != 0 ) && !OSSemAccept(StopMP3Decode) )	
   {   
       res = f_read( FileObject, readBuf, 512, &n_Read );
       MP3_Data_Index += n_Read;             /* 记录已播放大小 */
	   
	   OSMboxPost(mp3Data,(void*)readBuf );  /* 传送数据解码 */	 

	   OSSemPend(MP3DecodeComplete,0,&err);	 /* 获取解码完成信号量 */
   } 

   if(res == FR_OK && n_Read == 0 )
   {
      outOfData = 1;  
      AUDIO_Playback_Stop();
   } 	 
   f_close(FileObject);
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

