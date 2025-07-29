/****************************************Copyright (c)**************************************************                         
**
**                                 http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			Audio.h
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
#ifndef __AUDIO_H
#define __AUDIO_H

/* Includes ------------------------------------------------------------------*/
#include "ff.h"
#include "stm32f10x.h"
#include "MP3Header.h"
#include <string.h>
#include <stdio.h>

/* Private define ------------------------------------------------------------*/
#define READBUF_SIZE   ( 512 )           /*512 bytes */

/* Private variables ---------------------------------------------------------*/
/* file system*/
extern FATFS    fs;                      /* Work area (file system object) for logical drive */
extern FIL      mp3FileObject;           /* file objects */
extern FRESULT  res;                     /* FatFs function common result code	*/
extern UINT     n_Read;                  /* File R/W count */
extern uint8_t  readBuf[READBUF_SIZE];   /* Read buffer where data from SD card is read to */
extern uint32_t MP3_Data_Index;			 /* MP3已播放大小 */
extern uint32_t outOfData;  

/* Private typedef -----------------------------------------------------------*/
typedef enum { NO_SOUND, IS_PLAYING , }                           AUDIO_Playback_status_enum;

/* Private function prototypes -----------------------------------------------*/
void AUDIO_Playback_Stop(void);
void PlayAudioFile(FIL *FileObject,char *path);

#endif 
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
