/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               Host_SL811.h
** Descriptions:            The USB Host macro
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2011-3-7
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

#ifndef __HOST_SL811_H
#define __HOST_SL811_H
		   
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Private define ------------------------------------------------------------*/
#define NULL		0
#define TRUE   		SET
#define FALSE  		RESET
#define ISO_BIT     0x10
#define MAX_DEV    	0x06     /* maximum number of devices (Address #0 + Slave + 4 Hub's device) */
#define MAX_EP    	0x05     /* maximum number of endpoints (Ep0 + max of 4 data endpoints) */

/* Constant Defines	*/
#define	ACTIVE_BLINK		0x40	/* Port B bit 6 - Active LED, blink when no device attach */
#define	IRQ_INPUT			0x10	/* Port B bit 4 - Active high interrupt input from SL811HST */
#define	nHOST_SLAVE_MODE	0x02	/* Port B bit 1 - f/w control of nHost/Slave pin */
#define	nRESET				0x01	/* Port B bit 0 - f/w control of nReset pin */

#define HUB_ADDR			0x01	/* fix hub address */
#define TIMEOUT_RETRY		0x06	/* Maximum no. of timeout retry during USB xfer */
#define XFERSTOP0			0xDE	/* code to indicate stop button pressed */
#define XFERSTOP1			0xAF	/* "OxDEAF" */
#define NONEXIST0			0xDE	/* code to indicate device does not exist for data start/stop */
#define NONEXIST1			0xAD	/* "OxDEAD" */
#define NOT_APPL			0xFF	/* indicate not applicable, in string & class descp */

#define	PORTX_LED			0xF0	/* Turn off all Port A LEDs */
#define	PORT1_LED			0x80	/* Port A bit 7 - Port #1 */
#define	PORT2_LED			0x40	/* Port A bit 6 - Port #2 */
#define	PORT3_LED			0x20	/* Port A bit 5 - Port #3 */
#define	PORT4_LED			0x10	/* Port A bit 4 - Port #4 */

#define SL_RESET			0xD0	/* EZUSB's host command for reset */
#define SL_DEVICE_DESCP		0xD1	/* EZUSB's host command for get device descriptor */
#define SL_CONFIG_DESCP		0xD2	/* EZUSB's host command for get config descriptor */
#define SL_CLASS_DESCP		0xD3	/* EZUSB's host command for get class descriptor */
#define SL_STRING_DESCP		0xD4	/* EZUSB's host command for get string descriptor */
#define SL_REFRESH			0xD5	/* EZUSB's host command for auto device attach/detach refresh */
#define SL_DATA_XFER_START	0xD6	/* EZUSB's host command for data transfer begin */
#define SL_DATA_XFER_STOP	0xD7	/* EZUSB's host command for data transfer end */
#define SL_TOGGLE_DS_REFRESH 0xD8
#define SL_SHOW_REGS        0xD9

/* Private typedef -----------------------------------------------------------*/
typedef __packed struct
{
    uint16_t  wVID, wPID;           /* Vendor ID and Product ID */
    uint8_t  bClass;            
    uint8_t  bNumOfEPs;             /* actual number endpoint from slave */
    uint8_t  iMfg;				    /* Manufacturer ID */
    uint8_t  iPdt;				    /* Product ID */
    uint8_t  bId1;
    uint8_t  bId2;
    uint8_t  bEPAddr[MAX_EP];       /* bit 7 = 1 = use PID_IN */ 
    uint8_t  bAttr[MAX_EP];         /* ISO | Bulk | Interrupt | Control */
    uint16_t  wPayLoad[MAX_EP];     /* ISO range: 1-1023, Bulk: 1-64, etc */
	uint16_t  bInterval[MAX_EP];    /* polling interval (for LS) */
    uint8_t  bData1[MAX_EP];        /* DataToggle value */
} pUSBDEV, *PUSBDEV;

