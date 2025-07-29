/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               SL811.c
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

/* Includes ------------------------------------------------------------------*/
#include "SL811.h"
#include "Host_SL811.h"

/* Private define ------------------------------------------------------------*/
#define USB_HOST_INDEX_ADDR     (*((volatile unsigned short *)0x6C800000)) 
#define USB_HOST_DATA_ADDR      (*((volatile unsigned short *)0x6C800002)) 

/* #define USB_DEBUG */	

/* Private variables ---------------------------------------------------------*/
uint8_t 		DBUF[256];		/* at 0x2000 for general descriptors data */
uint8_t 		STATUS[8];		/* for status data buffer */
uint8_t			REGBUFF[16];    /* Buffer for Register Data */
uint8_t 		HOSTCMD[8];		/* EZUSB's OUT1 host command data */
uint8_t 		pHOSTCMD[8];	/* previous data transfer info, during data transfer */
uint8_t 		HubChange[1];	/* Hub port endpoint 1 data status */
uint8_t 		DataBufLen;		/* EZUSB's IN #3 data transfer buffer length */
uint8_t 		pNumPort;		/* Number of downstream ports on hub */
uint8_t			remaind;		/* Remaining byte in a USB transfer */

pUSBDEV  		uDev[MAX_DEV];	/* Multiple USB devices attributes, Max 5 devices */
pHUBDEV			uHub;			/* Struct for downstream device on HUB */
pDevDesc  		pDev;			/* Device descriptor struct */
pCfgDesc 		pCfg;			/* Configuration descriptor struct */
pIntfDesc 		pIfc;			/* Interface descriptor struct */
pEPDesc 		pEnp;			/* Endpoint descriptor struct */
pStrDesc 		pStr;			/* String descriptor struct */
pHidDesc 		pHid;			/* HID class descriptor struct */
pHubDesc 		pHub;			/* HUD class descriptor struct */
pPortStatus	    pStat;			/* HID ports status */

/* Boolean Logic Defines */
FlagStatus      SLAVE_FOUND;				/* Slave USB device found */
FlagStatus  	SLAVE_ENUMERATED;			/* slave USB device enumeration done */
FlagStatus  	FULL_SPEED;					/* Full-Speed = TRUE, Low-Speed = FALSE */
FlagStatus	    HUB_DEVICE;					/* HUB device = TRUE */
FlagStatus 	    BULK_OUT_DONE;				/* Set when EZUSB's OUT1 hostcmd xfer is done */
FlagStatus 	    DESC_XFER;					/* Set when there is data for EZUSB's IN1 desc xfer */
FlagStatus 	    DATA_XFER;					/* Set when there is data for EZUSB's IN3 data xfer */
FlagStatus 	    DATA_XFER_OUT;				/* Set when there is data for EZUSB's OUT3 data xfer */
FlagStatus 	    CONFIG_DONE;				/* Set when EZUSB completes its enumeration process */
FlagStatus 	    TIMEOUT_ERR;				/* timeout error during data endpoint transfer */
FlagStatus	    DATA_STOP;					/* device unplugged during data transfer */
FlagStatus	    DATA_INPROCESS;				/* set when we are in a data pipe transfer */
FlagStatus	    pLS_HUB;					/* indicate previous command is a LS device on hub */

FlagStatus	    dsPoll;				        /* poll downstream port conections */

uint8_t 	    bData1;

/*******************************************************************************
* Function Name  : WordSwap
* Description    : Swap high and low byte
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
__inline uint16_t WordSwap( uint16_t input )
{
	return ( ( ( input & 0x00FF ) << 8 ) | ( ( input & 0xFF00 ) >> 8 ) );
}

/*******************************************************************************
* Function Name  : sl811_writereg
* Description    : Write one byte in sl811 register
* Input          : - reg: address of the selected register.
*                  - value: value to write to the selected register.
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
__inline void sl811_writereg( uint8_t reg , uint8_t value)
{
	USB_HOST_INDEX_ADDR = reg;
	USB_HOST_DATA_ADDR = value;		
}

/*******************************************************************************
* Function Name  : sl811_write
* Description    : Write one byte in sl811 register
* Input          : - value: value to write to the selected register.
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void sl811_write( uint8_t value)
{
	USB_HOST_DATA_ADDR = value;		
}


/*******************************************************************************
* Function Name  : sl811_readreg
* Description    : Read one byte in sl811 register
* Input          : None
* Output         : None
* Return         : sl811 Register Value.
* Attention		 : None
*******************************************************************************/
__inline uint8_t sl811_readreg( uint8_t reg )
{
	USB_HOST_INDEX_ADDR = reg;
	return (uint8_t)USB_HOST_DATA_ADDR;
}

/*******************************************************************************
* Function Name  : sl811_readreg
* Description    : Read one byte in sl811 register
* Input          : None
* Output         : None
* Return         : sl811 Register Value.
* Attention		 : None
*******************************************************************************/
uint8_t sl811_read( void )
{
	return (uint8_t)USB_HOST_DATA_ADDR;
}

