/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               SL811Disk.c
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

/* Includes ------------------------------------------------------------------*/
#include "SL811.h"
#include "SL811Disk.h"

/* Private define ------------------------------------------------------------*/
#define SL811_DISK

/* Private variables ---------------------------------------------------------*/
uint8_t buffer[512];
uint8_t data_toggle , address , endp_in , endp_out;


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
* Function Name  : sl811_set_addr
* Description    : set addr
* Input          : None
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int sl811_set_addr(void)
{
	sl811_writereg(0x10, 0x00);
	sl811_write(0x05);
	sl811_write(address);
	sl811_write(0x00);
	sl811_write(0x00);
	sl811_write(0x00);
	sl811_write(0x00);
	sl811_write(0x00);
	
	sl811_writereg(0x01, 0x10);			/* buffer address */
	sl811_writereg(0x02, 0x08);			/* size max */
	sl811_writereg(0x03, 0xD0);			/* PID SETUP */
	sl811_writereg(0x04, 0x00);			/* device address */

	sl811_writereg(0x0D, 0x01);
	sl811_writereg(0x00, 0x27);			/* send command */

	while( ( sl811_readreg(0x0D) & 0x01 ) == 0 );	
    /* wait ack : host send IN packet and device must answer an empty packet */
	do{
		sl811_writereg(0x01, 0x10);			/* buffer address */
		sl811_writereg(0x02, 0x08);			/* size max */
		sl811_writereg(0x03, 0x90);			/* PID IN */
		sl811_writereg(0x04, 0x00);			/* device address */

		sl811_writereg(0x0D, 0x01);
		sl811_writereg(0x00, 0x23);			/* envoi commande */

		while( ( sl811_readreg(0x0D) & 0x01 ) == 0 );
	}
	while( ( sl811_readreg(0x03) & 0x01 ) != 1 );

	if( sl811_readreg(0x04) != 8 )
	{
		return ERROR;
	}
	return 0;
}

/*******************************************************************************
* Function Name  : sl811_get_device_desc
* Description    : device descriptor
* Input          : None
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void sl811_get_device_desc(uint8_t * buf)
{			
	sl811_writereg(0x10, 0x80);
	sl811_write(0x06);
	sl811_write(0x00);
	sl811_write(0x01);
	sl811_write(0x00);
	sl811_write(0x00);
	sl811_write(0x12);		
	sl811_write(0x00);

	sl811_writereg(0x01, 0x10);
	sl811_writereg(0x02, 0x08);		    /* size */
	sl811_writereg(0x03, 0xD0);		    /* PID SETUP */
	sl811_writereg(0x04, address);		/* device address */
	
	sl811_writereg(0x0D, 0x01);		
	sl811_writereg(0x00, 0x27);	
		
	while( ( sl811_readreg(0x0D) & 0x01 ) == 0 );	

    /* get first bytes of descriptors */
    do{
		sl811_writereg(0x01, 0x10);		/* adresse buffer */
		sl811_writereg(0x02, 0x12);		/* nombre maximal d'octet que l'on peut recevoir */
		sl811_writereg(0x03, 0x90);		/* PID IN */
		sl811_writereg(0x04, address);

		sl811_writereg(0x0D, 0x01);
		sl811_writereg(0x00, 0x23);		/* envoi de la commande */
    
		while( ( sl811_readreg(0x0D) & 0x01 ) == 0 );	
	}
	while( (sl811_readreg(0x03) & 0x40 ) != 0 );

	if( sl811_readreg(0x04) == 0 ) 		    /* read number of recieved bytes */
	{
		sl811_read_buf(0x10, buf, 0x12);    /* copy 18 bytes */
	}
	else									/* 10 bytes again */
	{
		sl811_read_buf(0x10, buf, 0x08);
        /* next 8 bytes of the descriptor */
		do{
			sl811_writereg(0x01, 0x10);		
			sl811_writereg(0x02, 0x08);
			sl811_writereg(0x03, 0x90);	    /* PID IN */
			sl811_writereg(0x04, address);

			sl811_writereg(0x0D, 0x01);
			sl811_writereg(0x00, 0x23);		/* send command */
			while( ( sl811_readreg(0x0D) & 0x01 ) == 0 );	
		}
		while( ( sl811_readreg(0x03) & 0x40 ) != 0 );

		buf[8] = sl811_readreg(0x10);
		buf[9] = sl811_read();
		buf[10] = sl811_read();
		buf[11] = sl811_read();
		buf[12] = sl811_read();
		buf[13] = sl811_read();
		buf[14] = sl811_read();
		buf[15] = sl811_read();

       /* last 2 bytes of the descriptor */
		do{
			sl811_writereg(0x01, 0x20);	
			sl811_writereg(0x02, 0x02);
			sl811_writereg(0x03, 0x90);	     /* PID IN */
			sl811_writereg(0x04, address);

			sl811_writereg(0x0D, 0x01);
			sl811_writereg(0x00, 0x23);		 /* send command */
	   
			while( ( sl811_readreg(0x0D) & 0x01 ) == 0 );

			buf[16] = sl811_readreg(0x20);
			buf[17] = sl811_read();
		}
		while( ( sl811_readreg(0x03) & 0x40 ) != 0 );
	}	
    /* send PID OUT to finish transaction */		
	sl811_writereg(0x01, 0x10);
	sl811_writereg(0x02, 0x00);		   /* size 0 */
	sl811_writereg(0x03, 0x10);		   /* PID OUT */
	sl811_writereg(0x04, address);	   /* device address */
	
	sl811_writereg(0x0D, 0x01);
	sl811_writereg(0x00, 0x67);		
	while( (sl811_readreg(0x0D) & 0x01 ) == 0 );
}