typedef __packed struct				/* USB Address #0(enum), #1(Hub), #2..#5(device behind hub) */
{								    /* [X] = device address, valid #2..#5 only, #0 is used during enum */
	uint8_t  bPortPresent[MAX_DEV];	/* '1' present, '0' absent */
	uint8_t  bPortNumber[MAX_DEV];	/* contain port number at which device is attached */
	uint8_t  bPortSpeed[MAX_DEV]; 	/* '1' LowSpeed, '0' FullSpeed */
} pHUBDEV, *PHUBDEV;

/* USB specific request */
typedef __packed struct
{
    uint8_t bmRequest;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} SetupPKG, *pSetupPKG;

/* Standard Device Descriptor */
typedef __packed struct
{   
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;
    uint8_t bNumConfigurations;
} sDevDesc, *pDevDesc;

/* Standard Configuration Descriptor */
typedef __packed struct
{	
    uint8_t bLength;                 /* Size of descriptor in byte */
	uint8_t bType;					 /* Configuration */
	uint16_t wLength;                /* Total length */
	uint8_t bNumIntf;				 /* Number of interface */
	uint8_t bCV;             		 /* bConfigurationValue */
	uint8_t bIndex;          		 /* iConfiguration */
	uint8_t bAttr;                   /* Configuration Characteristic */
	uint8_t bMaxPower;				 /* Power config */
} sCfgDesc, *pCfgDesc;

/* Standard Interface Descriptor */
typedef __packed struct
{	
    uint8_t bLength;
	uint8_t bType;
	uint8_t iNum;
	uint8_t iAltString;
	uint8_t bEndPoints;
	uint8_t iClass;
	uint8_t iSub; 
	uint8_t iProto;
	uint8_t iIndex; 
} sIntfDesc, *pIntfDesc;

/* Standard EndPoint Descriptor */
typedef __packed struct
{	
    uint8_t bLength;
	uint8_t bType;
	uint8_t bEPAdd;
	uint8_t bAttr;
	uint16_t wPayLoad;               /* low-speed this must be 0x08 */
	uint8_t bInterval;
} sEPDesc, *pEPDesc;

/* Standard String Descriptor */
typedef __packed struct
{	
    uint8_t bLength;
	uint8_t bType;
	uint16_t wLang;
} sStrDesc, *pStrDesc;


/* Structure for USB HUB */

/* Hub Standard Descriptor */
typedef __packed struct
{
    uint8_t bLength;
    uint8_t DType;
    uint8_t bNbrPort;               /* Number of Downstream ports */
    uint16_t wHubCharacteristic;    /* See chapter 11 */
    uint8_t bbPwrOn2PwrGood;        /* Time in 2ms interval */
    uint8_t bHubContrCurrent;       /* Max current in mA */
    uint8_t bDeviceRemovable;       /* Idicate if a port has a removable */
    uint8_t bPortPwrCtlMask;        
} sHubDesc, *pHubDesc;

typedef __packed struct 
{
    uint16_t wHubStatus;
    uint16_t wHubChange;
} sHubStatus, *pHubStatus;

typedef __packed struct 
{
    uint8_t wPortStatus_Lo;
    uint8_t wPortStatus_Hi;
    uint8_t wPortChange_Lo;	
    uint8_t wPortChange_Hi;	
} sPortStatus, *pPortStatus;

/* Structure for USB HID */
/* HID Standard Descriptor */
typedef __packed struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdHID;
    uint8_t bCountryCode;
    uint8_t bNumDescriptors;
    uint8_t bDescriptorType2;
    uint8_t wItemLength;
} sHidDesc, *pHidDesc;

/* EP0 use for configuration and Vendor Specific command interface */

#define EP0_Buf		    0x10	/* define start of EP0 64-uint8_t buffer */
#define EP1_Buf		    0x40	/* define start of EP1 64-uint8_t buffer */

/*-------------------------------------------------------------------------
 * SL811H Register Control memory map
 * --Note: 
 *      --SL11H only has one control register set from 0x00-0x04
 *      --SL811H has two control register set from 0x00-0x04 and 0x08-0x0c
 *------------------------------------------------------------------------*/

#define EP0Control      0x00
#define EP0Address      0x01
#define EP0XferLen      0x02
#define EP0Status       0x03
#define EP0Counter      0x04