/*******************************************************************************
* Function Name  : sl811_write_buf
* Description    : Write buffer 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void sl811_write_buf( uint8_t adr, uint8_t * buffer, uint8_t size)
{
	USB_HOST_INDEX_ADDR = adr;
	while( size-- )
	{
		USB_HOST_DATA_ADDR = *buffer++;
	}
}

/*******************************************************************************
* Function Name  : sl811_read_buf
* Description    : Read buffer 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void sl811_read_buf( uint8_t adr, uint8_t * buffer, uint8_t size)
{
	USB_HOST_INDEX_ADDR = adr;
	while( size-- )
	{
		*buffer++ = USB_HOST_DATA_ADDR;
	}
}

/*******************************************************************************
* Function Name  : FSMC_Configuration
* Description    : Configures the Parallel interface (FSMC)
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void FSMC_Configuration(void)
{
	FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef FSMC_NORSRAMTimingInitStructure;	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable FSMC, GPIOD, GPIOE, GPIOF, GPIOG and AFIO clocks */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE |
	                       RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG |
	                       RCC_APB2Periph_AFIO, ENABLE);
	
	/* PD.14(D0) , PD.15(D1) , PD.00(D2) , PD.01(D4) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	/* PE.07(D4) , PE.08(D5) , PE.09(D6) , PE.10(D7) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	/* Set PE.05(A21), PE.06(A22) as alternate function push pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	/* Set PF.00(A0) as alternate function push pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	/* NOE and NWE configuration */  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
  	
	/* Set PG.10(NE3) PG.12(NE4) as alternate function push pull - CE3(LCD /CS) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_12;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	/* FSMC set read time */
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressSetupTime = 10;             /* 地址建立时间  */
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressHoldTime = 0;	   
	FSMC_NORSRAMTimingInitStructure.FSMC_DataSetupTime = 10;	            /* 数据建立时间  */
	FSMC_NORSRAMTimingInitStructure.FSMC_BusTurnAroundDuration = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_CLKDivision = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_DataLatency = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;	/* FSMC 访问模式 */
	
	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
	/* FSMC set write time */
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressSetupTime = 5;              /* 地址建立时间  */
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressHoldTime = 0;	   
	FSMC_NORSRAMTimingInitStructure.FSMC_DataSetupTime = 5;	                /* 数据建立时间  */
	FSMC_NORSRAMTimingInitStructure.FSMC_BusTurnAroundDuration = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_CLKDivision = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_DataLatency = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;	/* FSMC 访问模式 */
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;	  
	
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
	
	/* Enable FSMC Bank4_SRAM Bank */
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  
}

/*******************************************************************************
* Function Name  : SL811_MemTest
* Description    : SL811 test
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void SL811_MemTest(void)
{
	uint8_t value,temp;

	/* 0x10-0xFF 存储缓冲器 */
	
	for( value = 0x10 ; value < 0xff; value++ )  
	{
		sl811_writereg( value , value );
	}
	
	for( value = 0x10 ; value < 0xff; value++ )  
	{
		if( value != sl811_readreg( value ) )
		{
#ifdef USB_DEBUG
			printf("SL811 is error \r\n");
#endif
			return;
		}
	}
	
	for( value = 0x10 ; value < 0xff; value++ )  
	{
		sl811_writereg( value , ~value ); 
	}
	
	for( value = 0x10 ; value < 0xff; value++ )  
	{
		temp = ~value;
		if(  temp != sl811_readreg( value ) )
		{
#ifdef USB_DEBUG
			printf("SL811 is error \r\n");
#endif
			return;
		}
	}
#ifdef USB_DEBUG
	printf("SL811 is OK \r\n"); 
#endif

}

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
* Function Name  : USBReset
* Description    : UsbReset during enumeration of device attached directly to SL811HS
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void USBReset(void)
{
	uint8_t temp;
	
	temp =  sl811_readreg( CtrlReg );   
	
	sl811_writereg( CtrlReg , temp | 0x08 );
	delay_ms(10);			
	sl811_writereg( CtrlReg , temp );
}

/*******************************************************************************
* Function Name  : sl811_init
* Description    : SL811 variables initialization
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void sl811_init(void)
{   
    FSMC_Configuration();

    SL811_MemTest();	
	
    sl811_writereg(cSOFcnt,0xae);
    pNumPort = 0x00;	
	
    FULL_SPEED = FALSE;	
    SLAVE_FOUND = FALSE;
    SLAVE_ENUMERATED = FALSE;
    BULK_OUT_DONE = FALSE;
    DATA_INPROCESS = FALSE;
    dsPoll = TRUE;				          /* poll downstream port conections */

    sl811_writereg(cSOFcnt,0xae);
    sl811_writereg(CtrlReg,0x08);         /* Reset USB engine, full-speed setup, suspend disable */
    delay_ms(10);					      /* Delay for HW stablize */
    sl811_writereg(CtrlReg,0x00);         /* Set to normal operation */
    sl811_writereg(IntEna,0x61);          /* USB-A, Insert/Remove, USB_Resume */
    sl811_writereg(IntStatus,INT_CLEAR);  /* Clear Interrupt enable status */
    delay_ms(100);					      /* Delay for HW stablize */	
}


