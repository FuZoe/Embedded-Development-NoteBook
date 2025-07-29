/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               dm9000a.c
** Descriptions:            None
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2011-3-20
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
#include "dm9000a.h"
#include "includes.h"

/* Private define ------------------------------------------------------------*/

/* #define DM9000_DEBUG */

#ifdef  DM9000_DEBUG 
#define DM9000_TRACE	printf

#else
#define DM9000_TRACE(...)
#endif

#define DM9000_PHY              0x40    /* PHY address 0x01 */

#define DM9000_REG              (*((volatile unsigned short *) 0x6C400000)) 
#define DM9000_RAM              (*((volatile unsigned short *) 0x6C400002)) 

 /* Private variables ---------------------------------------------------------*/
static struct dm9000_eth dm9000_device;
static OS_EVENT *dm9000_sem_ack; 
static OS_EVENT *dm9000_sem_lock;


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
* Function Name  : dm9000_io_read
* Description    : Read a byte from I/O port
* Input          : - reg: reg
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static __inline uint8_t dm9000_io_read(uint16_t reg)
{
    DM9000_REG = reg;
    return (uint8_t) DM9000_RAM;
}

/*******************************************************************************
* Function Name  : dm9000_io_write
* Description    : Write a byte to I/O port
* Input          : - reg: reg
*                  - value: data
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static __inline void dm9000_io_write(uint16_t reg, uint16_t value)
{
    DM9000_REG = reg;
    DM9000_RAM = value;
}

/*******************************************************************************
* Function Name  : phy_read
* Description    : Read a word from phyxcer
* Input          : - reg: reg
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static uint16_t phy_read(uint16_t reg)
{
    uint16_t val;

    /* Fill the phyxcer register into REG_0C */
    dm9000_io_write(DM9000_EPAR, DM9000_PHY | reg);
    dm9000_io_write(DM9000_EPCR, 0xc);	/* Issue phyxcer read command */

    delay_ms(100);		                /* Wait read complete */

    dm9000_io_write(DM9000_EPCR, 0x0);	/* Clear phyxcer read command */
    val = (dm9000_io_read(DM9000_EPDRH) << 8) | dm9000_io_read(DM9000_EPDRL);

    return val;
}

/*******************************************************************************
* Function Name  : phy_write
* Description    : Write a word to phyxcer
* Input          : - reg: reg
*                  - value: data
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void phy_write(uint16_t reg, uint16_t value)
{
    /* Fill the phyxcer register into REG_0C */
    dm9000_io_write(DM9000_EPAR, DM9000_PHY | reg);

    /* Fill the written data into REG_0D & REG_0E */
    dm9000_io_write(DM9000_EPDRL, (value & 0xff));
    dm9000_io_write(DM9000_EPDRH, ((value >> 8) & 0xff));
    dm9000_io_write(DM9000_EPCR, 0xa);	/* Issue phyxcer write command */

    delay_ms(500);		                /* Wait write complete */

    dm9000_io_write(DM9000_EPCR, 0x0);	/* Clear phyxcer write command */
}

/*******************************************************************************
* Function Name  : phy_mode_set
* Description    : Set PHY operationg mode
* Input          : - media_mode:
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void phy_mode_set(uint32_t media_mode)
{
    uint16_t phy_reg4 = 0x01e1, phy_reg0 = 0x1000;
    if( !(media_mode & DM9000_AUTO) )
    {
        switch (media_mode)
        {
	        case DM9000_10MHD:
	             phy_reg4 = 0x21;
	             phy_reg0 = 0x0000;
	             break;
	        case DM9000_10MFD:
	             phy_reg4 = 0x41;
	             phy_reg0 = 0x1100;
	             break;
	        case DM9000_100MHD:
	             phy_reg4 = 0x81;
	             phy_reg0 = 0x2000;
	             break;
	        case DM9000_100MFD:
	             phy_reg4 = 0x101;
	             phy_reg0 = 0x3100;
	             break;
        }
        phy_write(4, phy_reg4);	        /* Set PHY media mode */
        phy_write(0, phy_reg0);	        /* Tmp */
    }

    dm9000_io_write(DM9000_GPCR, 0x01);	/* Let GPIO0 output */
    dm9000_io_write(DM9000_GPR, 0x00);	/* Enable PHY */
}

