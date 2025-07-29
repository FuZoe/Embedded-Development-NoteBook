/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The SST25VF016B application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-10-29
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
#include "stm32f10x.h"
#include "SST25VF016B.h"
#include "FlashDriver.h"
#include "GLCD.h" 
#include "BMP.c"

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
	uint8_t  pic_head[2];				
	uint16_t pic_size_l;			    
	uint16_t pic_size_h;			   
	uint16_t pic_nc1;				  
	uint16_t pic_nc2;				   
	uint16_t pic_data_address_l;	   
	uint16_t pic_data_address_h;	
	uint16_t pic_message_head_len_l;
	uint16_t pic_message_head_len_h;
	uint16_t pic_w_l;				
	uint16_t pic_w_h;				   
	uint16_t pic_h_l;				   
	uint16_t pic_h_h;				   	
	uint16_t pic_bit;				   
	uint16_t pic_dip;				   
	uint16_t pic_zip_l;			       
	uint16_t pic_zip_h;			       
	uint16_t pic_data_size_l;		   
	uint16_t pic_data_size_h;		   
	uint16_t pic_dipx_l;			   
	uint16_t pic_dipx_h;			   	
	uint16_t pic_dipy_l;			   
	uint16_t pic_dipy_h;			   
	uint16_t pic_color_index_l;	       
	uint16_t pic_color_index_h;	       
	uint16_t pic_other_l;			   
	uint16_t pic_other_h;			   
	uint16_t pic_color_p01;		       
	uint16_t pic_color_p02;		       
	uint16_t pic_color_p03;		        
	uint16_t pic_color_p04;		       
	uint16_t pic_color_p05;		       
	uint16_t pic_color_p06;		       
	uint16_t pic_color_p07;		       
	uint16_t pic_color_p08;			    			
}BMP_HEAD;

BMP_HEAD bmp;

typedef struct
{
	uint16_t x;							/* LCD坐标X */
	uint16_t y;							/* LCD坐标Y	*/
	uint8_t  r;				            /* RED */
	uint8_t  g;						    /* GREEN */
	uint8_t  b;							/* BLUE	*/
}BMP_POINT;

BMP_POINT point;

/* Private define ------------------------------------------------------------*/
#define BMP_ADDR  (1024*0)		   /* BMP图片数据在SPI FLASH 存放位置 */


/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int main(void)
{
  uint16_t count;

  SPI_FLASH_Init();
  LCD_Initializtion();
  SPI_FLASH_Test();
  SSTF016B_Erase( BMP_ADDR/SEC_SIZE , ( BMP_ADDR+sizeof(BMP_DATA) )/SEC_SIZE +1 );	  /* 写之前要先刷除 */
  LCD_Clear(Black); 
  GUI_Text(80,112,"Writing To SPI Flash",White,Black);
  df_write_open( BMP_ADDR );    /* 要写的初始地址 */

  for(count=0;count<sizeof(BMP_DATA)/255;count++)
  {
    df_write( (uint8_t*)&BMP_DATA[count*255],255); 
    df_write_close(); 
  }
  df_write( (uint8_t*)&BMP_DATA[count*255],sizeof(BMP_DATA)%255);  
  df_write_close();   
   
  /* 把写入SPI Flash的BMP图片数据读出来 */
  df_read_open( BMP_ADDR );    /* 图片的初始地址 */
  df_read( (uint8_t *)&bmp, sizeof(bmp));   
  if( (bmp.pic_head[0]=='B') && (bmp.pic_head[1]=='M') )  /* 如果是BMP图片 */
  {
    uint16_t  tx,ty;
	uint8_t buffer[1024]; 

	df_read_seek( BMP_ADDR + ( bmp.pic_data_address_h<<16 | bmp.pic_data_address_l ) );  /* 跳过无用的数据 */
	for(ty=0;ty<bmp.pic_h_l;ty++)
	{	
	  df_read( &buffer[0] , (bmp.pic_w_l)*3 );
	  for(tx=0;tx<bmp.pic_w_l;tx++)
	  {
        point.r = *(tx*3 +2+buffer);
	    point.g = *(tx*3 +1+buffer);
	    point.b = *(tx*3 +0+buffer);								
	    point.x = tx;
	    point.y = ty;
        LCD_SetPoint( point.y,point.x,RGB565CONVERT(point.r,point.g,point.b));
      }                         
	}
  } 
  /* Infinite loop */
  while (1) ;
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