/*******************************************************************************
* Function Name  : sl811_get_device_conf
* Description    : configuration descriptor
* Input          : None
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void sl811_get_device_conf( uint8_t * buf)
{
	/* Device descriptor */	
	sl811_writereg(0x10, 0x80);
	sl811_write(0x06);
	sl811_write(0x00);
	sl811_write(0x02);
	sl811_write(0x00);
	sl811_write(0x00);
	sl811_write(0x20);		
	sl811_write(0x00);

	sl811_writereg(0x01, 0x10);		
	sl811_writereg(0x02, 0x08);		  /* size */
	sl811_writereg(0x03, 0xD0);		  /* PID SETUP */
	sl811_writereg(0x04, address);	  /* device address */
	sl811_writereg(0x0D, 0x01);		
	sl811_writereg(0x00, 0x07);		
	while( (sl811_readreg(0x0D) & 0x01 ) == 0 );	
    /* get first bytes */
	do{
		sl811_writereg(0x01, 0x10);		
		sl811_writereg(0x02, 0x20);		/* size */
		sl811_writereg(0x03, 0x90);		/* PID IN */
		sl811_writereg(0x04, address);

		sl811_writereg(0x0D, 0x01);
		sl811_writereg(0x00, 0x03);		/* send cmd */
		while( ( sl811_readreg(0x0D) & 0x01 ) == 0 );
	}
	while( ( sl811_readreg(0x03) & 0x40 ) != 0 );

	if( sl811_readreg(0x04) == 0 ) 			/* read number of bytes */
	{
		sl811_read_buf(0x10, buf, 0x20);	/* copy bytes */
	}
	else									/* 1 bytes to read */
	{
		sl811_read_buf(0x10, buf, 0x08);

        /* last bytes of the descriptor */
		do{
			sl811_writereg(0x01, 0x10);		
			sl811_writereg(0x02, 0x08);
			sl811_writereg(0x03, 0x90);		/* PID IN */
			sl811_writereg(0x04, address);

			sl811_writereg(0x0D, 0x01);
			sl811_writereg(0x00, 0x03);		/* send cmd */
			while( (sl811_readreg(0x0D) & 0x01 ) == 0 );
		}
		while( ( sl811_readreg(0x03) & 0x40 ) != 0 );
	
		sl811_read_buf(0x10, buf + 8, 0x08);
		
		do{
			sl811_writereg(0x01, 0x10);		
			sl811_writereg(0x02, 0x08);
			sl811_writereg(0x03, 0x90);		/* PID IN */
			sl811_writereg(0x04, address);

			sl811_writereg(0x0D, 0x01);
			sl811_writereg(0x00, 0x03);		/* send cmd */
			while( (sl811_readreg (0x0D) & 0x01 ) == 0 );
		}
		while( ( sl811_readreg(0x03) & 0x40 ) != 0 );

		sl811_read_buf(0x10, buf + 16, 0x08);

        /* 8 last bytes */	
		do{
			sl811_writereg(0x01, 0x10);		
			sl811_writereg(0x02, 0x08);
			sl811_writereg(0x03, 0x90);		/* PID IN */
			sl811_writereg(0x04, address);

			sl811_writereg(0x0D, 0x01);
			sl811_writereg(0x00, 0x03);		/* send cmd */
			while( (sl811_readreg(0x0D) & 0x01 ) == 0 );
		}
		while( (sl811_readreg(0x03) & 0x40 ) != 0 );

		sl811_read_buf(0x10, buf + 24, 0x08);	
	}		
    /* send a PID OUT to finish request */		
	sl811_writereg(0x01, 0x00);		
	sl811_writereg(0x02, 0x00);		/* size */
	sl811_writereg(0x03, 0x10);		/* PID OUT */
	sl811_writereg(0x04, address);	/* device address */
	
	sl811_writereg(0x0D, 0x01);
	sl811_writereg(0x00, 0x47);		/* PID DATA1 */ 
	while( (sl811_readreg(0x0D) & 0x01 ) == 0 );
}