/*******************************************************************************
* Function Name  : usbXfer
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : The CRC contains Device 4-bit of EndPoint Number | 7-bit of Device Addr
*                  iso = 1 transfer with ISO mode, else Bulk /Interrupt/Control
*                  return 0 on Success
*                  successful transfer = return TRUE
*                  fail transfer = return FALSE
*                  USB Status 
*                  0x01  ACK
*                  0x02  Device Error Bit 
*                  0x04  Device Time out 
*                  0x08  Toggle bit 
*                  0x10  SET_UP packet bit (Not used in SL811H)
*                  0x20  Overflow bit 
*                  0x40  Device return NAKs, forever
*                  0x80  Device return STALL
*                  need to keep track DATA0 and DATA1 for write endpoint 
*******************************************************************************/
int usbXfer( uint8_t usbaddr, uint8_t endpoint, uint8_t pid, uint8_t iso, uint16_t wPayload, uint16_t wLen, uint8_t *buffer )
{  
	uint8_t  cmd, result, timeout, intr=0x0;
	uint8_t	 xferLen, bufLen, data0, data1, dataX, addr;

    /* Default setting for usb trasnfer */
	bufLen = dataX = timeout = 0;					/* reset all */
	data0 = EP0_Buf;								/* DATA0 buffer address */
	data1 = data0 + (uint8_t)wPayload;					/* DATA1 buffer address */
	DATA_STOP =	TIMEOUT_ERR = FALSE;				/* set default conditions */
	DATA_INPROCESS = FALSE;

	/* Define data transfer payload */
	if (wLen >= wPayload)  							/* select proper data payload */
	{
		xferLen = wPayload;							/* limit to wPayload size */ 
	}						
	else
	{										
		xferLen = wLen;								/* else take < payload len */
	}					

	/* For IN token */

	if( pid == PID_IN )								/* for current IN tokens */
	{							
		if(FULL_SPEED)								/* FS/FS on Hub, sync to sof */
		{											
			cmd = sDATA0_RD;	
		}					
		else										/* LS, no sync to sof for IN */
		{
			cmd = DATA0_RD;	
		}				
	}

	/* For OUT token */
	else if( pid == PID_OUT )						/* for OUT tokens */
	{  	
		if(xferLen)									/* only when there are */	
		{
			sl811_write_buf(data0,buffer,xferLen); 	/* data to transfer on USB */
		}
		if(FULL_SPEED)								
		{
			cmd = sDATA0_WR;						/* FS/FS on Hub, sync to sof */
		}
		else										/* LS, no sync to sof for OUT */
		{
			cmd = DATA0_WR;							
		}

		/* implement data toggle */
		bData1 = uDev[usbaddr].bData1[endpoint];
        uDev[usbaddr].bData1[endpoint] = ( uDev[usbaddr].bData1[endpoint] ? 0 : 1 );  /* DataToggle */
		if(bData1)
        {
		    cmd |= 0x40;                              /* Set Data1 bit in command */
		}
	}

	/* For SETUP/OUT token */
	else											/* for current SETUP/OUT tokens */
	{  	
		if(xferLen)									/* only when there are */	
		{
			sl811_write_buf(data0,buffer,xferLen); 	/* data to transfer on USB */
		}
		if(FULL_SPEED)								
		{
			cmd = sDATA0_WR;						/* FS/FS on Hub, sync to sof */
		}
		else										/* LS, no sync to sof for OUT */
		{
			cmd = DATA0_WR;	
		}						
	}

	/* Isochronous data transfer setting */
	if (iso) 
	{
		cmd |= ISO_BIT;                     		/* if iso setup ISO mode */
	}

	/* For EP0's IN/OUT token data, start with DATA1 */
	/* Control Endpoint0's status stage */
    /* For data endpoint, IN/OUT data, start */ 
	if (endpoint == 0 && pid != PID_SETUP) 			/* for Ep0's IN/OUT token */
	{
		cmd |= 0x40; 								/* always set DATA1 */
	}

	/* Arming of USB data transfer for the first pkt */
	sl811_writereg(EP0Status,((endpoint&0x0F)|pid));	/* PID + EP address */
	sl811_writereg(EP0Counter,usbaddr);			        /* USB address */
	sl811_writereg(EP0Address,data0);			        /* buffer address, start with "data0" */
	sl811_writereg(EP0XferLen,xferLen);			        /* data transfer length */
	sl811_writereg(IntStatus,INT_CLEAR); 		        /* clear interrupt status */
	sl811_writereg(EP0Control,cmd);						/* Enable ARM and USB transfer start here */
	if( endpoint == 0 ) 
	{
#ifdef USB_DEBUG
		printf( "CMD:[%x,%x,%x,%x,%x] \r\n", ( (endpoint&0x0F) | pid ), usbaddr, data0, xferLen, cmd );
#endif
	}	

	/* Main loop for completing a wLen data trasnfer */
	
	while(1)
	{  
	    uint8_t i = 0;   
		/* Wait for done interrupt */
		while(1)												/* always ensure requested device is */
		{														    		   
			if( i++ >= 100 )
			{
			   return FALSE;
			}   
		                                                        /* inserted at all time, then you will */
			intr = sl811_readreg(IntStatus);					/* wait for interrupt to be done, and */
			/* proceed to parse result from slave */ 
			if((intr & USB_RESET) || (intr & INSERT_REMOVE))
			{													/* device */					
				DATA_STOP = TRUE;								/* if device is removed, set DATA_STOP */
				return FALSE;									/* flag true, so that main loop will */
			}													/* know this condition and exit gracefully */
			if(intr & USB_A_DONE)								
			{
				break;	
			}
			if(intr & USB_B_DONE)								
			{
				break;
			}																	
		}
		/* interrupt done */
		
		if( endpoint == 0 ) 
		{
#ifdef USB_DEBUG
	        printf("EP0 intstatus %x \r\n", intr);
#endif
		}

		sl811_writereg(IntStatus,INT_CLEAR); 						/* clear interrupt status */
		result 	  = sl811_readreg(EP0Status);						/* read EP0status register */
		remaind = sl811_readreg(EP0Counter);						/* remaind value in last pkt xfer */

		if( endpoint == 0 ) 
		{ 
#ifdef USB_DEBUG
		    printf("EP0 Status %x \r\n", result);
#endif
		}

		/* ACK */
		if ( result & EP0_ACK )									/* Transmission ACK */
		{	
			/* SETUP TOKEN */
			if(pid == PID_SETUP)								/* do nothing for SETUP/OUT token */
			{
				break;											/* exit while(1) immediately */
		    }

			/* OUT TOKEN */				
			else if(pid == PID_OUT)
			{
				break;
			}

			/* IN TOKEN	*/
			else if( pid == PID_IN )
			{													/* for IN token only */
				wLen  -= (uint16_t)xferLen;							/* update remainding wLen value */
				cmd   ^= 0x40;    								/* toggle DATA0/DATA1 */
				dataX++;										/* point to next dataX */

				/* If host requested for more data than the slave */
				/* have, and if the slave's data len is a multiple */
				/* of its endpoint payload size/last xferLen. Do */
				/* not overwrite data in previous buffer. */
				if(remaind==xferLen)							/* empty data detected */
				{
					bufLen = 0;									/* do not overwriten previous data */
				}
				else											/* reset bufLen to zero */
				{
					bufLen = xferLen;							/* update previous buffer length */
				}

				/* Arm for next data transfer when requested data */ 
				/* length have not reach zero, i.e. wLen!=0, and */
				/* last xferlen of data was completed, i.e */
				/* remaind is equal to zero, not a short pkt */
				if( !remaind && wLen )						/* remaind==0 when last xferLen */
				{												/* was all completed or wLen!=0 */
					addr    = (dataX & 1) ? data1:data0; 		/* select next address for data */
					xferLen = (uint8_t)(wLen>=wPayload) ? wPayload:wLen;	/* get data length required */
					if (FULL_SPEED)								/* sync with SOF transfer */
					{	
					    cmd |= 0x20;							/* always sync SOF when FS, regardless */
				    }

#ifdef USB_DEBUG
				    printf("endpoint=%d,usbaddr=%d,data0=%d,xferLen=%d,cmd=%d \r\n",endpoint,usbaddr,data0,xferLen,cmd);
#endif

					sl811_writereg(EP0XferLen, xferLen); 			/* select next xfer length */
					sl811_writereg(EP0Address, addr);           	/* data buffer addr */
					sl811_writereg(IntStatus,INT_CLEAR);			/* is a LS is on Hub */
					sl811_writereg(EP0Control,cmd);					/* Enable USB transfer and re-arm */
				}				

				/* Copy last IN token data pkt from prev transfer */
				/* Check if there was data available during the */
				/* last data transfer */
				if( bufLen )										
				{	
					sl811_read_buf( ((dataX&1)?data0:data1), buffer, bufLen );
					DATA_INPROCESS = TRUE;
					buffer += bufLen;								
				}

				/* Terminate on short packets, i.e. remaind!=0 */
				/* a short packet or empty data packet OR when */ 
				/* requested data len have completed, i.e.wLen=0 */
				/* For a LOWSPEED device, the 1st device descp, */
				/* wPayload is default to 64-byte, LS device will */
				/* only send back a max of 8-byte device descp, */
				/* and host detect this as a short packet, and */
				/* terminate with OUT status stage */
				if( remaind || !wLen )
				{
					break;
				}
			}							
		}
		
		/* NAK */
		if (result & EP0_NAK)									/* NAK Detected	*/
		{														
			if( endpoint == 0 )									/* on ep0 during enumeration of LS device */
			{													/* happen when slave is not fast enough, */
				sl811_writereg(IntStatus,INT_CLEAR);				/* clear interrupt status, need to */
				sl811_writereg(EP0Control,cmd);						/* re-arm and request for last cmd, IN token */
                result = 0;                                     /* respond to NAK status only */
			}
			else												/* normal data endpoint, exit now !!! , non-zero ep */
			{
				break;											/* main loop control the interval polling */
			}
		}
		
		/* TIMEOUT */
		if (result & EP0_TIMEOUT)								/* TIMEOUT Detected */
		{														
			if( endpoint == 0 )								    /* happens when hub enumeration */
			{
				if(++timeout >= TIMEOUT_RETRY)
				{	
				    timeout--;
					break;										/* exit on the timeout detected */	
				}
				sl811_writereg(IntStatus,INT_CLEAR);				/* clear interrupt status, need to */
				sl811_writereg(EP0Control,cmd);						/* re-arm and request for last cmd again */
			}
			else												
			{													/* all other data endpoint, data transfer */ 
				TIMEOUT_ERR = TRUE;								/* failed, set flag to terminate transfer */
				break;											/* happens when data transfer on a device */
			}													/* through the hub */
		}

		/* STALL */
		if (result & EP0_STALL)  								/* STALL detected */
		{
			return TRUE;										/* for unsupported request */
		}																
		/* OVEFLOW */
		if (result & EP0_OVERFLOW)  							/* OVERFLOW detected */
		{
			break;
		}
		/* ERROR*/
		if (result & EP0_ERROR)  								/* ERROR detected */
		{
			break;
		}
	}	
   
	if (result & EP0_ACK) 	                                    /* on ACK transmission */
	{
		return TRUE;		                                    /* return OK */
	}
	return FALSE;			                                    /* fail transmission */	
}

