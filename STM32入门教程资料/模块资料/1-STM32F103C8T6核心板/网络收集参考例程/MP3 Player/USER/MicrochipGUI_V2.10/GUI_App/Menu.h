/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               menu.h
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

#ifndef _MENU_H
#define _MENU_H


/* Private define ------------------------------------------------------------*/
#define     ID_BUTTON1     1    /* 上一曲         */
#define     ID_BUTTON2     2    /* 播放歌曲       */
#define     ID_BUTTON3     3    /* 停止播放		  */
#define     ID_BUTTON4     4	/* 下一曲         */ 
#define     ID_BUTTON5 	   5	/* 声音控制开关	  */
#define     ID_BUTTON6	   6	/* 列表框箭头上	  */
#define     ID_BUTTON7 	   7	/* 列表框箭头下	  */

#define     ID_SLIDER1     10   /* 声音进度条     */
#define     ID_SLIDER2	   11	/* 播放进度条     */
#define     ID_SLIDER3	   12	/* 下拉列表滑动条 */

#define     ID_LISTBOX1	   13	/* 列表框         */

#define     ID_PICTURE1    14	/* 播放器界面      */
#define     ID_PICTURE2    15	/* 增加音量        */
#define     ID_PICTURE3    16	/* 减少音量		   */
#define     ID_PICTURE4    17	/* 播放器界面之一  */
#define     ID_PICTURE5    18	/* 播放器界面之二  */
#define     ID_PICTURE6    19	/* 播放器界面之三  */
#define     ID_PICTURE7    20	/* 播放器界面之四  */

/* Private typedef -----------------------------------------------------------*/
typedef enum {
   DISPLAY_MP3 =0,
   START_MP3     ,
} SCREEN_STATES;

/* Private variables ---------------------------------------------------------*/
extern SCREEN_STATES  ScreenState;

extern FONT_EXTERNAL  externalfont;
extern volatile const BITMAP_FLASH previous_on;
extern volatile const BITMAP_FLASH previous_off;

extern volatile const BITMAP_FLASH play_on;
extern volatile const BITMAP_FLASH play_off;

extern volatile const BITMAP_FLASH stop_on;
extern volatile const BITMAP_FLASH stop_off;

extern volatile const BITMAP_FLASH next_on;
extern volatile const BITMAP_FLASH next_off ;

extern volatile const BITMAP_FLASH press_up;
extern volatile const BITMAP_FLASH press_down ;

extern volatile const BITMAP_FLASH rpress_up ;
extern volatile const BITMAP_FLASH rpress_dowm ;

extern volatile const BITMAP_FLASH musicon ;
extern volatile const BITMAP_FLASH musicoff ;

extern volatile const BITMAP_FLASH sound_add;
extern volatile const BITMAP_FLASH sound_delete;

extern volatile const BITMAP_FLASH UNMUTE_HOVER;
extern volatile const BITMAP_FLASH MUTE_HOVER;

extern volatile const BITMAP_FLASH Smart_MP3; 
extern volatile const BITMAP_FLASH Smart_MP3_Part1;
extern volatile const BITMAP_FLASH Smart_MP3_Part2;
extern volatile const BITMAP_FLASH Smart_MP3_Part3;
extern volatile const BITMAP_FLASH Smart_MP3_Part4;


/* Private function prototypes -----------------------------------------------*/
void StartMenu(void);


#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
