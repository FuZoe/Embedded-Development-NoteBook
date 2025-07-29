/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               SL811mouse.c
** Descriptions:            The SL811 Mouse application function
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

/* Includes ------------------------------------------------------------------*/
#include "SL811.h"
#include "mouse_lcd.h"
#include "Host_SL811.h"
#include "SL811mouse.h"

/* Private define ------------------------------------------------------------*/
 #define SL811MOUSE_DEBUG 

/* Private variables ---------------------------------------------------------*/
struct HidMouseStruct ReceiveMouseData;

/*******************************************************************************
* Function Name  : delay_ms
* Description    : Delay Time
* Input          : - cnt: Delay Time
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void delay_ms(uint16_t ms)    
{ 
	uint16_t i,j; 
	for( i = 0; i < ms; i++ )
	{ 
		for( j = 0; j < 1141; j++ );
	}
} 

/*******************************************************************************
* Function Name  : sl811Initformouse
* Description    : initialize mouse
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int sl811Initformouse(void)
{
	volatile int i =0;	
	
	FULL_SPEED = FALSE;	
	SLAVE_FOUND = FALSE;
	SLAVE_ENUMERATED = FALSE;
	BULK_OUT_DONE = FALSE;
	DATA_INPROCESS = FALSE;
    dsPoll = TRUE;				          /* poll downstream port conections */

	sl811_writereg(CtrlReg,0x08);      	  /* Reset USB engine, full-speed setup, suspend disable */
	delay_ms(10);					      /* Delay for HW stablize */
	sl811_writereg(CtrlReg,0x00);      	  /* Set to normal operation */

	sl811_writereg(IntEna,0x00);
	sl811_writereg(IntStatus,INT_CLEAR);  /* Clear Interrupt enable status */
	delay_ms(10);					      /* Delay for HW stablize */

	pNumPort    = 0;			          /* zero no. of downstream ports */
	SLAVE_FOUND = FALSE;		          /* Clear USB device found flag */
	FULL_SPEED  = TRUE;			          /* Assume full speed device */
	DATA_STOP   = FALSE;	
 							
	i = sl811_readreg(IntStatus);		  /* Read Interrupt Status */	
	if( (i & USB_RESET) == 0 )
	{
		if( (i & USB_DPLUS) == 0 )	      /* Checking full or low speed */
		{									
		    /* Low Speed is detected */
#ifdef SL811MOUSE_DEBUG
			printf("'Low' Speed is detected!! \nRegister for status=0x%x \r\n",i);
#endif

			/* Set up Master and low Speed direct and SOF cnt high = 0x2e */
			sl811_writereg(cSOFcnt,0xee);   	/* Set up host and low speed direct and SOF cnt */
			/* SOF Coonuter Low = 0xe0; 1ms interval */
			sl811_writereg(cDATASet,0xe0);  	/* SOF Counter Low = 0xE0; 1ms interval */
			/* Setup 6Mhz and EOP enable */
			sl811_writereg(CtrlReg,0x21);  	    /* Setup 6MHz and EOP enable */        
			FULL_SPEED = FALSE;		        /* low speed device flag */
		}
		else	
		{	

			/* Full Speed is detected */
#ifdef SL811MOUSE_DEBUG
			printf("'Full' Speed is detected!! \nRegister for status=0x%x \r\n",i);
#endif
								
			sl811_writereg(cSOFcnt,0xae);   	/* Set up Master & low speed direct and SOF cnt high=0x2e */
			sl811_writereg(cDATASet,0xe0);  	/* SOF Counter Low = 0xE0; 1ms interval */
			sl811_writereg(CtrlReg,0x05);   	/* Setup 48MHz and SOF enable */
			return 0;
		}
	}
	else
	{
		return 0;
	}
			
	SLAVE_FOUND = TRUE;			                /* Set USB device found flag */
	SLAVE_ENUMERATED = FALSE;		            /* no slave device enumeration */

	sl811_writereg(EP0Status,0x90);   		    /* Setup SOF Token, EP0 */
	sl811_writereg(EP0Counter,0x00);		    /* reset to zero count */
	sl811_writereg(EP0Control,sDATA0_RD);   	/* start generate SOF or EOP */
	delay_ms(25);					            /* Hub required approx. 24.1mS */
	sl811_writereg(IntStatus,INT_CLEAR);	    /* Clear Interrupt status */
	
	sl811_writereg(IntEna,0x03);      		    /* USB-A, Insert/Remove, USB_Resume */
	sl811_writereg(IntStatus,INT_CLEAR);	    /* Clear Interrupt enable status */

	return 1;
}

/*******************************************************************************
* Function Name  : EnumUsbDevformouse
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int EnumUsbDevformouse(uint8_t usbaddr)
{
	uint8_t uAddr = 0;		/* for enumeration to Address #0 */

	/* Reset only Slave device attached directly */
	uDev[0].wPayLoad[0] = 64;	/* default 64-byte payload of Endpoint 0, address #0 */
	if(usbaddr == 1)			/* bus reset for the device attached to SL811HS only */
	{
		USBReset();				/* that will always have the USB address = 0x01 (for a hub) */
	}
    delay_ms(2);

	/* Get USB Device Descriptors on EP0 & Addr 0 */
	/* with default 64-byte payload */
	pDev =(pDevDesc)DBUF;							/* ask for 64 bytes on Addr #0 */
	if (!GetDesc(uAddr,DEVICE,0,18,DBUF))	        /* and determine the wPayload size */
	{
		return FALSE;								/* get correct wPayload of Endpoint 0 */
	}
	uDev[usbaddr].wPayLoad[0] = pDev->bMaxPacketSize0;/* on current non-zero USB address */

	/* Set Slave USB Device Address */