/*******************************************************************************
* Function Name  : ep0Xfer
* Description    : Control Endpoint 0's USB Data Xfer Dev contains the CRC of EP | Address
*                  ep0Xfer, endpoint 0 data transfer
* Input          : None
* Output         : None
* Return         : Return: -Fail -1    -Sucess 0   -non-zero remaind(0<wlen <setup->wlength)
* Attention		 : None
*******************************************************************************/
int ep0Xfer( uint8_t usbaddr, uint16_t payload, pSetupPKG setup, uint8_t *pData )
{
	uint8_t	pid  = PID_IN;
	uint16_t	wLen = setup->wLength;	   /* swap back for correct length */
	uint8_t	ep0 = 0;			           /* always endpoint zero */
	
	/* SETUP token with 8-byte request on endpoint 0 */
	
	if ( !usbXfer(usbaddr, ep0, PID_SETUP, 0, payload, 8, (uint8_t*)setup) ) 
	{
#ifdef USB_DEBUG
	    printf("SETUP token error \r\n");
#endif
   		return FALSE;
   	} 

	/* IN or OUT data stage on endpoint 0 */	
   	if (wLen)											/* if there are data for transfer */
	{
		if (setup->bmRequest & 0x80)					/* host-to-device : IN token */
		{
			pid  = PID_OUT;	
			if( !usbXfer(usbaddr, ep0, PID_IN, 0, payload, wLen, pData) )
			{
				return FALSE;
			}
			payload = 0;
		}
		else											/* device-to-host : OUT token */
   		{							
			if( !usbXfer(usbaddr, ep0, PID_OUT, 0, payload, wLen, pData) )
			{
				return FALSE;
			}
		}
	}

	/* Status stage IN or OUT zero-length data packet */
	if( !usbXfer(usbaddr, ep0, pid, 0, payload, 0, NULL) )
	{
		return FALSE;
	}
	return TRUE;											
}

