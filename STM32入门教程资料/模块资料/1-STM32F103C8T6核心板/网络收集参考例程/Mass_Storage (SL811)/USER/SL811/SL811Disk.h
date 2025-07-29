/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               SL811Disk.h
** Descriptions:            The SL811 Disk application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2011-4-13
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

#ifndef __SL811DISK_H
#define __SL811DISK_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Private variables ---------------------------------------------------------*/
#define	READY				0
#define	COMPLETED			0
#define	NO_DEVICE			1
#define	NOT_FULL_SPEED		2
#define	NOT_MASS_STORAGE	3
#define	ERROR				4
#define	BUSY				5
#define	STALL				6

/* Private function prototypes -----------------------------------------------*/
int sl811_set_conf(void);
int sl811_set_addr(void);
int sl811_usb_ready(void);
int sl811_disk_init(void);
void sl811_clear_feature(void);
int sl811_get_capacity( uint8_t * buf );
void sl811_get_device_desc(uint8_t * buf);
void sl811_get_device_conf( uint8_t * buf);
int sl811_read_sector( uint32_t sector, uint8_t * buf );
int sl811_write_sector( uint32_t sector, uint8_t * buf );

#endif /* __FSMC_NAND_H */

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
