/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               SL811.h
** Descriptions:            The SL811 application function
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

#ifndef __SL811_H
#define __SL811_H

/* Includes ------------------------------------------------------------------*/
#include "Host_SL811.h"
#include "stm32f10x.h"
#include <stdio.h>

/* Private variables ---------------------------------------------------------*/
extern uint8_t 		    DBUF[256];		/* at 0x2000 for general descriptors data */
extern uint8_t 	  	    STATUS[8];		/* for status data buffer */
extern uint8_t		    REGBUFF[16];    /* Buffer for Register Data */
extern uint8_t 		    HOSTCMD[8];		/* EZUSB's OUT1 host command data */
extern uint8_t 		    pHOSTCMD[8];	/* previous data transfer info, during data transfer */
extern uint8_t 		    HubChange[1];	/* Hub port endpoint 1 data status */
extern uint8_t 		    DataBufLen;		/* EZUSB's IN #3 data transfer buffer length */
extern uint8_t 		    pNumPort;		/* Number of downstream ports on hub */
extern uint8_t		    remaind;		/* Remaining byte in a USB transfer */

extern pUSBDEV  		uDev[MAX_DEV];	/* Multiple USB devices attributes, Max 5 devices */
extern pHUBDEV			uHub;			/* Struct for downstream device on HUB */
extern pDevDesc  		pDev;			/* Device descriptor struct */
extern pCfgDesc 		pCfg;			/* Configuration descriptor struct */
extern pIntfDesc 		pIfc;			/* Interface descriptor struct */
extern pEPDesc 		    pEnp;			/* Endpoint descriptor struct */
extern pStrDesc 		pStr;			/* String descriptor struct */
extern pHidDesc 		pHid;			/* HID class descriptor struct */
extern pHubDesc 		pHub;			/* HUD class descriptor struct */
extern pPortStatus	    pStat;			/* HID ports status */

/* Boolean Logic Defines */
extern FlagStatus       SLAVE_FOUND;				/* Slave USB device found */
extern FlagStatus  	    SLAVE_ENUMERATED;			/* slave USB device enumeration done */
extern FlagStatus  	    FULL_SPEED;					/* Full-Speed = TRUE, Low-Speed = FALSE */
extern FlagStatus	    HUB_DEVICE;					/* HUB device = TRUE */
extern FlagStatus 	    BULK_OUT_DONE;				/* Set when EZUSB's OUT1 hostcmd xfer is done */
extern FlagStatus 	    DESC_XFER;					/* Set when there is data for EZUSB's IN1 desc xfer */
extern FlagStatus 	    DATA_XFER;					/* Set when there is data for EZUSB's IN3 data xfer */
extern FlagStatus 	    DATA_XFER_OUT;				/* Set when there is data for EZUSB's OUT3 data xfer */
extern FlagStatus 	    CONFIG_DONE;				/* Set when EZUSB completes its enumeration process */
extern FlagStatus 	    TIMEOUT_ERR;				/* timeout error during data endpoint transfer */
extern FlagStatus	    DATA_STOP;					/* device unplugged during data transfer */
extern FlagStatus	    DATA_INPROCESS;				/* set when we are in a data pipe transfer */
extern FlagStatus	    pLS_HUB;					/* indicate previous command is a LS device on hub */

extern FlagStatus	    dsPoll;				        /* poll downstream port conections */

extern uint8_t 	        bData1;

/* Private function prototypes -----------------------------------------------*/
void USBReset(void);
void sl811_init(void);
int speed_detect(void); 
int slave_detect(void);
int Slave_Detach(void);
void SL811_MemTest(void);
uint8_t sl811_read( void );
int SetAddress( uint16_t addr );
void sl811_write( uint8_t value);
int EnumUsbDev( uint8_t usbaddr );
uint8_t sl811_readreg( uint8_t reg );
uint16_t GetDevInfo( uint8_t *DevInfo);
int GetStatus( uint8_t usbaddr, uint8_t *desc);
void sl811_writereg( uint8_t reg , uint8_t value);
int Set_Configuration( uint8_t usbaddr, uint16_t wVal);
int GetPortStatus( uint8_t usbaddr, uint8_t cPort, uint8_t *desc);
void sl811_read_buf( uint8_t adr, uint8_t * buffer, uint8_t size);
void sl811_write_buf( uint8_t adr, uint8_t * buffer, uint8_t size);
int GetHubDesc( uint8_t usbaddr, uint16_t wValue, uint16_t wLen, uint8_t *desc);
int PortFeature( uint8_t usbaddr, uint8_t bReq, uint16_t wValue, uint8_t cPort);
int GetHid_Desc( uint8_t usbaddr, uint16_t wValue, uint16_t wLen, uint8_t *desc);
int ep0Xfer( uint8_t usbaddr, uint16_t payload, pSetupPKG setup, uint8_t *pData );
int GetDesc( uint8_t usbaddr, uint16_t wValue, uint16_t wIndex, uint16_t wLen, uint8_t *desc );
int DataRW( uint8_t usbaddr, uint8_t epaddr, uint16_t wPayload, uint16_t wLen, uint8_t *pData );
int usbXfer( uint8_t usbaddr, uint8_t endpoint, uint8_t pid, uint8_t iso, uint16_t wPayload, uint16_t wLen, uint8_t *buffer );
int VendorCmd( uint8_t usbaddr, uint8_t bReq, uint8_t bCmd, uint16_t wValue, uint16_t wIndex, uint16_t wLen, uint8_t *pData );

#endif 

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
