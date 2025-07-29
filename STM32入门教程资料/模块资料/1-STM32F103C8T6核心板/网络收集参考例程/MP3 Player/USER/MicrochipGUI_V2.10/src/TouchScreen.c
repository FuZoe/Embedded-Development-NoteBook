/****************************************Copyright (c)**************************************************                         
**
**                                 http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			TouchScreen.c
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
#include "TouchScreen.h"


/*********************************************************************
* Function: void TouchInit(void)
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: sets ADC 
*
* Note: none
*
********************************************************************/
void TouchInit(void)
{
	TP_Init();
}

/*********************************************************************
* Function: SHORT TouchGetX()
*
* PreCondition: none
*
* Input: none
*
* Output: x coordinate
*
* Side Effects: none
*
* Overview: returns x coordinate if touch screen is pressed
*           and -1 if not
*
* Note: none
*
********************************************************************/


/*********************************************************************
* Function: SHORT TouchGetY()
*
* PreCondition: none
*
* Input: none
*
* Output: y coordinate
*
* Side Effects: none
*
* Overview: returns y coordinate if touch screen is pressed
*           and -1 if not
*
* Note: none
*
********************************************************************/

/*********************************************************************
* Function: void TouchCalibration()
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: Runs the calibration routine. 
*
* Note: none
*
********************************************************************/
void TouchCalibration(void)
{
	TouchPanel_Calibrate();
}


/*********************************************************************
* Function: void TouchGetMsg(GOL_MSG* pMsg)
*
* PreCondition: none
*
* Input: pointer to the message structure to be populated
*
* Output: none
*
* Side Effects: none
*
* Overview: populates GOL message structure
*
* Note: none
*
********************************************************************/
void TouchGetMsg(GOL_MSG* pMsg)
{
	static SHORT prevX = -1;
	static SHORT prevY = -1;
	SHORT x,y;

    getDisplayPoint(&display,Read_Ads7846(),&matrix); 	

    if ((display.x < 0) || (display.x > MAX_Y)  || (display.y < 0) || (display.y > MAX_X))
    {
	       display.x = -1;
	       display.y = -1;
	}	    
 
    x = display.x;  
	y = display.y;
    
	pMsg->type    = TYPE_TOUCHSCREEN;
    pMsg->uiEvent = EVENT_INVALID;

    if( (prevX == x) && (prevY == y) ) 
	{
    	pMsg->uiEvent = EVENT_STILLPRESS;
        pMsg->param1 = x;
        pMsg->param2 = y;
        return;
    }    

    if( (prevX != -1) || (prevY != -1) )
    {

        if( (x != -1) && (y != -1) )
        {
            pMsg->uiEvent = EVENT_MOVE;	  
        }
		else
		{
            pMsg->uiEvent = EVENT_RELEASE;
            pMsg->param1 = prevX;
            pMsg->param2 = prevY;
            prevX = x;
            prevY = y;
            
			return;
        }
    }
	else
	{
        if( (x != -1) && (y != -1) )
		{
            pMsg->uiEvent = EVENT_PRESS;
        }
		else
		{
			pMsg->uiEvent = EVENT_INVALID;
        }

    }

    pMsg->param1 = x;
    pMsg->param2 = y;
    prevX = x;
    prevY = y;

}

/*********************************************************************
* Function: void TouchStoreCalibration(void)
*
* PreCondition: EEPROMInit() must be called before
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: stores calibration parameters into EEPROM
*
* Note: none
*
********************************************************************/





/*********************************************************************
* Function: void TouchGetCalPoints(void)
*
* PreCondition: InitGraph() must be called before
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: gets values for 3 touches
*
* Note: none
*
********************************************************************/
void 	TouchGetCalPoints(void)
{
}