/*******************************************************************************
* Function Name  : sl811_set_conf
* Description    : set configuration 
* Input          : None
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int sl811_set_conf(void)
{
	sl811_writereg(0x10, 0x00);
	sl811_write(0x09);
	sl811_write(0x01);
	sl811_write(0x00);
	sl811_write(0x00);
	sl811_write(0x00);
	sl811_write(0x00);
	sl811_write(0x00);
	
	sl811_writereg(0x01, 0x10);			
	sl811_writereg(0x02, 0x08);			/* size */
	sl811_writereg(0x03, 0xD0);			/* PID SETUP */
	sl811_writereg(0x04, address);		/* device address */

	sl811_writereg(0x0D, 0x01);
	sl811_writereg(0x00, 0x07);
	while( (sl811_readreg(0x0D) & 0x01 ) == 0 );
	sl811_writereg(0x01, 0x10);			
	sl811_writereg(0x02, 0x08);			/* size */
	sl811_writereg(0x03, 0x90);			/* PID IN */
	sl811_writereg(0x04, address);		/* device address */
	sl811_writereg(0x0D, 0x01);
	sl811_writereg(0x00, 0x03);			/* send commande */
	while( ( sl811_readreg(0x0D) & 0x01 ) == 0 );
	if( (sl811_readreg(0x03) & 0x01 ) == 0 )
	{
	   return ERROR;	
	}
	return 0;
}

/*******************************************************************************
* Function Name  : sl811_get_capacity
* Description    : get capacity
* Input          : None
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int sl811_get_capacity( uint8_t * buf )
{
    /* Send CBW "USBC" */
	sl811_writereg(0x10, 'U');	
	sl811_write('S');	
	sl811_write('B');	
	sl811_write('C');	
	sl811_write('P');	
	sl811_write('I');	
	sl811_write('K');	
	sl811_write('A');	
	sl811_write(0x08);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x80);	
	sl811_write(0x00);	
	sl811_write(0x0C);
	sl811_write(0x25);
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	do{
		sl811_writereg(0x01, 0x10);		
		sl811_writereg(0x02, 0x1F);				 /* size */
		sl811_writereg(0x03, 0x10 + endp_out);	 /* PID OUT ENDP1 */
		sl811_writereg(0x04, address);			 /* device address */
	
		sl811_writereg(0x0D, 0x01);
		if( (data_toggle & 0x01) )
		{
		  sl811_writereg(0x00, 0x67);
		}	
		else 
	    {
		  sl811_writereg(0x00, 0x27);
		}

		data_toggle = data_toggle ^ 0x01;
		while( ( sl811_readreg(0x0D) & 0x01 ) == 0 );
	}
	while( (sl811_readreg(0x03) & 0x01 ) != 1 );	
	do{
		sl811_writereg(0x01, 0x10);			
		sl811_writereg(0x02, 0x08);			    /* size */
		sl811_writereg(0x03, 0x90 + endp_in);	/* PID IN ENDP2 */
		sl811_writereg(0x04, address);			/* device address */
		sl811_writereg(0x0D, 0x01);
		sl811_writereg(0x00, 0x23);			    /* send cmd */
		while( (sl811_readreg(0x0D) & 0x01 ) == 0 );

		if( (sl811_readreg(0x03) & 0x80 ) != 0 )
		{
		    return STALL;
	    }
	}
	while( (sl811_readreg(0x03) & 0x01 ) != 1 );
	
	sl811_read_buf(0x10, buf, 0x08);
    /* Get CSW "USBS" */
	do{
		sl811_writereg(0x01, 0x10);			
		sl811_writereg(0x02, 0x0D);				/* size */
		sl811_writereg(0x03, 0x90 + endp_in);	/* PID IN */
		sl811_writereg(0x04, address);			/* device address */

		sl811_writereg(0x0D, 0x01);
		sl811_writereg(0x00, 0x23);				/* send cmd */
		while( (sl811_readreg(0x0D) & 0x01 ) == 0 );
	}
    while( (sl811_readreg(0x03) & 0x01 ) != 1 );
	
	if( sl811_readreg(0x10+ 0x0C) != 0 )
	{
		return ERROR;
	}
	return 0;	
}

