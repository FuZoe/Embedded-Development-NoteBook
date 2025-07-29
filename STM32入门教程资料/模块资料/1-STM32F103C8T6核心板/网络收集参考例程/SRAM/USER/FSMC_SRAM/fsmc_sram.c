/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               fsmc_sram.c
** Descriptions:            The FSMC SRAM application function
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

/* Includes ------------------------------------------------------------------*/
#include "fsmc_sram.h"
#include <stdio.h>

/* Private define ------------------------------------------------------------*/
#define SRAM_BLANK1_START_ADDR  ((uint32_t)0x68000000)
#define SRAM_BLANK1_END_ADDR	((uint32_t)0x680FFFFF)

#define SRAM_BLANK2_START_ADDR  ((uint32_t)0x68100000)
#define SRAM_BLANK2_END_ADDR	((uint32_t)0x681FFFFF)


static FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;	 /* 在函数内定义会出现莫名奇妙的错误 */
  
/*******************************************************************************
* Function Name  : FSMC_SRAM_Init
* Description    : Configures the FSMC and GPIOs to interface with the SRAM memory.
*				   This function must be called before any write/read operation
*				   on the SRAM.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void FSMC_SRAM_Init(void)
{ 
  GPIO_InitTypeDef GPIO_InitStructure; 
  FSMC_NORSRAMTimingInitTypeDef  FSMC_NORSRAMTimingInitStructure;;

  /* Enable FSMC, GPIOD, GPIOE, GPIOF, GPIOG and AFIO clocks */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF | 
                         RCC_APB2Periph_GPIOG | RCC_APB2Periph_AFIO , ENABLE);
  
  /*-- GPIO Configuration ------------------------------------------------------*/
  /* SRAM Data lines configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 |
                                GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure); 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  /* SRAM Address lines configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | 
                                GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 | 
                                GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  																		                    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | 
                                GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);
   
  /* NOE and NWE configuration */  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* NE3 configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_12 ; 
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  
  /* NBL0, NBL1 configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; 
  GPIO_Init(GPIOE, &GPIO_InitStructure); 
  
/*-- FSMC Configuration ------------------------------------------------------*/
  FSMC_NORSRAMTimingInitStructure.FSMC_AddressSetupTime = 3;
  FSMC_NORSRAMTimingInitStructure.FSMC_AddressHoldTime = 0;
  FSMC_NORSRAMTimingInitStructure.FSMC_DataSetupTime = 3;
  FSMC_NORSRAMTimingInitStructure.FSMC_BusTurnAroundDuration = 0;
  FSMC_NORSRAMTimingInitStructure.FSMC_CLKDivision = 0;
  FSMC_NORSRAMTimingInitStructure.FSMC_DataLatency = 0;
  FSMC_NORSRAMTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 

  /* Enable FSMC Bank1_SRAM Bank */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);  
}


/*******************************************************************************
* Function Name  : FSMC_SRAM_Test
* Description    : test SRAM
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void FSMC_SRAM_Test(void)
{
	__IO uint32_t addr;
	
	printf("SRAM BLANK1测试....\r\n");

	for( addr = SRAM_BLANK1_START_ADDR; addr < SRAM_BLANK1_END_ADDR + 1 ; addr += 4 )
	{
	    *(uint32_t *)addr = addr;
	}
	
	for( addr = SRAM_BLANK1_START_ADDR; addr < SRAM_BLANK1_END_ADDR + 1 ; addr += 4 )
	{
		if (*(uint32_t *)addr != addr)
		{
			break;
		}
	}
			
	if( addr >= SRAM_BLANK1_END_ADDR + 1 )
	{
	    printf("SRAM BLANK1测试 OK....\r\n");
	}
	else
	{
		printf("SRAM BLANK1 error. Address = 0x%08X, Read = 0x%08X, Expected = 0x%08X \r\n", addr, *(uint32_t *)addr, addr);
	}

	printf("SRAM BLANK2测试....\r\n");

	for( addr = SRAM_BLANK2_START_ADDR; addr < SRAM_BLANK2_END_ADDR + 1 ; addr += 4 )
	{
	    *(uint32_t *)addr = addr;
	}
	
	for( addr = SRAM_BLANK2_START_ADDR; addr < SRAM_BLANK2_END_ADDR + 1 ; addr += 4 )
	{
		if (*(uint32_t *)addr != addr)
		{
			break;
		}
	}
			
	if( addr >= SRAM_BLANK2_END_ADDR + 1 )
	{
	    printf("SRAM BLANK2测试 OK....\r\n");
	}
	else
	{
		printf("SRAM BLANK2 error. Address = 0x%08X, Read = 0x%08X, Expected = 0x%08X \r\n", addr, *(uint32_t *)addr, addr);
	}

}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
