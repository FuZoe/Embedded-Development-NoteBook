/****************************************Copyright (c)**************************************************                         
**
**                                 http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			search.c
** Descriptions:		提供特殊功能的内存查找函数
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

/* Includes ------------------------------------------------------------------*/
#include	"search.h"

/*******************************************************************************
* Function Name  : MemSearch
* Description    : 在指定大小的内存区域查找数据，该函数可以查找任意类型的数据
* Input          : - pvMem: 指向指定的内存区域指针
*				   - u32MemSize: 被检索内存区域的大小
*				   - u32Data: 被检索的数据
*				   - u32DataSize: 被检索的数据大小
* Output         : None
* Return         : 找到则返回位置指针，否则返回值为NULL
* Attention		 : None
*******************************************************************************/
void *MemSearch(void *pvMem, INT32U u32MemSize, INT32U u32Data, INT32U u32DataSize)
{
	const INT8U		*__pubAddr = pvMem;	
	INT8S 			*__pbData = (INT8S *)&u32Data;
	INT32S 			i;
	BOOLEAN			__blIsMatch;
	
	do{
		__pubAddr = (INT8U*)MemChr((INT8S*)__pubAddr, *__pbData, ((INT8U *)pvMem + u32MemSize) - __pubAddr);
		if(__pubAddr != NULL)
		{
			__blIsMatch = 1;
			for(i=1; i<u32DataSize; i++)
			{
				if(*(__pubAddr + i) != *(__pbData + i))
				{
					__blIsMatch = 0;
					break;
				}
			}
		
			if(__blIsMatch)
				return (void *)__pubAddr;
		
			__pubAddr++;	
		}
		else
			break;		
	}while(1);
	
	return NULL;
}

/*******************************************************************************
* Function Name  : MemChr
* Description    : 在指定的内存区域查找字符
* Input          : - pbMem: 指向指定的内存区域指针
*				   - bBYTE: 被检索的字符
*				   - u32MemSize: 内存大小
* Output         : None
* Return         : 返回找到的实际位置
* Attention		 : None
*******************************************************************************/

INT8S *MemChr(INT8S *pbMem, INT8S bBYTE, INT32U u32MemSize)
{
	INT8S		*__pbNext = pbMem;
	INT32S		i;

	for(i=0; i<u32MemSize; i++)
	{
		if(*(__pbNext+i) == bBYTE)
			return (__pbNext + i);
	}
	
	return NULL;
}

/*******************************************************************************
* Function Name  : memstr
* Description    : 在指定的内存区域查找字符串
* Input          : - pbMem: 指向指定的内存区域指针
*				   - pbStr: 被检索的字符串
*                  - u32StrSize: 被检索的字符串大小
*				   - u32MemSize: 内存大小
* Output         : None
* Return         : 找到则返回TRUE，否则返回FALSE
* Attention		 : None
*******************************************************************************/
BOOLEAN memstr(INT8S *pbMem, INT8S *pbStr, INT32U u32StrSize, INT32U u32MemSize)
{
	INT8S *__pbNext = pbMem;
	
	__pbNext = MemChr(__pbNext, (INT8S)pbStr[0], u32MemSize);
	while(__pbNext != NULL)
	{
		if(__pbNext != NULL)
		{
			if(strncmp( (void*)__pbNext, (void*)pbStr, u32StrSize) == 0)
				return 1;
			
			__pbNext += 1;
			__pbNext = MemChr(__pbNext, (INT8S)pbStr[0], (pbMem + u32MemSize) - __pbNext);
		}
	}
		
	return 0;
}

/*******************************************************************************
* Function Name  : memstrExt
* Description    : 在指定的内存区域查找字符串
* Input          : - pbMem: 指向指定的内存区域指针
*				   - pbStr: 被检索的字符串
*                  - u32StrSize: 被检索的字符串大小
*				   - u32MemSize: 内存大小
* Output         : None
* Return         : 找到返回字符串所在的位置指针，否则返回NULL
* Attention		 : None
*******************************************************************************/
INT8S *memstrExt(INT8S *pbMem, INT8S *pbStr, INT32U u32StrSize, INT32U u32MemSize)
{
	INT8S *__pbNext = pbMem;
	
	__pbNext = MemChr(__pbNext, (INT8S)pbStr[0], u32MemSize);
	while(__pbNext != NULL)
	{
		if(__pbNext != NULL)
		{
			if(strncmp( (void*)__pbNext, (void*)pbStr, u32StrSize) == 0)
				return __pbNext;
			
			__pbNext += 1;
			__pbNext = MemChr(__pbNext, (INT8S)pbStr[0], (pbMem + u32MemSize) - __pbNext);
		}
	}
		
	return NULL;
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