/*******************************************************************************
* Function Name  : sl811_clear_feature
* Description    : clear feature
* Input          : None
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void sl811_clear_feature(void)
{
	sl811_writereg(0x10, 0x02);
	sl811_write(0x01);
	sl811_write(0x00);
	sl811_write(0x00);
	sl811_write(0x80 + endp_in);
	sl811_write(0x00);
	sl811_write(0x00);
	sl811_write(0x00);
	
	sl811_writereg(0x01, 0x10);			
	sl811_writereg(0x02, 0x08);			/* size */
	sl811_writereg(0x03, 0xD0);			/* PID SETUP */
	sl811_writereg(0x04, address);		/* adevice address */

	sl811_writereg(0x0D, 0x01);
	sl811_writereg(0x00, 0x27);			/* send cmd */
	while( (sl811_readreg(0x0D) & 0x01 ) == 0 );
	
	do{
		sl811_writereg(0x01, 0x10);			
		sl811_writereg(0x02, 0x08);		/* size */
		sl811_writereg(0x03, 0x90);		/* PID IN */
		sl811_writereg(0x04, address);	/* device address */

		sl811_writereg(0x0D, 0x01);
		sl811_writereg(0x00, 0x23);		/* send cmd */
		while( (sl811_readreg(0x0D) & 0x01 ) == 0 );
	}
	while( (sl811_readreg(0x03) & 0x01) != 1 );

   /* get error frame */
	do{
		sl811_writereg(0x01, 0x10);			
		sl811_writereg(0x02, 0x0D);				/* size */
		sl811_writereg(0x03, 0x90 + endp_in);	/* PID IN */
		sl811_writereg(0x04, address);			/* device address */

		sl811_writereg(0x0D, 0x01);
		sl811_writereg(0x00, 0x23);				/* send cmd */
		while( (sl811_readreg(0x0D) & 0x01 ) == 0);
	}
	while( (sl811_readreg(0x03) & 0x01) != 1 );
}

