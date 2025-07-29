/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               fsmc_nor.h
** Descriptions:            The FSMC NOR application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2011-2-16
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

#ifndef __FSMC_NOR_H
#define __FSMC_NOR_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <string.h>
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  uint16_t Manufacturer_Code;
  uint16_t Device_Code1;
  uint16_t Device_Code2;
  uint16_t Device_Code3;
}
NOR_IDTypeDef;

/* NOR Status */
typedef enum
{
  NOR_SUCCESS = 0,
  NOR_ONGOING,
  NOR_ERROR,
  NOR_TIMEOUT
}
NOR_Status;

/* Private function prototypes -----------------------------------------------*/
void FSMC_NOR_Init(void);
void FSMC_NOR_Test(void);
void FSMC_NOR_ReadID(NOR_IDTypeDef* NOR_ID);
NOR_Status FSMC_NOR_EraseBlock(uint32_t BlockAddr);
NOR_Status FSMC_NOR_EraseChip(void);
NOR_Status FSMC_NOR_WriteHalfWord(uint32_t WriteAddr, uint16_t Data);
NOR_Status FSMC_NOR_WriteBuffer(uint16_t* pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite);
NOR_Status FSMC_NOR_ProgramBuffer(uint16_t* pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite);
u16 FSMC_NOR_ReadHalfWord(uint32_t ReadAddr);
void FSMC_NOR_ReadBuffer(uint16_t* pBuffer, uint32_t ReadAddr, uint32_t NumHalfwordToRead);
NOR_Status FSMC_NOR_ReturnToReadMode(void);
NOR_Status FSMC_NOR_Reset(void);
NOR_Status FSMC_NOR_GetStatus(uint32_t Timeout);

#endif /* __FSMC_NOR_H */

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