#define EP1Control      0x08
#define EP1Address      0x09
#define EP1XferLen      0x0a
#define EP1Status       0x0b
#define EP1Counter      0x0c

#define CtrlReg         0x05
#define IntEna          0x06
                               /* 0x07 is reserved */
#define IntStatus       0x0d
#define cDATASet        0x0e
#define cSOFcnt         0x0f   /* Master=1 Slave=0, D+/D-Pol Swap=1 0=not [0-5] SOF Count */ 
                               /* 0xAE = 1100 1110 */
                               /* 0xEE = 1110 1110 */

#define IntMask         0x57   /* Reset|DMA|EP0|EP2|EP1 for IntEna */
#define HostMask        0x47   /* Host request command  for IntStatus */
#define ReadMask        0xd7   /* Read mask interrupt   for IntStatus */

/* Interrupt Status Mask */
#define USB_A_DONE		0x01
#define USB_B_DONE		0x02
#define BABBLE_DETECT	0x04
#define INT_RESERVE		0x08
#define SOF_TIMER		0x10
#define INSERT_REMOVE	0x20
#define USB_RESET		0x40
#define USB_DPLUS		0x80
#define INT_CLEAR		0xFF

/* EP0 Status Mask */
#define EP0_ACK			0x01	/* EPxStatus bits mask during a read */
#define EP0_ERROR		0x02
#define EP0_TIMEOUT		0x04
#define EP0_SEQUENCE	0x08
#define EP0_SETUP		0x10
#define EP0_OVERFLOW	0x20
#define EP0_NAK			0x40
#define EP0_STALL		0x80

/* Standard Chapter 9 definition */
#define GET_STATUS      0x00																  
#define CLEAR_FEATURE   0x01
#define SET_FEATURE     0x03
#define SET_ADDRESS     0x05
#define GET_DESCRIPTOR  0x06
#define SET_DESCRIPTOR  0x07
#define GET_CONFIG      0x08
#define SET_CONFIG      0x09
#define GET_INTERFACE   0x0a
#define SET_INTERFACE   0x0b
#define SYNCH_FRAME     0x0c

#define DEVICE          0x01
#define CONFIGURATION   0x02
#define STRING          0x03
#define INTERFACE       0x04
#define ENDPOINT        0x05

#define STDCLASS        0x00
#define HIDCLASS        0x03
#define HUBCLASS	 	0x09      /* bDeviceClass, bInterfaceClass */

/* SL11H/SL811H definition */

/* USB-A, USB-B Host Control Register [00H, 08H] */
/* Pre  Reserved */
/* DatT Dir [1=Trans, 0=Recv] */
/* SOF Enable */
/* ISO  Arm */ 
#define DATA0_WR   	0x07   /* 0000 0111 (      Data0 +       OUT + Enable + Arm) */
#define sDATA0_WR  	0x27   /* 0010 0111 (      Data0 + SOF + OUT + Enable + Arm) */
#define pDATA0_WR   0x87   /* 1000 0111 (Pre + Data0 +       OUT + Enable + Arm) */
#define psDATA0_WR  0xA7   /* 1010 0111 (Pre + Data0 + SOF + OUT + Enable + Arm) */

#define DATA0_RD    0x03   /* 0000 0011 (      Data0 +       IN +  Enable + Arm) */
#define sDATA0_RD   0x23   /* 0010 0011 (      Data0 + SOF + IN +  Enable + Arm) */
#define pDATA0_RD   0x83   /* 1000 0011 (Pre + Data0 +       IN +  Enable + Arm) */
#define psDATA0_RD  0xA3   /* 1010 0011 (Pre + Data0 + SOF + IN +  Enable + Arm) */

#define PID_SETUP   0xD0 
#define PID_IN      0x90
#define PID_OUT     0x10

/* Class Descriptor for HID (chap.7) : These values from bRequest */
/* HID Class-Specific Request */
#define GET_REPORT      0x01
#define GET_IDLE        0x02
#define GET_PROTOCOL    0x03
						      /*  0x04-0x08 is reserved */
#define SET_REPORT      0x09
#define SET_IDLE        0x0A
#define SET_PROTOCOL    0x0B
                              /* 0x24-0x2F is reserved */