/*******************************************************************************
* Function Name  : dm9000_isr
* Description    : interrupt service routine
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void dm9000_isr(void)
{
    uint16_t int_status;
    uint16_t last_io;

    last_io = DM9000_REG;

    /* Got DM9000 interrupt status */
    int_status = dm9000_io_read(DM9000_ISR);    /* Got ISR */
    dm9000_io_write(DM9000_ISR, int_status);    /* Clear ISR status */

	DM9000_TRACE("dm9000 isr: int status %04x \r\n", int_status);

    /* receive overflow */
    if (int_status & ISR_ROS)
    {
        DM9000_TRACE("overflow \r\n");
    }

    if (int_status & ISR_ROOS)
    {
        DM9000_TRACE("overflow counter overflow \r\n");
    }

    /* Received the coming packet */
    if (int_status & ISR_PRS)
    {
	    extern OS_EVENT* ethernetinput;
        /* disable receive interrupt */
        dm9000_io_write(DM9000_IMR, IMR_PAR);
        dm9000_device.imr_all = IMR_PAR | IMR_PTM;
        dm9000_io_write(DM9000_IMR, dm9000_device.imr_all);
		/* a frame has been received */
	    OSSemPost(ethernetinput);	  
    }
    /* Transmit Interrupt check */
    if (int_status & ISR_PTS)
    {
        /* transmit done */
        int tx_status = dm9000_io_read(DM9000_NSR);    /* Got TX status */

        if (tx_status & (NSR_TX2END | NSR_TX1END))
        {
            dm9000_device.packet_cnt --;
            if (dm9000_device.packet_cnt > 0)
            {
                DM9000_TRACE("dm9000 isr: tx second packet \r\n");

                /* transmit packet II */
                /* Set TX length to DM9000 */
                dm9000_io_write(DM9000_TXPLL, dm9000_device.queue_packet_len & 0xff);
                dm9000_io_write(DM9000_TXPLH, (dm9000_device.queue_packet_len >> 8) & 0xff);

                /* Issue TX polling command */
                dm9000_io_write(DM9000_TCR, TCR_TXREQ);	/* Cleared after TX complete */
            }
            /* unlock DM9000 device */
	        OSSemPost(dm9000_sem_ack);	 
        }
    }

    DM9000_REG = last_io;
}

/*******************************************************************************
* Function Name  : dm9000_init
* Description    : initialize the interface
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int dm9000_init(void)
{
    int i, oft, lnk;
    uint32_t value;

    /* RESET device */
    dm9000_io_write(DM9000_NCR, NCR_RST);
    delay_ms(1000);		/* delay 1000 ms */

    /* identfy DM9000 */
    value  = dm9000_io_read(DM9000_VIDL);
    value |= dm9000_io_read(DM9000_VIDH) << 8;
    value |= dm9000_io_read(DM9000_PIDL) << 16;
    value |= dm9000_io_read(DM9000_PIDH) << 24;
    if (value == DM9000_ID)
    {
        DM9000_TRACE("dm9000 id: 0x%x \r\n", value);
    }
    else
    {
        return -1;
    }

    /* GPIO0 on pre-activate PHY */
    dm9000_io_write(DM9000_GPR, 0x00);	            /* REG_1F bit0 activate phyxcer */
    dm9000_io_write(DM9000_GPCR, GPCR_GEP_CNTL);    /* Let GPIO0 output */
    dm9000_io_write(DM9000_GPR, 0x00);              /* Enable PHY */

    /* Set PHY */
    phy_mode_set(dm9000_device.mode);

    /* Program operating register */
    dm9000_io_write(DM9000_NCR, 0x0);	                                /* only intern phy supported by now */
    dm9000_io_write(DM9000_TCR, 0);	                                    /* TX Polling clear */
    dm9000_io_write(DM9000_BPTR, 0x3f);	                                /* Less 3Kb, 200us */
    dm9000_io_write(DM9000_FCTR, FCTR_HWOT(3) | FCTR_LWOT(8));	        /* Flow Control : High/Low Water */
    dm9000_io_write(DM9000_FCR, 0x0);	                                /* SH FIXME: This looks strange! Flow Control */
    dm9000_io_write(DM9000_SMCR, 0);	                                /* Special Mode */
    dm9000_io_write(DM9000_NSR, NSR_WAKEST | NSR_TX2END | NSR_TX1END);	/* clear TX status */
    dm9000_io_write(DM9000_ISR, 0x0f);	                                /* Clear interrupt status */
    dm9000_io_write(DM9000_TCR2, 0x80);	                                /* Switch LED to mode 1 */

    /* set mac address */
    for (i = 0, oft = 0x10; i < 6; i++, oft++)
    {
		dm9000_io_write(oft, dm9000_device.dev_addr[i]);
	}
    /* set multicast address */
    for (i = 0, oft = 0x16; i < 8; i++, oft++)
    {
	    dm9000_io_write(oft, 0xff);
	}

    /* Activate DM9000 */
    dm9000_io_write(DM9000_RCR, RCR_DIS_LONG | RCR_DIS_CRC | RCR_RXEN);	/* RX enable */
    dm9000_io_write(DM9000_IMR, IMR_PAR);

    if (dm9000_device.mode == DM9000_AUTO)
    {
        while (!(phy_read(1) & 0x20))
        {
            /* autonegation complete bit */
            delay_ms( 10 );
            i++;
            if (i == 255)
            {
                DM9000_TRACE("could not establish link \r\n");
                return 0;
            }
        }
    }

    /* see what we've got */
    lnk = phy_read(17) >> 12;
    DM9000_TRACE("operating at ");
    switch (lnk)
    {
		case 1:
		     DM9000_TRACE("10M half duplex ");
		     break;
		case 2:
		     DM9000_TRACE("10M full duplex ");
		     break;
		case 4:
		     DM9000_TRACE("100M half duplex ");
		     break;
		case 8:
		     DM9000_TRACE("100M full duplex ");
		     break;
		default:
		     DM9000_TRACE("unknown: %d ", lnk);
		     break;
    }
    DM9000_TRACE("mode \r\n");

    dm9000_io_write(DM9000_IMR, dm9000_device.imr_all);	/* Enable TX/RX interrupt mask */

    return 0;
}

