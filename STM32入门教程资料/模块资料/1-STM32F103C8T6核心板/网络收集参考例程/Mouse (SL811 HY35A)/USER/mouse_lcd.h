/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               mouse_lcd.h
** Descriptions:            The mouse lcd application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2011-4-11
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

#ifndef __MOUSE_LCD_H
#define __MOUSE_LCD_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "SL811mouse.h"

/* Private define ------------------------------------------------------------*/

/*Left Button : Report data :0x01*/
#define HID_MOUSE_BUTTON1                0x01 
/*Right Button : Report data :0x02*/
#define HID_MOUSE_BUTTON2                0x02 
/*Middle Button : Report data : 0x04*/
#define HID_MOUSE_BUTTON3                0x04 

/* Mouse directions */
#define MOUSE_WINDOW_X                  2
#define MOUSE_WINDOW_Y                  2
#define MOUSE_WINDOW_HEIGHT             236
#define MOUSE_WINDOW_WIDTH              285

#define HID_MOUSE_BUTTON_HEIGHT         10
#define HID_MOUSE_BUTTON_WIDTH          24
#define HID_MOUSE_BUTTON_XCHORD         MOUSE_WINDOW_X + MOUSE_WINDOW_WIDTH + 6
#define HID_MOUSE_BUTTON1_YCHORD        200
#define HID_MOUSE_BUTTON3_YCHORD        110
#define HID_MOUSE_BUTTON2_YCHORD        30 	   

/* Private function prototypes -----------------------------------------------*/
void USR_MOUSE_Init	(void);
void HID_MOUSE_UpdatePosition(int8_t x ,int8_t y);
void HID_MOUSE_ButtonReleased(uint8_t button_idx);
void HID_MOUSE_ButtonPressed (uint8_t button_idx);
void USR_MOUSE_ProcessData(struct HidMouseStruct *data);

#endif 

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