/*******************************************************************************
* Function Name  : sl811_read_sector
* Description    : read sector
* Input          : None
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int sl811_read_sector( uint32_t sector, uint8_t * buf )
{
    uint8_t index;
		
	while( sl811_usb_ready() != 0 );

	sl811_writereg(0x10, 'U');	
	sl811_write('S');	
	sl811_write('B');	
	sl811_write('C');	
	sl811_write('P');	
	sl811_write('I');	
	sl811_write('K');	
	sl811_write('A');	
	sl811_write(0x00);		
	sl811_write(0x02);			/* 512 bytes */
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x80);	
	sl811_write(0x00);	
	sl811_write(0x0C);
	sl811_write(0x28);
	sl811_write(0x00);	
	sl811_write((uint8_t)(sector>>24));	
	sl811_write((uint8_t)(sector>>16));	
	sl811_write((uint8_t)(sector>>8));
	sl811_write((uint8_t)(sector));	
	sl811_write(0x00);	
	sl811_write(0x00);			
	sl811_write(0x01);			/* sector number */
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	

	do{	
		sl811_writereg(0x01, 0x10);		
		sl811_writereg(0x02, 0x1F);				    /* size */
		sl811_writereg(0x03, 0x10 + endp_out);		/* PID OUT ENDP1 */
		sl811_writereg(0x04, address);				/* device address */
	
		sl811_writereg(0x0D, 0x01);
		if( (data_toggle & 0x01) )
		{
			sl811_writereg(0x00, 0x67);
		}	
		else 
	    {  
			sl811_writereg(0x00, 0x27);
		}
		data_toggle = data_toggle ^ 0x01;

		while( ( sl811_readreg(0x0D) & 0x01 ) == 0);
	}
	while( (sl811_readreg(0x03) & 0x01 ) != 1 );

	for( index = 0; index < 8; index++ )
	{		
		do{
			sl811_writereg(0x01, 0x10);			
			sl811_writereg(0x02, 0x40);				/* size */
			sl811_writereg(0x03, 0x90 + endp_in);	/* PID IN ENDP2 */
			sl811_writereg(0x04, address);			/* device address */
			sl811_writereg(0x0D, 0x01);
			sl811_writereg(0x00, 0x23);				/* send cmd */
			while( (sl811_readreg(0x0D)&0x01) == 0);

#ifdef SL811_DISK
			if( (sl811_readreg(0x03)&0x02) == 1 ) 
			{
				printf("ERR CRC \r\n");
			}
#endif
		}
		while( (sl811_readreg(0x03)&0x01) != 1 );
	
		sl811_read_buf(0x10, buf + ( index * 0x40 ), 0x40 );
	}
    /* get CSW "USBS" frame to verify errors */
	do{	
		sl811_writereg(0x01, 0x10);			
		sl811_writereg(0x02, 0x0D);					/* size */
		sl811_writereg(0x03, 0x90 + endp_in);		/* PID IN */
		sl811_writereg(0x04, address);				/* device address */
		sl811_writereg(0x0D, 0x01);
		sl811_writereg(0x00, 0x23);					/* send cmd */
		while( (sl811_readreg(0x0D) & 0x01 ) == 0);
	}
	while( (sl811_readreg(0x03) & 0x01 ) != 1 );
	
	if( sl811_readreg(0x10 + 0x0C) != 0 )
	{
		return ERROR; 
	}
	return 0;		
}

/*******************************************************************************
* Function Name  : sl811_write_sector
* Description    : write sector
* Input          : None
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int sl811_write_sector( uint32_t sector, uint8_t * buf )
{
    uint8_t index;
	
	while( sl811_usb_ready() != 0 );

	/* Fill buffer */
	sl811_writereg(0x10, 'U');	
	sl811_write('S');	
	sl811_write('B');	
	sl811_write('C');	
	sl811_write('P');	
	sl811_write('I');	
	sl811_write('K');	
	sl811_write('A');	
	sl811_write(0x00);	
	sl811_write(0x02);		/* 512 bytes */
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x0C);
	sl811_write(0x2A);
	sl811_write(0x00);	
	sl811_write(sector>>24);	
	sl811_write(sector>>16);	
	sl811_write(sector>>8);
	sl811_write(sector);	
	sl811_write(0x00);	
	sl811_write(0x00);			
	sl811_write(0x01);		/* sector number */
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	do{	 	
		sl811_writereg(0x01, 0x10);		
		sl811_writereg(0x02, 0x1F);				    /* size */
		sl811_writereg(0x03, 0x10 + endp_out);		/* PID OUT ENDP1 */
		sl811_writereg(0x04, address);				/* device address */
		sl811_writereg(0x0D, 0x01);
		if( (data_toggle & 0x01) )
		{  
		  sl811_writereg(0x00, 0x67);
	    }
		else 
		{  
		  sl811_writereg(0x00, 0x27);
		}
		while( (sl811_readreg(0x0D) & 0x01 ) == 0 );
	}
    while( ( sl811_readreg(0x03) & 0x01 ) != 1 );

	data_toggle = data_toggle ^ 0x01;

    /* send 8 OUT packets of 64 bytes each */ 
	for( index = 0; index < 8; index++ )
	{		
		do{
			sl811_write_buf(0x10, buf + (index*0x40), 0x40);
			
			sl811_writereg(0x01, 0x10);			
			sl811_writereg(0x02, 0x40);					/* size 64 */
			sl811_writereg(0x03, 0x10 + endp_out);		/* PID OUT ENDP1 */
			sl811_writereg(0x04, address);				/* device address */

			sl811_writereg(0x0D, 0x01);
			if( (data_toggle & 0x01) )
            {
			  sl811_writereg(0x00, 0x67);
			}		
			else 
 	        {  
			  sl811_writereg(0x00, 0x27);
			}
			while( (sl811_readreg(0x0D) & 0x01 ) == 0);
		}
		while( (sl811_readreg(0x03) & 0x01 ) != 1 );
		data_toggle = data_toggle ^ 0x01;
	}	
    /* get CSW "USBS" to verify errors */
	do{	
		sl811_writereg(0x01, 0x10);			
		sl811_writereg(0x02, 0x0D);					/* size */
		sl811_writereg(0x03, 0x90 + endp_in);		/* PID IN */
		sl811_writereg(0x04, address);				/* device address */
		sl811_writereg(0x0D, 0x01);
		sl811_writereg(0x00, 0x23);					/* send cmd */
		while((sl811_readreg(0x0D)&0x01) == 0);
	}
	while( (sl811_readreg(0x03)&0x01) != 1 );
	
	if( sl811_readreg(0x10+ 0x0C) != 0 )
	{  
		return ERROR;	
	}	
	
	return 0;	
} 

