/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               VS1003.h
** Descriptions:            The VS1003 application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2011-2-17
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

#ifndef _VS1003_H_
#define _VS1003_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Private define ------------------------------------------------------------*/
#define SPI_SPEED_HIGH    1  
#define SPI_SPEED_LOW     0

/* 命令片选 */
#define MP3_CCS(x)	  x ? GPIO_SetBits(GPIOC,GPIO_Pin_5) : GPIO_ResetBits(GPIOC,GPIO_Pin_5)

/* 数据片选 */ 
#define MP3_DCS(x)	  x ? GPIO_SetBits(GPIOC,GPIO_Pin_6) : GPIO_ResetBits(GPIOC,GPIO_Pin_6)


#define MP3_DREQ      GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)


#define VS_WRITE_COMMAND 	0x02
#define VS_READ_COMMAND 	0x03

/* VS1003寄存器定义 */
#define SPI_MODE        	0x00   
#define SPI_STATUS      	0x01   
#define SPI_BASS        	0x02   
#define SPI_CLOCKF      	0x03   
#define SPI_DECODE_TIME 	0x04   
#define SPI_AUDATA      	0x05   
#define SPI_WRAM        	0x06   
#define SPI_WRAMADDR    	0x07   
#define SPI_HDAT0       	0x08   
#define SPI_HDAT1       	0x09 
#define SPI_AIADDR      	0x0a   
#define SPI_VOL         	0x0b   
#define SPI_AICTRL0     	0x0c   
#define SPI_AICTRL1     	0x0d   
#define SPI_AICTRL2     	0x0e   
#define SPI_AICTRL3     	0x0f   
#define SM_DIFF         	0x01   
#define SM_JUMP         	0x02   
#define SM_RESET        	0x04   
#define SM_OUTOFWAV     	0x08   
#define SM_PDOWN        	0x10   
#define SM_TESTS        	0x20   
#define SM_STREAM       	0x40   
#define SM_PLUSV        	0x80   
#define SM_DACT         	0x100   
#define SM_SDIORD       	0x200   
#define SM_SDISHARE     	0x400   
#define SM_SDINEW       	0x800   
#define SM_ADPCM        	0x1000   
#define SM_ADPCM_HP     	0x2000 	

/* Private function prototypes -----------------------------------------------*/
void VS1003_Init(void);
void VS1003_SetVol(void); 
void VS1003_Reset(void);
void VS1003_RamTest(void);
void VS1003_SineTest(void);
void VS1003_SoftReset(void); 
void VS1003_Record_Init(void);
uint16_t VS1003_GetBitrate(void);
void VS1003_ResetDecodeTime(void);
uint16_t VS1003_GetDecodeTime(void);
void VS1003_WriteData( uint8_t * buf);
uint16_t VS1003_ReadReg( uint8_t reg);
 
#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

