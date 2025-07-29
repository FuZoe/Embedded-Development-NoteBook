/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               mouse_lcd.c
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

/* Includes ------------------------------------------------------------------*/
#include "GLCD.h"
#include "mouse_lcd.h"
#include "SL811mouse.h"

/* Private variables ---------------------------------------------------------*/
int16_t  x_loc  = 0, y_loc  = 0; 
int16_t  prev_x = 0, prev_y = 0;
extern uint16_t TextColor,BackColor;

/*******************************************************************************
* Function Name  : USR_MOUSE_ProcessData
* Description    : Process Mouse data
* Input          : Mouse data to be displayed
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void USR_MOUSE_ProcessData(struct HidMouseStruct *data)
{ 
	static uint8_t b_state[3] = { 0, 0 , 0};
	
	if ((data->x != 0) && (data->y != 0))
	{
		HID_MOUSE_UpdatePosition(data->x , data->y);
	}

	if( data->button_L ) 
	{
		if(b_state[0] == 0)
		{
			HID_MOUSE_ButtonPressed (0);
			b_state[0] = 1;
		}
	}
	else
	{
		if(b_state[0] == 1)
		{
			HID_MOUSE_ButtonReleased (0);
			b_state[0] = 0;
		}
	}

	if( data->button_R ) 
	{
		if(b_state[1] == 0)
		{
			HID_MOUSE_ButtonPressed (1);
			b_state[1] = 1;
		}
	}
	else
	{
		if(b_state[1] == 1)
		{
			HID_MOUSE_ButtonReleased (1);
			b_state[1] = 0;
		}
	}

	if( data->button_M ) 
	{
		if(b_state[2] == 0)
		{
			HID_MOUSE_ButtonPressed (2);
			b_state[2] = 1;
		}
	}
	else
	{
		if(b_state[2] == 1)
		{
			HID_MOUSE_ButtonReleased (2);
			b_state[2] = 0;
		}
	}
}

/*******************************************************************************
* Function Name  : HID_MOUSE_UpdatePosition
* Description    : The function is to handle mouse scroll to upadte the mouse position
*				   on display window
* Input          : - x: USB HID Mouse X co-ordinate
*                  - y: USB HID Mouse Y co-ordinate
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void HID_MOUSE_UpdatePosition (int8_t x, int8_t y)
{
  if ((x != 0) || (y != 0)) 
  {	    
    x_loc += x/2;
    y_loc += y/2;
    
    if(y_loc > ( MOUSE_WINDOW_Y + MOUSE_WINDOW_HEIGHT - 18 ) ){
      y_loc = MOUSE_WINDOW_Y + MOUSE_WINDOW_HEIGHT - 18 ;
    }
    if(y_loc < ( MOUSE_WINDOW_Y + 1 ) ){
      y_loc = MOUSE_WINDOW_Y + 1;
    }  
	  
    if(x_loc > ( MOUSE_WINDOW_X + MOUSE_WINDOW_WIDTH - 8 ) ){
      x_loc = MOUSE_WINDOW_X + MOUSE_WINDOW_WIDTH - 8 ;
    }  

    if(x_loc < ( MOUSE_WINDOW_X + 1 ) ){
      x_loc = MOUSE_WINDOW_X + 1;
    }  
 
    if ((prev_x != 0) && (prev_y != 0))
    {
      PutChar( prev_x , prev_y , ' ' , Green , Black);
    }
    
    PutChar  ( x_loc , y_loc , 'x' , Green , Black);
    
    prev_x = x_loc;
    prev_y = y_loc;
  }
}  

/*******************************************************************************
* Function Name  : HID_MOUSE_ButtonPressed
* Description    : The function is to handle mouse button press
* Input          : - button_idx: mouse button pressed
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void HID_MOUSE_ButtonPressed(uint8_t button_idx)
{

  /* Set the color for button press status */
  LCD_SetTextColor(Green);
  LCD_SetBackColor(Green);
  
  /* Change the color of button pressed to indicate button press*/
  switch (button_idx)
  {
    /* Left Button Pressed */
  case 0 :
    LCD_DrawFullRect(HID_MOUSE_BUTTON_XCHORD, HID_MOUSE_BUTTON1_YCHORD,\
    HID_MOUSE_BUTTON_WIDTH, HID_MOUSE_BUTTON_HEIGHT );
    break;
    
    
    /* Right Button Pressed */  
  case 1 :
    LCD_DrawFullRect(HID_MOUSE_BUTTON_XCHORD,HID_MOUSE_BUTTON2_YCHORD,\
       HID_MOUSE_BUTTON_WIDTH,HID_MOUSE_BUTTON_HEIGHT);
    break; 
    
    /* Middle button Pressed */  
  case 2 :
    LCD_DrawFullRect(HID_MOUSE_BUTTON_XCHORD,HID_MOUSE_BUTTON3_YCHORD,\
      HID_MOUSE_BUTTON_WIDTH,HID_MOUSE_BUTTON_HEIGHT);
    break;
    
  }
}

