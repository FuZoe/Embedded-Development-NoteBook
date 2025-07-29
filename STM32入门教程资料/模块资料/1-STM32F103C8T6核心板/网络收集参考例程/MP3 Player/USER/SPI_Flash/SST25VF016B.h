/****************************************Copyright (c)**************************************************
**
**                                 http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File Name: 			SST25VF016B.h
** Last modified Date: 	2010-10-29
** Last Version: 		1.0
** Descriptions: 		SPI flash 操作函数 
**
**------------------------------------------------------------------------------------------------------
** Created 	 By: 		AVRman
** Created date: 		2010-10-29
** Version: 1.0
** Descriptions: 		First version
**
**------------------------------------------------------------------------------------------------------
** Modified by: 	
** Modified date: 	
** Version:
** Descriptions:  	
**
********************************************************************************************************/

#ifndef __SST25VF016B_H 
#define __SST25VF016B_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "vs1003.h"

/* Private typedef -----------------------------------------------------------*/
/* 定义操作SST25VF016B所需的数据 */
typedef enum ERTYPE{Sec1,Sec8,Sec16,Chip} ErType;  
typedef enum IDTYPE{Manu_ID,Dev_ID,Jedec_ID} idtype;

/* Private define ------------------------------------------------------------*/
#define MAX_ADDR		0x1FFFFF	/* 定义芯片内部最大地址 */
#define	SEC_MAX     	511         /* 定义最大扇区号 */
#define SEC_SIZE		0x1000      /* 扇区大小	*/

/* 宏,用于定义SST25VF016B的片选引脚 */
#define RCC_APB2Periph_GPIO_SPI_FLASH_CS      RCC_APB2Periph_GPIOB
#define SPI_FALSH_CS_PORT                     GPIOB 
#define SPI_FALSH_CS_PIN                      GPIO_Pin_12 

#define SPI_FLASH_CS_LOW()       GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define SPI_FLASH_CS_HIGH()      GPIO_SetBits(GPIOB, GPIO_Pin_12)

/* Private function prototypes -----------------------------------------------*/
void  SPI_FLASH_Init(void);
void SPI_FLASH_SetSpeed( uint8_t SpeedSet);
uint8_t SSTF016B_RD(uint32_t Dst, uint8_t* RcvBufPt ,uint32_t NByte);
uint8_t SSTF016B_RdID(idtype IDType,uint32_t* RcvbufPt);
uint8_t SSTF016B_WR(uint32_t Dst, uint8_t* SndbufPt,uint32_t NByte);

uint8_t SSTF016B_Erase(uint32_t sec1, uint32_t sec2);
void SPI_FLASH_Test(void);

#endif
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

