/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               key.h
** Descriptions:            The key application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2011-3-16
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

#ifndef __KEY_H
#define __KEY_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Private define ------------------------------------------------------------*/
#define BUTTON_FILTER_TIME 	5		/* 按键滤波时间50ms 单位10ms 只有连续检测到50ms状态不变才认为有效，包括弹起和按下两种事件 */
#define BUTTON_LONG_TIME 	100		/* 持续1秒，认为长按事件 */
#define KEY_FIFO_SIZE	    20

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
	/* 下面是一个函数指针，指向判断按键手否按下的函数 */
	uint8_t (*IsKeyDownFunc)(void); /* 按键按下的判断函数,1表示按下 */

	uint8_t Count;			        /* 滤波器计数器 */
	uint8_t FilterTime;		        /* 滤波时间(最大255,表示2550ms) */
	uint16_t LongCount;		        /* 长按计数器 */
	uint16_t LongTime;		        /* 按键按下持续时间, 0表示不检测长按 */
	uint8_t  State;			        /* 按键当前状态（按下还是弹起） */
	uint8_t KeyCodeUp;		        /* 按键弹起的键值代码, 0表示不检测按键弹起 */
	uint8_t KeyCodeDown;	        /* 按键按下的键值代码, 0表示不检测按键按下 */
	uint8_t KeyCodeLong; 	        /* 按键长按的键值代码, 0表示不检测长按 */
}BUTTON_T;

/* 定义键值代码 */
typedef enum
{
	KEY_NONE = 0,			/* 0 表示按键事件 */

	/* 需要检测USER键弹起事件和长按事件 */
	KEY_DOWN_USER,			/* User键按下 */
	KEY_UP_USER,			/* User键弹起 */
	KEY_HOLD_USER,			/* User键长按 */

	KEY_DOWN_WAKEUP,		/* WakeUp键按下 */
	KEY_DOWN_TAMPER,		/* Tamper键按下 */

	KEY_DOWN_JOY_UP,		/* 摇杆UP键按下 */
	KEY_DOWN_JOY_DOWN,		/* 摇杆DOWN键按下 */
	KEY_DOWN_JOY_LEFT,		/* 摇杆LEFT键按下 */
	KEY_DOWN_JOY_RIGHT,		/* 摇杆RIGHT键按下 */
	KEY_DOWN_JOY_OK			/* 摇杆OK键按下 */
}KEY_ENUM;

/*	按键FIFO用到变量 */
typedef struct
{
	uint8_t Buf[KEY_FIFO_SIZE];		/* 键值缓冲区 */
	uint8_t Read;	                /* 缓冲区读指针 */
	uint8_t Write;	                /* 缓冲区写指针 */
}KEY_FIFO_T;

/* Private function prototypes -----------------------------------------------*/
void KeyPro(void);
uint8_t GetKey(void);
void Key_Initializtion(void);
void PutKey(uint8_t _KeyCode);

#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/



