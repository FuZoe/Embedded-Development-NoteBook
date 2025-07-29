/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               SL811mouse.h
** Descriptions:            The SL811 Mouse application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2011-2-17
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

#ifndef __SL811MOUSE_H
#define __SL811MOUSE_H

/* Includes ------------------------------------------------------------------*/
#include "Host_SL811.h"
#include "stm32f10x.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
struct HidMouseStruct
{
	int button_L,button_R,button_M;
	int x;
	int y;
	int wheel;
};

/* Private function prototypes -----------------------------------------------*/
int MouseInit(void);
void MouseTask(void);
int sl811Initformouse(void);
int EnumUsbDevformouse(uint8_t usbaddr);

#endif 

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
