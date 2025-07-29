/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : mass_mal.h
* Author             : MCD Application Team
* Version            : V3.2.1
* Date               : 07/05/2010
* Description        : Header for mass_mal.c file.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MASS_MAL_H
#define __MASS_MAL_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define MAL_OK   0
#define MAL_FAIL 1
#define MAX_LUN  1

/* STM32 Flash */
#define FLASH_PAGE_SIZE         0x800                      /* 2K per page */
#define FLASH_WAIT_TIMEOUT      100000 

#define USER_FLASH_START ( 0x8000000 + ( 16 * 1024 ) )	   /* Flash start address 16K */
#define USER_FLASH_END	 ( 0x8000000 + ( 0x40000 ) )
#define USER_FLASH_SIZE  ( USER_FLASH_END - USER_FLASH_START )

/* Mass Storage Memory Layout */
#define BOOT_SECT_SIZE 	MSC_BlockSize
#define ROOT_DIR_SIZE   (MSC_BlockSize * 1)
#define FAT_SIZE		(MSC_BlockSize * 2)

#define MSC_MemorySize  ( BOOT_SECT_SIZE + FAT_SIZE + ROOT_DIR_SIZE + USER_FLASH_SIZE ) 
#define MSC_BlockSize   512
#define MSC_BlockCount  (MSC_MemorySize / MSC_BlockSize)

#define BLOCKS_PER_CLUSTER  2
#define NO_OF_CLUSTERS 	(MSC_BlockCount/BLOCKS_PER_CLUSTER)

#define DIR_ENTRY 64
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

uint16_t MAL_Init (uint8_t lun);
uint16_t MAL_GetStatus (uint8_t lun);
uint16_t MAL_Read(uint8_t lun, uint32_t Memory_Offset, uint32_t *Readbuff, uint16_t Transfer_Length);
uint16_t MAL_Write(uint8_t lun, uint32_t Memory_Offset, uint32_t *Writebuff, uint16_t Transfer_Length);
#endif /* __MASS_MAL_H */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
