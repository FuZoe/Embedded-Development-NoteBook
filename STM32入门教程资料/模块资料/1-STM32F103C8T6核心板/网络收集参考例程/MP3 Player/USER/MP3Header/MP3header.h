/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               MP3header.h
** Descriptions:            None
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-12-6
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

#ifndef __MP3Header_H__
#define __MP3Header_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Private typedef -----------------------------------------------------------*/
struct PlayFileList
{
    char filename[24];			 /* 短文件名8.3格式   */
    struct PlayFileList *next;	 /* 指向链表下一个MP3 */
	struct PlayFileList *back;	 /* 指向链表上一个MP3 */
};
typedef struct PlayFileList dnode;
typedef dnode *dlink;

typedef struct time
{
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint16_t msecond;
}TIME;

/* music tag information structure */
struct tag_info
{
	char title [40];        /* music title                     */
	char artist[40];        /* music artist                    */

	int duration;           /* music total duration (second)   */
	int position;           /* music current position (second) */

	uint8_t  length;        /* bits PerSample			       */
	uint32_t bit_rate;      /* bit rate                        */
	uint32_t sampling;      /* sampling                        */
	uint32_t data_start;    /* start position of data          */
};

/* Private define ------------------------------------------------------------*/
/* file type */
#define wavFile                    	0
#define mp3File                    	1 
#define unknownFile				   	2

#define MIN(x, y)			((x) < (y)? (x) : (y))

/* Private variables ---------------------------------------------------------*/
extern dlink PlayFile;
/* MP3文件信息 */
extern struct tag_info mp3_info;


/* Private function prototypes -----------------------------------------------*/
void SearchMusic(char* path);
void TimeFormat(uint32_t srctime,TIME *destime);
void Read_ID3V1(FIL *FileObject, struct tag_info *info);
void Read_ID3V2(FIL *FileObject, struct tag_info *info);
uint32_t GetMP3MaxTime(uint8_t *MP3buffer, uint16_t buffersize,uint32_t filelen);

#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