/*******************************************************************************
* Function Name  : VendorCmd
* Description    : Control endpoint
* Input          : None
* Output         : None
* Return         : -1 on failure, 0 on success > 0 if remaind
* Attention		 : None
*******************************************************************************/
int VendorCmd( uint8_t usbaddr, uint8_t bReq, uint8_t bCmd, uint16_t wValue, uint16_t wIndex, uint16_t wLen, uint8_t *pData )
{ 
	SetupPKG setup;
	
	setup.bmRequest  = bReq;
	setup.bRequest   = bCmd;
	setup.wValue     = WordSwap(wValue);
	setup.wIndex     = wIndex;
	setup.wLength    = wLen;
	
	return ep0Xfer(usbaddr, uDev[usbaddr].wPayLoad[0], &setup, pData);
}

/*******************************************************************************
* Function Name  : SetAddress
* Description    : Set Device Address
* Input          : - addr: Device Address 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int SetAddress( uint16_t addr )
{
	return VendorCmd(0,0,SET_ADDRESS, WordSwap(addr), 0, 0, NULL);
}

/*******************************************************************************
* Function Name  : Set_Configuration
* Description    : Set Device Configuration
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int Set_Configuration( uint8_t usbaddr, uint16_t wVal)
{
	return VendorCmd(usbaddr, 0, SET_CONFIG, WordSwap(wVal), 0, 0, NULL);
}

/*******************************************************************************
* Function Name  : GetDesc
* Description    : Get Device Descriptor : Device, Configuration, String
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int GetDesc( uint8_t usbaddr, uint16_t wValue, uint16_t wIndex, uint16_t wLen, uint8_t *desc )
{ 
	return VendorCmd(usbaddr, 0x80, GET_DESCRIPTOR, wValue, wIndex, wLen, desc);
}

/*******************************************************************************
* Function Name  : GetHid_Desc
* Description    : HID Get_Desc
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int GetHid_Desc( uint8_t usbaddr, uint16_t wValue, uint16_t wLen, uint8_t *desc)
{ 
	return VendorCmd(usbaddr, 0x81, GET_DESCRIPTOR, wValue, 0, wLen, desc);
}

/*******************************************************************************
* Function Name  : GetHubDesc
* Description    : GetHUBDesc
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int GetHubDesc( uint8_t usbaddr, uint16_t wValue, uint16_t wLen, uint8_t *desc)
{ 
	return VendorCmd(usbaddr, 0xA0, GET_DESCRIPTOR, wValue, 0, wLen, desc);
}

/*******************************************************************************
* Function Name  : GetStatus
* Description    : Get Status : (HUB)
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int GetStatus( uint8_t usbaddr, uint8_t *desc)
{ 
	return VendorCmd(usbaddr, 0x80, GET_STATUS, 0, 0, 2, desc);		 
}

/*******************************************************************************
* Function Name  : PortFeature
* Description    : PortFeature : (SET_FEATURE, CLEAR_FEATURE)
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int PortFeature( uint8_t usbaddr, uint8_t bReq, uint16_t wValue, uint8_t cPort)
{ 
	return VendorCmd(usbaddr, 0x23, bReq, WordSwap(wValue), WordSwap((uint16_t)cPort), 0, NULL);		  
}

/*******************************************************************************
* Function Name  : GetPortStatus
* Description    : GetPortStatus
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int GetPortStatus( uint8_t usbaddr, uint8_t cPort, uint8_t *desc)
{ 
	return VendorCmd(usbaddr, 0xA3, GET_STATUS, 0, WordSwap((uint16_t)cPort), 0x04, desc);
}

/*******************************************************************************
* Function Name  : GetDevInfo
* Description    : GetDevInfo
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
uint16_t GetDevInfo( uint8_t *DevInfo)
{ 
    uint8_t EpAddr , DescBufLen = 0;
    uint16_t i;

    for(EpAddr = 1; (EpAddr < MAX_DEV); EpAddr++)
	{
		if(uHub.bPortPresent[EpAddr])
		{
			DevInfo[DescBufLen++] = EpAddr;							             /* USB Address */
 			DevInfo[DescBufLen++] = uHub.bPortNumber[EpAddr];		             /* Port Number */
			DevInfo[DescBufLen++] = uHub.bPortSpeed[EpAddr];		             /* Device Speed (from enum) */
			DevInfo[DescBufLen++] = uDev[EpAddr].bClass;			             /* Class Type (from enum) */
			*(uint16_t*)&DevInfo[DescBufLen++] = uDev[EpAddr].wVID;	             /* VID */
			DescBufLen++;
			*(uint16_t*)&DevInfo[DescBufLen++] = uDev[EpAddr].wPID;	             /* PID */
			DescBufLen++;
			DevInfo[DescBufLen++] = (uint8_t)uDev[EpAddr].wPayLoad[0];           /* Ep0 MaxPktSize (max 64 bytes) */
			DevInfo[DescBufLen++] = uDev[EpAddr].bNumOfEPs;			             /* Number of data endpoints */
			for( i = 0; i < uDev[EpAddr].bNumOfEPs; i++ )		                 /* save all data endpoints info */
			{												
				DevInfo[DescBufLen++] = uDev[EpAddr].bEPAddr[i+1];	             /* ep address/direction */
				DevInfo[DescBufLen++] = uDev[EpAddr].bAttr[i+1];	             /* transfer type */
				*(uint16_t*)&DevInfo[DescBufLen++] = uDev[EpAddr].wPayLoad[i+1]; /* max data payload */
				DescBufLen++;                                                    /* 2 byte payload */
				DevInfo[DescBufLen++] = uDev[EpAddr].bInterval[i+1];             /* polling interval */
			}	
		}
    }
	return(DescBufLen);
}

