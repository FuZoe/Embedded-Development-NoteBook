/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : mass_mal.c
* Author             : MCD Application Team
* Version            : V3.2.1
* Date               : 07/05/2010
* Description        : Medium Access Layer interface
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "stm32f10x.h"
#include "mass_mal.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t Mass_Memory_Size[1];
uint32_t Mass_Block_Size[1];
uint32_t Mass_Block_Count[1];

uint8_t Fat_RootDir[FAT_SIZE + ROOT_DIR_SIZE];   /* 1536 */

uint32_t Flash_Buf[ FLASH_PAGE_SIZE/4 ];

const uint8_t BootSect[] = {
0xEB,0x3C,0x90,0x4D,0x53,0x44,0x4F,0x53,0x35,0x2E,0x30,0x00,0x02,BLOCKS_PER_CLUSTER,0x01,0x00,
0x01,0x10,0x00,0xEC,0x03,0xF8,0x02,0x00,0x01,0x00,0x01,0x00,0x00,0x00,
};

 const uint8_t RootDirEntry[DIR_ENTRY] = {
 'B', 'o', 'o', 't', 'l', 'o', 'a','d', 'e', 'r', 0x28,0x28,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 'F', 'I', 'R', 'M', 'W', 'A', 'R', 'E', 'B', 'I', 'N',0x20,0x18,0xbc,0x41,0x97,
0x37,0x38,0x37,0x38,0x00,0x00,0x3d,0x6e,0x2b,0x38,0x02,0x00,0x00,0xD0,0x07,0x00,
 };

/* Private function prototypes -----------------------------------------------*/
void translate_rd( uint32_t offset, uint8_t *Readbuff, uint32_t length);
void translate_wr( uint32_t offset, uint32_t *Writebuff ,uint32_t length);

/*******************************************************************************
* Function Name  : MAL_Init
* Description    : Initializes the Media on the STM32
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t MAL_Init(uint8_t lun)
{
  switch (lun)
  {
    case 0:
	   memset(Flash_Buf,0,FLASH_PAGE_SIZE);
       break;
    case 1:
	  return MAL_FAIL;
    default:
      return MAL_FAIL;
  }
  return MAL_OK;
}

/*******************************************************************************
* Function Name  : MAL_Write
* Description    : Write sectors
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t MAL_Write(uint8_t lun, uint32_t Memory_Offset, uint32_t *Writebuff, uint16_t Transfer_Length)
{
  switch (lun)
  {
    case 0:	

	  translate_wr( Memory_Offset , Writebuff , Transfer_Length );

      break;
    case 1:
      break;
    default:
      return MAL_FAIL;
  }
  return MAL_OK;
}

/*******************************************************************************
* Function Name  : MAL_Read
* Description    : Read sectors
* Input          : None
* Output         : None
* Return         : Buffer pointer
*******************************************************************************/
uint16_t MAL_Read(uint8_t lun, uint32_t Memory_Offset, uint32_t *Readbuff, uint16_t Transfer_Length)
{
  switch (lun)
  {
    case 0:

  	  translate_rd( Memory_Offset , (uint8_t *)Readbuff , Transfer_Length );

      break;
    case 1:
      break;
    default:
      return MAL_FAIL;
  }
  return MAL_OK;
}

/*******************************************************************************
* Function Name  : MAL_GetStatus
* Description    : Get status
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t MAL_GetStatus (uint8_t lun)
{
  if (lun == 0)
  {		 
      Mass_Block_Count[0] = MSC_BlockCount; 
      Mass_Block_Size[0] =  MSC_BlockSize; 
      Mass_Memory_Size[0] = MSC_MemorySize; 
      return MAL_OK;
  }

  return MAL_FAIL;
}

void translate_wr( uint32_t offset, uint32_t *Writebuff ,uint32_t length)
{
  uint32_t i;

    if (offset < BOOT_SECT_SIZE)
	{
	  /* Can't write boot sector */
	}
	else if ( offset < (BOOT_SECT_SIZE + FAT_SIZE + ROOT_DIR_SIZE) )
         {
           for ( i = 0; i<length; i++)
	       {
		     Fat_RootDir[(offset+i) - BOOT_SECT_SIZE] = Writebuff[i];
			 if ( Writebuff[i] == 0xe5 )
			 {
			 
			 }
		   }
         }
		 else
		 {				
			for( i = 0; i < length; i+=4 )
	        { 				  
			   FLASH_ProgramWord( USER_FLASH_START + ( offset - (BOOT_SECT_SIZE + FAT_SIZE + ROOT_DIR_SIZE) ) + i , Writebuff[i>>2] ); 	   		 
	        } 			
		 }
}

void translate_rd( uint32_t offset, uint8_t *Readbuff, uint32_t length)
{
  uint32_t i;
  uint8_t data;
  vu8 * firmware ;
  firmware = (vu8*)USER_FLASH_START;
  for ( i = 0; i<length; i++)
  {
    if (offset < BOOT_SECT_SIZE)
	{
      switch (offset)
	  {
	    case 19:
		  data = (uint8_t)(MSC_BlockCount & 0xFF);
		break;
		case 20:
		  data = (uint8_t)((MSC_BlockCount >> 8) & 0xFF);
		break;
		case 510:
		  data = 0x55;
		break;
		case 511:
		  data = 0xAA;
		break;
		default:
		if ( offset > 29 )
		{
	      data = 0x0;
		}
		else
		{
		  data = BootSect[offset];
		}
		break;
	  }	  
	}
	else if (offset < (BOOT_SECT_SIZE + FAT_SIZE + ROOT_DIR_SIZE))
	     {
		   data = Fat_RootDir[offset - BOOT_SECT_SIZE];
		 }
		 else
		 {
#ifdef  CODE_PROTECT
		     data = 0x0;
#else
		     data = *(firmware + (offset - (BOOT_SECT_SIZE + FAT_SIZE + ROOT_DIR_SIZE)));
#endif
		 }
    Readbuff[i] = data;
	offset++;
  }
}

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
