/****************************************Copyright (c)**************************************************                         
**
**                                 http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			search.h
** Descriptions:		内存查找函数库头文件
**
**------------------------------------------------------------------------------------------------------
** Created by:			AVRman
** Created date:		2011-4-4
** Version:				2.0
** Descriptions:		The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:			
** Modified date:	
** Version:
** Descriptions:		
********************************************************************************************************/
#ifndef	__search_h
#define	__search_h

/* Includes ------------------------------------------------------------------*/
#include <includes.h>

/* Private function prototypes -----------------------------------------------*/
void *MemSearch(void *pvMem, INT32U u32MemSize, INT32U u32Data, INT32U u32DataSize);
INT8S *MemChr(INT8S *pbMem, INT8S bBYTE, INT32U u32MemSize);
BOOLEAN memstr(INT8S *pbMem, INT8S *pbStr, INT32U u32StrSize, INT32U u32MemSize);
INT8S *memstrExt(INT8S *pbMem, INT8S *pbStr, INT32U u32StrSize, INT32U u32MemSize);

#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

