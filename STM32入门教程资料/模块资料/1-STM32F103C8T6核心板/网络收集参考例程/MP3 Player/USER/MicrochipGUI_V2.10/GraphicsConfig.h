/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               GraphicsConfig.h
** Descriptions:            None
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-24
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

#ifndef _GRAPHICSCONFIG_H
#define _GRAPHICSCONFIG_H

//////////////////// COMPILE OPTIONS AND DEFAULTS ////////////////////

/*********************************************************************
* Overview: Defines color depth. 
********************************************************************/
#define COLOR_DEPTH						16


/*********************************************************************
* Overview: Horizontal and vertical display resolution
*                  (from the glass datasheet).
*********************************************************************/
#define DISP_HOR_RESOLUTION				240 
#define DISP_VER_RESOLUTION				320
/*********************************************************************
* Overview: Image orientation (can be 0, 90, 180, 270 degrees).
*********************************************************************/
#define DISP_ORIENTATION				0
/*********************************************************************
* Overview: Panel Data Width (can be 18 or 24 bits).
*********************************************************************/
#define DISP_DATA_WIDTH                 18
/*********************************************************************
* Overview: LSHIFT Polarity Swap
* If defined LSHIFT is a falling trigger
*********************************************************************/
#define DISP_INV_LSHIFT
/*********************************************************************
* Overview: Horizontal synchronization timing in pixels
*                  (from the glass datasheet).
*********************************************************************/
//#define DISP_HOR_PULSE_WIDTH		    25
//#define DISP_HOR_BACK_PORCH		    4
//#define DISP_HOR_FRONT_PORCH			0
/*********************************************************************
* Overview: Vertical synchronization timing in lines
*                  (from the glass datasheet).
*********************************************************************/
//#define DISP_VER_PULSE_WIDTH		    4
//#define DISP_VER_BACK_PORCH		    0
//#define DISP_VER_FRONT_PORCH			0


/*********************************************************************
* Overview: Blocking and Non-Blocking configuration selection. To
*			enable non-blocking configuration USE_NONBLOCKING_CONFIG
*			must be defined. If this is not defined, blocking 
*			configuration is assumed.
*
********************************************************************/
#define USE_NONBLOCKING_CONFIG // Comment this line to use blocking configuration

/*********************************************************************
* Overview: Keyboard control on some objects can be used by enabling
*			the GOL Focus (USE_FOCUS)support. 
*
*********************************************************************/
//#define USE_FOCUS

/*********************************************************************
* Overview: Input devices used defines the messages that Objects will
*			process. The following definitions indicate the usage of 
*			the different input device:
*			- USE_TOUCHSCREEN - enables the touch screen support.
*			- USE_KEYBOARD    - enables the key board support.
*
*********************************************************************/
#define USE_TOUCHSCREEN			// Enable touch screen support.
#define USE_KEYBOARD			// Enable key board support.

/*********************************************************************
* Overview: To save program memory, unused Widgets or Objects can be 
*			removed at compile time. 
*
*********************************************************************/
#define USE_GOL                 // Enable Graphics Object Layer.
#define USE_BUTTON				// Enable Button Object.
//#define USE_WINDOW				// Enable Window Object.
//#define USE_CHECKBOX			// Enable Checkbox Object.
//#define USE_RADIOBUTTON			// Enable Radio Button Object.
//#define USE_EDITBOX				// Enable Edit Box Object.
#define USE_LISTBOX				// Enable List Box Object.
#define USE_SLIDER 				// Enable Slider or Scroll Bar Object.
//#define USE_PROGRESSBAR			// Enable Progress Bar Object.
//#define USE_STATICTEXT			// Enable Static Text Object.
#define USE_PICTURE				// Enable Picture Object.
//#define USE_GROUPBOX			// Enable Group Box Object.
//#define USE_ROUNDDIAL			// Enable Dial Object.
//#define USE_METER				// Enable Meter Object.
//#define USE_DIGITALMETER
//#define USE_GRID
//#define USE_CUSTOM				// Enable Custom Control Object (an example to create customized Object).

/*********************************************************************
* Overview: To enable support for unicode fonts, USE_MULTIBYTECHAR  
*			must be defined. This changes XCHAR definition. See XCHAR 
*			for details.
*
*********************************************************************/
//#define USE_MULTIBYTECHAR

/*********************************************************************
* Overview: Font data can be placed in two locations. One is in 
*			FLASH memory and the other is from external memory.
*			Definining one or both enables the support for fonts located
*			in internal flash and external memory. 
*	- USE_FONT_FLASH - Font in internal flash memory support.	
*	- USE_FONT_EXTERNAL - Font in external memory support.	
*
*********************************************************************/
#define USE_FONT_FLASH 			// Support for fonts located in internal flash
#define USE_FONT_EXTERNAL		// Support for fonts located in external memory

/*********************************************************************
* Overview: Similar to Font data bitmaps can also be placed in 
*			two locations. One is in FLASH memory and the other is 
*			from external memory.
*			Definining one or both enables the support for bitmaps located
*			in internal flash and external memory. 
*	- USE_BITMAP_FLASH - Font in internal flash memory support.	
*	- USE_BITMAP_EXTERNAL - Font in external memory support.	
*
*********************************************************************/
#define USE_BITMAP_FLASH		// Support for bitmaps located in internal flash
//#define USE_BITMAP_EXTERNAL		// Support for bitmaps located in external memory
					

#define GFX_malloc malloc
#define GFX_free   free

#endif // _GRAPHICSCONFIG_H