/*******************************************************************************
* Function Name  : sl811_usb_ready
* Description    : ready
* Input          : None
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int sl811_usb_ready(void)
{
	/* fill buffer */
	sl811_writereg(0x10, 'U');	
	sl811_write('S');	
	sl811_write('B');	
	sl811_write('C');	
	sl811_write('P');	
	sl811_write('I');	
	sl811_write('K');	
	sl811_write('A');	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x0C);
	sl811_write(0x00);
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);		
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	sl811_write(0x00);	
	do{		
		sl811_writereg(0x01, 0x10);		
		sl811_writereg(0x02, 0x1F);				    /* size */
		sl811_writereg(0x03, 0x10 + endp_out);		/* PID OUT + ENDP */
		sl811_writereg(0x04, address);				/* device address */
	
		sl811_writereg(0x0D, 0x01);
		if( (data_toggle & 0x01) )
		{
			sl811_writereg(0x00, 0x67);
		}
		else
		{  
			sl811_writereg(0x00, 0x27);
		}
		data_toggle = data_toggle ^ 0x01;
		while( (sl811_readreg(0x0D) & 0x01 ) == 0);
	}
	while( ( sl811_readreg(0x03) & 0x01 ) != 1 );
    /* get CSW "USBS" to verify errors */
	do{	
		sl811_writereg(0x01, 0x10);			
		sl811_writereg(0x02, 0x0D);					/* size */
		sl811_writereg(0x03, 0x90 + endp_in);		/* PID IN */
		sl811_writereg(0x04, address);				/* device address */

		sl811_writereg(0x0D, 0x01);
		sl811_writereg(0x00, 0x23);					/* send cmd */
		while( (sl811_readreg(0x0D) & 0x01 ) == 0);
	}
	while( ( sl811_readreg(0x03) & 0x01 ) != 1 );
		
	if( (sl811_readreg(0x04) == 0) && (sl811_readreg(0x1C) == 0) )
	{  
		return READY;
	}
	else 
	{
		return BUSY;	
	}
} 

/*******************************************************************************
* Function Name  : sl811_disk_init
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void asci_print( uint8_t car )
{
	uint8_t temp;
	temp = ( car >> 4 ) + 0x30;
	if(temp > '9')
	{
		temp += 7;
	}	
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	USART_SendData(USART1, temp);
	
	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{}
	
	temp = ( car & 0x0F ) + 0x30;
	if(temp > '9')
	{
		temp += 7;
	}	
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	USART_SendData(USART1, temp);
	
	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{}
}

/*******************************************************************************
* Function Name  : sl811_disk_init
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int sl811_disk_init(void)
{
	uint8_t index;

	data_toggle = 0;			        /* init toggle */
								 
	printf("Chip revision: ");
	asci_print( sl811_readreg(0x0E) );