/*******************************************************************************
* Function Name  : DataRW
* Description    : USB Data Endpoint Read/Write wLen is in low byte first format
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int DataRW( uint8_t usbaddr, uint8_t epaddr, uint16_t wPayload, uint16_t wLen, uint8_t *pData )
{
	uint8_t pid = PID_OUT;

	if(epaddr & 0x80)	/* get direction of transfer */
	{
		pid = PID_IN;				
	}
	if(usbXfer(usbaddr,epaddr&0x0F,pid,0,wPayload,wLen,pData))
	{
		return TRUE;
	}
	return FALSE;
}

/*******************************************************************************
* Function Name  : EnumUsbDev
* Description    : USB Device Enumeration Process
*				   Support 1 confguration and interface #0 and alternate setting #0 only
*				   Support up to 1 control endpoint + 4 data endpoint only
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int EnumUsbDev( uint8_t usbaddr )
{  
	int i;				    /* always reset USB transfer address */ 
	uint8_t uAddr = 0;		/* for enumeration to Address #0 */
	uint8_t epLen;
	uint16_t strLang;

	/* Reset only Slave device attached directly */
	uDev[0].wPayLoad[0] = 64;	/* default 64-byte payload of Endpoint 0, address #0 */
	if(usbaddr == 1)			/* bus reset for the device attached to SL811HS only */
	{
		USBReset();				/* that will always have the USB address = 0x01 (for a hub) */
	}
    delay_ms(1);

	/* Get USB Device Descriptors on EP0 & Addr 0 */
	/* with default 64-byte payload */
	pDev =(pDevDesc)DBUF;							  /* ask for 64 bytes on Addr #0 */
	if ( !GetDesc(uAddr,DEVICE,0,18,DBUF) )			  /* and determine the wPayload size */
	{
		return FALSE;								  /* get correct wPayload of Endpoint 0 */
	}
	uDev[usbaddr].wPayLoad[0] = pDev->bMaxPacketSize0;/* on current non-zero USB address */

	/* Set Slave USB Device Address */