/*******************************************************************************
* Function Name  : dm9000_close
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int dm9000_close(void)
{
    /* RESET devie */
    phy_write(0, 0x8000);	            /* PHY RESET */
    dm9000_io_write(DM9000_GPR, 0x01);	/* Power-Down PHY */
    dm9000_io_write(DM9000_IMR, 0x80);	/* Disable all interrupt */
    dm9000_io_write(DM9000_RCR, 0x00);	/* Disable RX */

    return 0;
}

/*******************************************************************************
* Function Name  : dm9000_tx
* Description    : transmit packet
* Input          : - p: packet 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int dm9000_tx( struct pbuf* p)
{
    INT8U err;

    DM9000_TRACE("dm9000 tx: %d \r\n", p->tot_len);

    /* lock DM9000 */
    OSSemPend(dm9000_sem_lock,0,&err);	  

    /* disable dm9000a interrupt */
    dm9000_io_write(DM9000_IMR, IMR_PAR);

    /* Move data to DM9000 TX RAM */
    DM9000_outb(DM9000_IO_BASE, DM9000_MWCMD);

    {
        /* q traverses through linked list of pbuf's
         * This list MUST consist of a single packet ONLY */
        struct pbuf *q;
        uint16_t pbuf_index = 0;
        uint8_t word[2], word_index = 0;

        q = p;
        /* Write data into dm9000a, two bytes at a time
         * Handling pbuf's with odd number of bytes correctly
         * No attempt to optimize for speed has been made */
        while (q)
        {
            if (pbuf_index < q->len)
            {
                word[word_index++] = ((u8_t*)q->payload)[pbuf_index++];
                if (word_index == 2)
                {
                    DM9000_outw(DM9000_DATA_BASE, (word[1] << 8) | word[0]);
                    word_index = 0;
                }
            }
            else
            {
                q = q->next;
                pbuf_index = 0;
            }
        }
        /* One byte could still be unsent */
        if (word_index == 1)
        {
            DM9000_outw(DM9000_DATA_BASE, word[0]);
        }
    }

    if (dm9000_device.packet_cnt == 0)
    {
        DM9000_TRACE("dm9000 tx: first packet \r\n");

        dm9000_device.packet_cnt ++;
        /* Set TX length to DM9000 */
        dm9000_io_write(DM9000_TXPLL, p->tot_len & 0xff);
        dm9000_io_write(DM9000_TXPLH, (p->tot_len >> 8) & 0xff);

        /* Issue TX polling command */
        dm9000_io_write(DM9000_TCR, TCR_TXREQ);	/* Cleared after TX complete */
    }
    else
    {
        DM9000_TRACE("dm9000 tx: second packet \r\n");

        dm9000_device.packet_cnt ++;
        dm9000_device.queue_packet_len = p->tot_len;
    }

    /* enable dm9000a interrupt */
    dm9000_io_write(DM9000_IMR, dm9000_device.imr_all);

   /* unlock DM9000 device */
	OSSemPost(dm9000_sem_lock);	     /* 发送信号量 */

    /* wait ack */
    OSSemPend(dm9000_sem_ack,0,&err);	  /* 获取信号量 */
	  
    DM9000_TRACE("dm9000 tx done \r\n");

    return 0;
}

