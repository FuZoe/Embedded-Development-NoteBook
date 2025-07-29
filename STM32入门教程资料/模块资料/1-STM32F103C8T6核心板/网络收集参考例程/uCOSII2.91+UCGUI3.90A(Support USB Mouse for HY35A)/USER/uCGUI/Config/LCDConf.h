/****************************************Copyright (c)**************************************************                         
**
**                                 http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			LCDConf.c
** Descriptions:		LCD Configuration
**						
**------------------------------------------------------------------------------------------------------
** Created by:			AVRman
** Created date:		2010-11-9
** Version:				1.0
** Descriptions:		The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:			
** Modified date:	
** Version:
** Descriptions:		
********************************************************************************************************/


#ifndef LCDCONF_H
#define LCDCONF_H

/* Private define ------------------------------------------------------------*/
#define HY32D              (8989)

#define LCD_XSIZE          (320)
#define LCD_YSIZE          (240)
#define LCD_CONTROLLER     (HY32D)
#define LCD_BITSPERPIXEL   (16)
#define LCD_FIXEDPALETTE   (565)
#define LCD_SWAP_RB        (1)
//#define LCD_SWAP_XY        (1)
#define LCD_INIT_CONTROLLER()  LCD_Initializtion()


#endif
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