#ifdef USB_DEBUG
    printf("Set Slave USB Device Address \r\n");
#endif
	if (!SetAddress(usbaddr)) 		/* set to specific USB address */
	{
		return FALSE;				
	}
	uAddr = usbaddr;				/* ransfer using this new address */

	/* Get USB Device Descriptors on EP0 & Addr X */
	if ( !GetDesc(uAddr,DEVICE,0,(pDev->bLength),DBUF) ) 	
	{
		return FALSE;								/* For this current device */
    }
	uDev[usbaddr].wVID 	 = pDev->idVendor;			/* save VID */
	uDev[usbaddr].wPID 	 = pDev->idProduct;			/* save PID */
	uDev[usbaddr].iMfg 	 = pDev->iManufacturer;		/* save Mfg Index */
	uDev[usbaddr].iPdt 	 = pDev->iProduct;			/* save Product Index */

#ifdef USB_DEBUG
	printf("VID 0x%x, PID 0x%x \r\n", uDev[usbaddr].wVID, uDev[usbaddr].wPID);
#endif

	/* Get String Descriptors */
	//------------------------------------------------
	pStr = (pStrDesc)DBUF;	
	if (!GetDesc(uAddr,STRING,0,4,DBUF)) 			/* Get string language */
	{
		return FALSE;
	}								
	strLang = pStr->wLang;							/* get iManufacturer String length */
	if (!GetDesc(uAddr,(uint16_t)(uDev[usbaddr].iMfg<<8)|STRING,strLang,4,DBUF)) 		
	{	
	    return FALSE;								/* get iManufacturer String descriptors */
	}
	if (!GetDesc(uAddr,(uint16_t)(uDev[usbaddr].iMfg<<8)|STRING,strLang,pStr->bLength,DBUF)) 		
	{
		return FALSE;			
	}
	/* Get Slave USB Configuration Descriptors */
#ifdef USB_DEBUG
    printf("Get Slave USB Configuration Descriptors\n");
#endif

	pCfg = (pCfgDesc)DBUF;									
	if ( !GetDesc(uAddr,CONFIGURATION,0,8,DBUF) ) 		
	{
		return FALSE;
	}										
	if ( !GetDesc(uAddr,CONFIGURATION,0,pCfg->wLength,DBUF) )
	{
		return FALSE;		
	}
	pIfc = (pIntfDesc)(DBUF + 9);					/* point to Interface Descp */
	uDev[usbaddr].bClass 	= pIfc->iClass;			/* update to class type */
	uDev[usbaddr].bNumOfEPs = (pIfc->bEndPoints <= MAX_EP) ? pIfc->bEndPoints : MAX_EP;

	/* Set configuration (except for HUB device) */
#ifdef USB_DEBUG
    printf("Set_Configuration end \r\n");
#endif

	/* For each slave endpoints, get its attributes */
	/* Excluding endpoint0, only data endpoints */
	epLen = 0;
	for ( i = 1; i <= uDev[usbaddr].bNumOfEPs; i++ )		/* For each data endpoint */
	{			
		pEnp = (pEPDesc)(DBUF + 9 + 9 + epLen);	   			/* point to Endpoint Descp(non-HID) */		
		if(pIfc->iClass == HIDCLASS)	
		{	
		    pEnp = (pEPDesc)(DBUF + 9 + 9 + 9 + epLen);		/* update pointer to Endpoint(HID) */	
		}		
		uDev[usbaddr].bEPAddr[i]  	= pEnp->bEPAdd;			/* Ep address and direction */
		uDev[usbaddr].bAttr[i]		= pEnp->bAttr;			/* Attribute of Endpoint */
		uDev[usbaddr].wPayLoad[i] 	= pEnp->wPayLoad;		/* Payload of Endpoint */
		uDev[usbaddr].bInterval[i] 	= pEnp->bInterval;		/* Polling interval */
	    uDev[usbaddr].bData1[i] = 0;			            /* init data toggle */

#ifdef USB_DEBUG
        printf("EndPoint 0x%x, attr = 0x%x, pkt_size = 0x%x, interval = 0x%x\n", uDev[usbaddr].bEPAddr[i],uDev[usbaddr].bAttr[i],uDev[usbaddr].wPayLoad[i],uDev[usbaddr].bInterval[i]);
#endif

		epLen += 7;
	}

	/* Get Hid Report Descriptors */
	if( pIfc->iClass == HIDCLASS )
	{	
#ifdef USB_DEBUG
	    printf("HID Class \r\n");
#endif

		pHid = (pHidDesc)(DBUF + 9 + 9);	   				/* point to HID-CLASS descp */
		if (!GetHid_Desc(uAddr,HID_REPORT,pHid->wItemLength,DBUF))
		{
			return FALSE;
		}
	}

	/* directly to the HUB_DEVICE will be set */