#ifdef SL811MOUSE_DEBUG	
	printf("Set Slave USB Device Address \r\n");
#endif

	if ( !SetAddress(usbaddr) ) 	/* set to specific USB address */
	{
		return FALSE;		
	}
	uAddr = usbaddr;				/* ransfer using this new address */

	/* Get USB Device Descriptors on EP0 & Addr X */
	if (!GetDesc(uAddr,DEVICE,0,(pDev->bLength),DBUF)) 	
	{
		return FALSE;								/* For this current device: */
	}
	uDev[usbaddr].wVID 	 = pDev->idVendor;			/* save VID */
	uDev[usbaddr].wPID 	 = pDev->idProduct;			/* save PID */
	uDev[usbaddr].iMfg 	 = pDev->iManufacturer;		/* save Mfg Index */
	uDev[usbaddr].iPdt 	 = pDev->iProduct;			/* save Product Index */

#ifdef SL811MOUSE_DEBUG	
	printf("VID 0x%x, PID 0x%x \r\n", uDev[usbaddr].wVID, uDev[usbaddr].wPID);
#endif

	/* Get Slave USB Configuration Descriptors */
#ifdef SL811MOUSE_DEBUG	
	printf("Get Slave USB Configuration Descriptors \r\n");
#endif


	pCfg = (pCfgDesc)DBUF;									
	if (!GetDesc(uAddr,CONFIGURATION,0,8,DBUF)) 		
	{
		return FALSE;
	}										
	if (!GetDesc(uAddr,CONFIGURATION,0,pCfg->wLength,DBUF))
	{
		return FALSE;	
	}	

	pIfc = (pIntfDesc)(DBUF + 9);					/* point to Interface Descp */
	uDev[usbaddr].bClass 	= pIfc->iClass;			/* update to class type */
	uDev[usbaddr].bNumOfEPs = (pIfc->bEndPoints <= MAX_EP) ? pIfc->bEndPoints : MAX_EP;

	/* Set configuration (except for HUB device) */
#ifdef SL811MOUSE_DEBUG	
	printf("Set_Configuration end \r\n");
#endif

	return 1;
}

/*******************************************************************************
* Function Name  : MouseInit
* Description    : USB mouse initialize 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int MouseInit(void)
{
	if( ( sl811Initformouse() == 1 ) && ( EnumUsbDevformouse(2) == 1 ) )
	{
		VendorCmd(2,0x00,0x09,0x0100,0,0,DBUF);	
		VendorCmd(2,0x21,0x0a,0,0,0,DBUF);
		VendorCmd(2,0xa1,0x01,0x01,0,4,DBUF);	
	}
	else
	{
		return 0;
	}
	return 1;
}

/*******************************************************************************
* Function Name  : MouseTask
* Description    : USB mouse task 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void MouseTask(void)
{
	sl811_init();
	
	while(1)
	{
		if( !Slave_Detach() )
		{
			if( !SLAVE_FOUND )	
			{
				if( !MouseInit() )
				{
					SLAVE_FOUND = FALSE;
					SLAVE_ENUMERATED = FALSE;
					continue;
				}
			}
			DataRW(2, 0x81, 0x40, 4, DBUF);	
			if( DATA_INPROCESS )
			{
				if( DBUF[0] & 0x01 )
				{
					ReceiveMouseData.button_L = 1;
				}
				else
				{
					ReceiveMouseData.button_L = 0;
				}
				
				if( DBUF[0] & 0x02 )
				{
					ReceiveMouseData.button_R = 1;
				}
				else
				{
					ReceiveMouseData.button_R = 0;
				}
				
				if( DBUF[0] & 0x04 )
				{
					ReceiveMouseData.button_M = 1;
				}
				else
				{
					ReceiveMouseData.button_M = 0;
				}
				
				if( DBUF[1] & 0x80 )			
				{
					ReceiveMouseData.x = DBUF[1] - 256;	
				}			
				else
				{
					ReceiveMouseData.x = DBUF[1];	
				}
				
				if( DBUF[2] & 0x80 )			
				{
					ReceiveMouseData.y = DBUF[2] - 256;	
				}			
				else
				{
					ReceiveMouseData.y = DBUF[2];
				}
				
				ReceiveMouseData.wheel = DBUF[3];
				
				USR_MOUSE_ProcessData(&ReceiveMouseData);
	
#ifdef SL811MOUSE_DEBUG			
				printf("L:%d, R:%d, M:%d \r\n",ReceiveMouseData.button_L,ReceiveMouseData.button_R,ReceiveMouseData.button_M);		
				printf("Data:%d, %d, %d \r\n",ReceiveMouseData.x,ReceiveMouseData.y,ReceiveMouseData.wheel);
#endif
			}
			delay_ms(25);
		}
		else
		{
#ifdef SL811MOUSE_DEBUG			
			printf("No mouse! \r\n");
#endif 
			sl811_init();	
			delay_ms(500);
		}
	delay_ms(25);
	}
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