/*******************************************************************************
* Function Name  : dm9000_rx
* Description    : reception packet
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
struct pbuf *dm9000_rx(void)
{
    struct pbuf* p;
    uint32_t rxbyte;
    uint16_t rx_status, rx_len;
    uint16_t* data;
    INT8U err;
    /* init p pointer */
    p = NULL;

     /* lock DM9000 */
    OSSemPend(dm9000_sem_lock,0,&err);

__error_retry:
    /* Check packet ready or not */
    dm9000_io_read(DM9000_MRCMDX);	    		/* Dummy read */
    rxbyte = DM9000_inb(DM9000_DATA_BASE);		/* Got most updated data */
    if (rxbyte)
    {
        if (rxbyte > 1)
        {
            DM9000_TRACE("dm9000 rx: rx error, stop device \r\n");

            dm9000_io_write(DM9000_RCR, 0x00);	/* Stop Device */
            dm9000_io_write(DM9000_ISR, 0x80);	/* Stop INT request */
        }

        /* A packet ready now  & Get status/length */
        DM9000_outb(DM9000_IO_BASE, DM9000_MRCMD);

        rx_status = DM9000_inw(DM9000_DATA_BASE);
        rx_len = DM9000_inw(DM9000_DATA_BASE);

        DM9000_TRACE("dm9000 rx: status %04x len %d \r\n", rx_status, rx_len);

        /* allocate buffer */
        p = pbuf_alloc(PBUF_LINK, rx_len, PBUF_RAM);
        if (p != NULL)
        {
            struct pbuf* q;
            int32_t len;

            for (q = p; q != NULL; q= q->next)
            {
                data = (uint16_t*)q->payload;
                len = q->len;

                while (len > 0)
                {
                    *data = DM9000_inw(DM9000_DATA_BASE);
                    data ++;
                    len -= 2;
                }
            }
        }
        else
        {
            uint16_t dummy;

            DM9000_TRACE("dm9000 rx: no pbuf \r\n");

            /* no pbuf, discard data from DM9000 */
            data = &dummy;
            while (rx_len)
            {
                *data = DM9000_inw(DM9000_DATA_BASE);
                rx_len -= 2;
            }
        }

        if ( (rx_status & 0xbf00) || (rx_len < 0x40) || (rx_len > DM9000_PKT_MAX) )
        {
            DM9000_TRACE("rx error: status %04x, rx_len: %d \r\n", rx_status, rx_len);

            if (rx_status & 0x100)
            {
                DM9000_TRACE("rx fifo error \r\n");
            }
            if (rx_status & 0x200)
            {
                DM9000_TRACE("rx crc error \r\n");
            }
            if (rx_status & 0x8000)
            {
                DM9000_TRACE("rx length error \r\n");
            }
            if (rx_len > DM9000_PKT_MAX)
            {
                DM9000_TRACE("rx length too big \r\n");

                /* RESET device */
                dm9000_io_write(DM9000_NCR, NCR_RST);
                delay_ms(5); /* delay 5ms */
            }

            /* it issues an error, release pbuf */
            if (p != NULL) 
			{
			    pbuf_free(p);
			}
            p = NULL;

            goto __error_retry;
        }
    }
	else
    {
        /* clear packet received latch status */
        dm9000_io_write(DM9000_ISR, ISR_PTS);

        /* restore receive interrupt */
        dm9000_device.imr_all = IMR_PAR | IMR_PTM | IMR_PRM;
        dm9000_io_write(DM9000_IMR, dm9000_device.imr_all);
    }
    /* unlock DM9000 */
    OSSemPost(dm9000_sem_lock);

    return p;
}

