/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               Graphics.h
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

#ifndef _GRAPHICS_H
#define _GRAPHICS_H


/////////////////////// GRAPHICS_LIBRARY_VERSION /////////////////////
// MSB is version, LSB is subversion
#define GRAPHICS_LIBRARY_VERSION  0x0210

////////////////////////////// INCLUDES //////////////////////////////
#include <includes.h>


#include "GenericTypeDefs.h"
#include "GraphicsConfig.h"
#include "Primitive.h"  // Graphic primitives layer
#include "ScanCodes.h"  // Scan codes for AT keyboard
#include "GOL.h"        // GOL layer 

#include "GUIFonts_ASCII.h"
#include "AsciiLib.h"
#include "drvTFT001.h"
#include "TouchScreen.h"



#if defined (USE_BUTTON) || defined (USE_BUTTON_MULTI_LINE)
    #include "Button.h"
#endif
#ifdef USE_WINDOW
    #include "Window.h"
#endif
#ifdef USE_GROUPBOX
    #include "GroupBox.h"
#endif
#ifdef USE_STATICTEXT
    #include "StaticText.h"
#endif
#ifdef USE_SLIDER
    #include "Slider.h"
#endif
#ifdef USE_CHECKBOX
    #include "CheckBox.h"
#endif
#ifdef USE_RADIOBUTTON
    #include "RadioButton.h"
#endif
#ifdef USE_PICTURE
    #include "Picture.h"
#endif
#ifdef USE_PROGRESSBAR
    #include "ProgressBar.h"
#endif
#ifdef USE_EDITBOX
    #include "EditBox.h"
#endif
#ifdef USE_LISTBOX
    #include "ListBox.h"
#endif
#ifdef USE_ROUNDDIAL
    #include "RoundDial.h"
#endif
#ifdef USE_METER
    #include "Meter.h"
#endif
#ifdef USE_GRID
    #include "Grid.h"
#endif
#ifdef USE_CHART
    #include "Chart.h"
#endif
#ifdef USE_TEXTENTRY
    #include "TextEntry.h"
#endif
#ifdef USE_DIGITALMETER
    #include "DigitalMeter.h"
#endif
#ifdef USE_CUSTOM
 /* Included for custom control demo */  
//  #include "CustomControlDemo.h"	
#endif


/*********************************************************************
* Macros: RGB565CONVERT(red, green, blue)
*
* Overview: Converts true color into 5:6:5 RGB format.
*
* PreCondition: none
*
* Input: Red, Green, Blue components.
*
* Output: 5 bits red, 6 bits green, 5 bits blue color.
*
* Side Effects: none
*
********************************************************************/
#define RGB565CONVERT(red, green, blue) (WORD) (((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3))

/*********************************************************************
* Macros: RGB332CONVERT(red, green, blue)
*
* Overview: Converts true color into 5:6:5 RGB format.
*
* PreCondition: none
*
* Input: Red, Green, Blue components.
*
* Output: 5 bits red, 6 bits green, 5 bits blue color.
*
* Side Effects: none
*
********************************************************************/
#define RGB332CONVERT(red, green, blue) (BYTE) ((red & 0xE0) | ((green & 0xE0) >> 3) | (blue >> 6))


#endif