#define HID_DEV         0x21
#define HID_REPORT      0x22
#define HID_PHYSICAL    0x23
                              /* 0x24-0x2F is reserved */

/* Report Type */ 
#define INPUT         0x01
#define OUPUT         0x02
#define FEATURE       0x03

/* Protocol Codes in the bInterfaceProtocol */
#define NONE	        0
#define KEYBOARD        1
#define MOUSE	        2
#define OTHER	        3     /* 3-255 is reserved */

/* SubClass Codes in the bInterfaceSubClass */
#define sCLASS          1
#define sOTHER          2

/* HUB CLASS */

/* Hub Status & Hub Change bit masks */
#define HUB_STATUS_LOCAL_POWER	0x0001
#define HUB_STATUS_OVERCURRENT	0x0002
#define HUB_CHANGE_LOCAL_POWER	0x0001
#define HUB_CHANGE_OVERCURRENT	0x0002

/* wPortStatus bits */
#define USB_PORT_STAT_CONNECTION    0x0001
#define USB_PORT_STAT_ENABLE        0x0002
#define USB_PORT_STAT_SUSPEND       0x0004
#define USB_PORT_STAT_OVERCURRENT   0x0008
#define USB_PORT_STAT_RESET         0x0010
#define USB_PORT_STAT_POWER         0x0100
#define USB_PORT_STAT_LOW_SPEED     0x0200

/* wPortChange bits */
#define USB_PORT_STAT_C_CONNECTION  0x0001
#define USB_PORT_STAT_C_ENABLE      0x0002
#define USB_PORT_STAT_C_SUSPEND     0x0004
#define USB_PORT_STAT_C_OVERCURRENT 0x0008
#define USB_PORT_STAT_C_RESET       0x0010

/* Standard Chapter 11 definition */
#define	USB_HUB_NOT_USED       0
#define	USB_HUB_USED           1
#define	USB_HUB_IN_SHUTDOWN    2

/* HUB Class Feature Selector */
#define HUB_LOCAL_POWER    0x00
#define HUB_OVER_POWER     0x01

/* Port Status Field, wPortStatus */
#define PORT_CONNECTION    0x00
#define PORT_ENABLE        0x01
#define PORT_SUSPEND       0x02
#define PORT_OVER_CURRENT  0x03
#define PORT_RESET         0x04
#define PORT_POWER         0x08
#define PORT_LOW_SPEED     0x09

/* Port Change Field, wPortChange */
#define C_PORT_CONNECTION   0x10
#define C_PORT_ENABLE       0x11
#define C_PORT_SUSPEND      0x12
#define C_PORT_OVER_CURRENT 0x13
#define C_PORT_RESET        0x14

/* HUB Characteristic */
#define	HUB_GANG_POWER			 0
#define	HUB_INDIVIDUAL_POWER	 1
#define HUB_NO_POWER_SWITCH	 	 2
#define	HUB_POWER_MASK			 3

/* HUB Port Status */ 
#define HUB_STATUS_PORT_CONNECTION	   1
#define HUB_STATUS_PORT_ENABLE     	   2
#define HUB_STATUS_PORT_SUSPEND    	   4
#define HUB_STATUS_PORT_OVER_CURRENT   8
#define HUB_STATUS_PORT_RESET          0x10
#define HUB_STATUS_PORT_POWER     	   0x100
#define HUB_STATUS_PORT_LOW_SPEED  	   0x200

#define HUB_CHANGE_PORT_CONNECTION	   0x010000
#define HUB_CHANGE_PORT_ENABLE     	   0x020000
#define HUB_CHANGE_PORT_SUSPEND    	   0x040000
#define HUB_CHANGE_PORT_OVER_CURRENT   0x080000
#define HUB_CHANGE_PORT_RESET          0x100000

/* Other various equivalences */
#define	PORT_SPEED_SHIFT         9
#define	HUB_WAIT_PORT_RESET      25
#define RETRY_PORT_RESET         10

/* Possiblities Hub Error */
#define 	ERROR_HUB_SET_POWER				0x8500

#endif 

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
