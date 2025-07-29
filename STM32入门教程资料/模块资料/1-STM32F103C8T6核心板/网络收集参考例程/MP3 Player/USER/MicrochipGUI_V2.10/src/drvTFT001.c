/****************************************Copyright (c)**************************************************                         
**
**                                 http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			drvTFT001.c
** Descriptions:		None
**
**------------------------------------------------------------------------------------------------------
** Created by:			AVRman
** Created date:		2010-11-24
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
#include "Graphics.h"

/* Private variables ---------------------------------------------------------*/
WORD   _color;	  /* Color */
SHORT _clipRgn;	  /* Clipping region control */
SHORT _clipLeft;  /* Clipping region borders */
SHORT _clipTop;
SHORT _clipRight;
SHORT _clipBottom;



/*
*********************************************************************************************************
*                                          LOCAL DEFINES
*********************************************************************************************************
*/
#define WritePixel(color)  



/*********************************************************************
* Function:  void ResetDevice()
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: resets LCD, initializes PMP
*
* Note: none
*
********************************************************************/
void ResetDevice(void){
	LCD_Initializtion();
}