/*******************************************************************************
* Function Name  : EXTI2_IRQHandler
* Description    : This function handles External lines 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void EXTI2_IRQHandler(void)
{
	CPU_SR         cpu_sr;
	OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
    OSIntNesting++;
    OS_EXIT_CRITICAL();

    EXTI_ClearITPendingBit(EXTI_Line2);

	while( GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)==0 )
	{
	    dm9000_isr();
	}

    OSIntExit();                                 /* Tell uC/OS-II that we are leaving the ISR          */
}

/*******************************************************************************
* Function Name  : dm9000_configuration
* Description    : configures dm9000
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void dm9000_configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
	/* enable gpiob port clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);

    /* configure PE2 as external interrupt */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /* Connect DM9000 EXTI Line to GPIOE Pin 2 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource2);

    /* Configure DM9000 EXTI Line to generate an interrupt on falling edge */
    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Clear DM9000A EXTI line pending bit */
    EXTI_ClearITPendingBit(EXTI_Line2);	

    /* Enable the EXTI2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);    
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
    GPIO_InitTypeDef GPIO_InitStructure;
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  FSMC_NORSRAMTimingInitStructure;

	/* FSMC GPIO configure */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF |
						   RCC_APB2Periph_GPIOG | RCC_APB2Periph_AFIO, ENABLE);
    
	/* PD14 FSMC_D0   PD15 FSMC_D1   PD0  FSMC_D2   PD1  FSMC_D3 */	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
       
    /* PE7 ~ PE15  FSMC_D4 ~ FSMC_D12 */  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
                                  GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOE,&GPIO_InitStructure);

    /* FSMC_D13 ~ FSMC_D15   PD8 ~ PD10 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_Init(GPIOD,&GPIO_InitStructure);

    /* FSMC_A0 ~ FSMC_A5   FSMC_A6 ~ FSMC_A9 */
    /* PF0     ~ PF5       PF12    ~ PF15 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 |
                                  GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOF,&GPIO_InitStructure);

    /* FSMC_A10 ~ FSMC_A15  PG0 ~ PG5 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_Init(GPIOG,&GPIO_InitStructure);

    /* FSMC_A16 ~ FSMC_A18  PD11 ~ PD13 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_Init(GPIOD,&GPIO_InitStructure);

    /* FSMC_A19 ~ FSMC_A22 PE3~PE6 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_Init(GPIOE,&GPIO_InitStructure);

    /* RD-PD4 WR-PD5 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_Init(GPIOD,&GPIO_InitStructure);

    /* NBL0-PE0 NBL1-PE1 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_Init(GPIOE,&GPIO_InitStructure);

    /* NE1/NCE2 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOD,&GPIO_InitStructure);
    /* NE2 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOG,&GPIO_InitStructure);
    /* NE3 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOG,&GPIO_InitStructure);
    /* NE4 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOG,&GPIO_InitStructure);

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
* Function Name  : hw_dm9000_init
* Description    : hardware initialize 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void hw_dm9000_init(void)
{
    FSMC_Configuration();
	dm9000_configuration();

    dm9000_device.type  = TYPE_DM9000A;
    dm9000_device.mode	= DM9000_AUTO;
    dm9000_device.packet_cnt = 0;
    dm9000_device.queue_packet_len = 0;

    /* SRAM Tx/Rx pointer automatically return to start address */ 
	/* Packet Transmitted, Packet Received */
    dm9000_device.imr_all = IMR_PAR | IMR_PTM | IMR_PRM;

    /* set mac address: (only for test) */
    dm9000_device.dev_addr[0] = emacETHADDR0;
    dm9000_device.dev_addr[1] = emacETHADDR1;
    dm9000_device.dev_addr[2] = emacETHADDR2;
    dm9000_device.dev_addr[3] = emacETHADDR3;
    dm9000_device.dev_addr[4] = emacETHADDR4;
    dm9000_device.dev_addr[5] = emacETHADDR5;

    dm9000_sem_ack = OSSemCreate(0);

	dm9000_sem_lock = OSSemCreate(1);
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