/*******************************************************************************
* Function Name  : HID_MOUSE_ButtonReleased
* Description    : The function is to handle mouse button release
* Input          : - button_idx: mouse button released
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void HID_MOUSE_ButtonReleased(uint8_t button_idx)
{
  
  /* Set the color for release status */
  LCD_SetTextColor(White);
  LCD_SetBackColor(White);
  
  /* Change the color of button released to default button color*/
  switch (button_idx)
  {
    /* Left Button Released */
  case 0 :
    LCD_DrawFullRect(HID_MOUSE_BUTTON_XCHORD, HID_MOUSE_BUTTON1_YCHORD,\
       HID_MOUSE_BUTTON_WIDTH, HID_MOUSE_BUTTON_HEIGHT);
    break;
    
    /* Right Button Released */  
  case 1 :
    LCD_DrawFullRect(HID_MOUSE_BUTTON_XCHORD, HID_MOUSE_BUTTON2_YCHORD,\
       HID_MOUSE_BUTTON_WIDTH, HID_MOUSE_BUTTON_HEIGHT);
    break;
    
    /* Middle Button Released */  
  case 2 :
    LCD_DrawFullRect (HID_MOUSE_BUTTON_XCHORD, HID_MOUSE_BUTTON3_YCHORD,\
       HID_MOUSE_BUTTON_WIDTH, HID_MOUSE_BUTTON_HEIGHT);
    break;
  }
}

/*******************************************************************************
* Function Name  : USR_MOUSE_Init
* Description    : Init Mouse window
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void USR_MOUSE_Init	(void)
{
  /* Set default screen color*/
  LCD_Clear(Black);
  
  /* Display the application header */
  LCD_SetBackColor(Blue);  
  LCD_SetTextColor(White);  

  LCD_SetTextColor(White);    
  
  /* Display Mouse Window */
  LCD_DrawRect(MOUSE_WINDOW_X,
               MOUSE_WINDOW_Y, 
               MOUSE_WINDOW_WIDTH,
               MOUSE_WINDOW_HEIGHT);
  
  HID_MOUSE_ButtonReleased(0);
  HID_MOUSE_ButtonReleased(1);
  HID_MOUSE_ButtonReleased(2);
  
  LCD_SetTextColor(Green);
  LCD_SetBackColor(Black);
 
  PutChar( MOUSE_WINDOW_X + MOUSE_WINDOW_WIDTH / 2 - 4 , MOUSE_WINDOW_Y + MOUSE_WINDOW_HEIGHT / 2 - 8  , 'x' , TextColor , BackColor);	  

  x_loc  = MOUSE_WINDOW_X + MOUSE_WINDOW_WIDTH / 2 - 4 ;
  y_loc  = MOUSE_WINDOW_Y + MOUSE_WINDOW_HEIGHT / 2 - 8; 
  prev_x = x_loc;
  prev_y = y_loc; 
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