#ifdef SL811_DISK	
	printf("\r\nPlease connect USB device...\r\n");
#endif

	while( ( sl811_readreg(0x0D) & 0x40 ) )
	{
		delay_ms(200);
	}
				
	if( ( sl811_readreg(0x0D) & 0x80 ) == 0 )
	{
#ifdef SL811_DISK
		printf("USB Low speed device !!!\r\n");
#endif
		return NOT_FULL_SPEED;		/* return error if low speed device detected */
	}
#ifdef SL811_DISK
	printf("USB Full speed device detected\r\n");
#endif
	
	sl811_writereg(0x01, 0x10);		
	sl811_writereg(0x09, 0x50);		

	sl811_writereg(0x0F, 0xAE);		/* config host & frame generator prescaler */
	sl811_writereg(0x0E, 0xE0);		
	sl811_writereg(0x05, 0x01);
	sl811_writereg(0x00, 0x01);
	sl811_writereg(0x04, 0x00);		/* device address */
		
	delay_ms(50);
	address = 0;
	
	sl811_writereg(0x05, 0x09);		/* reset usb device */
	delay_ms(20);
	sl811_writereg(0x05, 0x01);

	delay_ms(50);					/* wait 50 ms */

	address = 1;
	sl811_set_addr();				/* Set Address : address will be 0x01 */

	delay_ms(20);

	sl811_get_device_desc(buffer);

    /* display device descriptor */
#ifdef SL811_DISK
	printf("Device descriptor :\r\n");
	for(index = 0; index <18; index++ )
	{
		asci_print(buffer[index]);
		printf(" ");
	}
#endif

	delay_ms(20);

	sl811_get_device_conf(buffer);

    /* display configuration descriptor */
#ifdef SL811_DISK
	printf("\r\nConfiguration descriptor :\r\n");
	for(index = 0; index < 16; index++ )
	{
		asci_print(buffer[index]);
		printf(" ");
	}
	printf("\r\n");
	for(index = 0; index < 16; index++ )
	{
		asci_print( buffer[index+16] );
		printf(" ");
	}
#endif
	
    /* check device class : 0x08 => MASS STORAGE CLASS */
	if( buffer[14] != 0x08 || buffer[9] != 0x09 || buffer[10] != 0x04 )
	{
		return NOT_MASS_STORAGE;
	}

#ifdef SL811_DISK
	printf("\r\nMass storage class device detected\r\n");
#endif

    /* get endpoint number */
	if( (buffer[0x12] == 0x07) && (buffer[0x13] == 0x05) )
	{
		if( (buffer[0x14] & 0x80) != 0 )
		{
			endp_in = buffer[0x14] & 0x0F;
		}
		else 
		{
			endp_out = buffer[0x14] & 0x0F;
		}
		if( (buffer[0x19] == 0x07) && (buffer[0x1A] == 0x05) )
		{
			if( (buffer[0x1B] & 0x80) != 0 )
			{
				endp_in = buffer[0x1B] & 0x0F;
			}
			else 
			{
				endp_out = buffer[0x1B] & 0x0F;
			}
		}
	}

	delay_ms(20);

	sl811_set_conf();
	
#ifdef SL811_DISK
	printf("Set config : OK \r\n");
#endif

	while( sl811_usb_ready() != 0 );
					
	if( sl811_get_capacity(buffer) == STALL )
	{
		sl811_clear_feature();
	}
		
	while( sl811_usb_ready() != 0 );

    /* get device capacity */
	sl811_get_capacity(buffer);

#ifdef SL811_DISK
	printf("Device capacity : ");
						   //73fff;
	for( index =0; index < 4; index++ )
	{
		asci_print(buffer[index]);
		printf(" ");
	}
	printf("\n\rSector size : ");
	for( ; index < 8; index++ )
	{
		asci_print(buffer[index]);
		printf(" ");
	}
	printf("\r\n");
#endif

	while( sl811_usb_ready() != 0 );

	return 0;
}
 
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