#ifdef USB_DEBUG
    printf("EnumUsbDev Return \r\n");
#endif

	return TRUE;
}

/*******************************************************************************
* Function Name  : speed_detect
* Description    : Full-speed and low-speed detect - Device atttached directly to SL811HS
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int speed_detect(void) 
{	
	volatile int i =0;
	pNumPort    = 0;			                /* zero no. of downstream ports */
	SLAVE_FOUND = FALSE;		                /* Clear USB device found flag */
	FULL_SPEED  = TRUE;			                /* Assume full speed device */
	DATA_STOP   = FALSE;		
 						
	i = sl811_readreg(IntStatus);	            /* Read Interrupt Status */	

	if( i & 0x20 )                              /* device is inserted or removed */
	{
		if( (i & USB_DPLUS) == 0 )		        /* Checking full or low speed */	
		{									    /* Low Speed is detected */
  	       
#ifdef USB_DEBUG
		    printf("'Low' Speed is detected!! Register for status=0x%x \r\n",i);
#endif
			/* Set up Master and low Speed direct and SOF cnt high = 0x2e */
			sl811_writereg(cSOFcnt,0xee);   	/* Set up host and low speed direct and SOF cnt */
			/* SOF Coonuter Low = 0xe0; 1ms interval */
			sl811_writereg(cDATASet,0xe0); 	    /* SOF Counter Low = 0xE0; 1ms interval */
			/* Setup 6Mhz and EOP enable */
			sl811_writereg(CtrlReg,0x21);  	    /* Setup 6MHz and EOP enable */         
			FULL_SPEED = FALSE;		            /* low speed device flag */
		}
		else	
		{	
			/* Full Speed is detected */
#ifdef USB_DEBUG
		    printf("'Full' Speed is detected!! Register for status=0x%x \r\n",i);	
#endif
												
			sl811_writereg(cSOFcnt,0xae);   	/* Set up Master & low speed direct and SOF cnt high=0x2e */
			sl811_writereg(cDATASet,0xe0);  	/* SOF Counter Low = 0xE0; 1ms interval */
			sl811_writereg(CtrlReg,0x05);   	/* Setup 48MHz and SOF enable */
		
		}
	}
	else 
	{
		sl811_writereg(IntStatus,INT_CLEAR);
		
#ifdef USB_DEBUG
	    printf("NO device detected or No Power. \r\n");
#endif

		return 0;
	}
			
	SLAVE_FOUND = TRUE;			            /* Set USB device found flag */
	SLAVE_ENUMERATED = FALSE;		        /* no slave device enumeration */

	sl811_writereg(EP0Status,0x50);   		/* Setup SOF Token, EP0 */
	sl811_writereg(EP0Counter,0x00);		/* reset to zero count */
	sl811_writereg(EP0Control,0x01);   		/* start generate SOF or EOP */
	delay_ms(25);					        /* Hub required approx. 24.1m */
	sl811_writereg(IntStatus,INT_CLEAR);	/* Clear Interrupt status */
	return 0;    					   	    /* exit speed_detect(); */
}

/*******************************************************************************
* Function Name  : slave_detect
* Description    : Detect USB Device
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int slave_detect(void)
{	
	/* Wait for SL811HS enumeration */
	if( !SLAVE_ENUMERATED )					/* only if slave is not configured */
	{
		speed_detect();						/* wait for an USB device to be inserted to */ 
		delay_ms(10);
		if( SLAVE_FOUND )					/* the SL811HST host */
		{
#ifdef USB_DEBUG
	   	    printf("slave found \r\n");
#endif

	  		if( EnumUsbDev(1) )				/* enumerate USB device, assign USB address = #1 */
			{
			   	SLAVE_ENUMERATED = TRUE;	/* Set slave USB device enumerated flag */

				
#ifdef USB_DEBUG
			    printf("'%s' Speed Device Attached. \r\n", FULL_SPEED?"Full":"Low");
#endif
			}	
		}
	}

	/* SL811HS enumerated, proceed accordingly */
	else									
	{													
		if( Slave_Detach() )				/* test for slave device detach */
		{
			return 0;					
		}													
	} 
	return 0;
}

/*******************************************************************************
* Function Name  : Slave_Detach
* Description    : Slave_Detach
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int Slave_Detach(void)
{
	if( (sl811_readreg(IntStatus)&INSERT_REMOVE) || (sl811_readreg(IntStatus)&USB_RESET) )
	{												/* Test USB detached */
		SLAVE_ENUMERATED = FALSE;					/* return to un-enumeration */
		
		sl811_writereg(IntStatus,INT_CLEAR); 		/* clear interrupt status */

#ifdef USB_DEBUG
		printf("Device Detached \r\n");
#endif

		return TRUE;									
	}
	return FALSE;
}
						   
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
