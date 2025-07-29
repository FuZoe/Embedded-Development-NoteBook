/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               LWIP.c
** Descriptions:            None
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2011-3-10
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
#include "GLCD.h"
#include "lwIP.h"
#include "lwIP/tcp.h"
#include "lwIP/udp.h"
#include "lwIP/dhcp.h"
#include "lwip/memp.h"
#include "ethernetif.h"
#include "lwIP/tcpip.h"
#include "netif/etharp.h"
#include "arch/sys_arch.h"
#include <includes.h>
/* Private define ------------------------------------------------------------*/
#define MAX_DHCP_TRIES        4
#define	MAX_X	             DISP_HOR_RESOLUTION
#define	MAX_Y	             DISP_VER_RESOLUTION

/* Private variables ---------------------------------------------------------*/
struct netif _netif;
static uint32_t IPaddress = 0;

/* Private function prototypes -----------------------------------------------*/
static void list_if                (void);
static void TcpipInitDone          (void *arg);
static void USART_Configuration    (void);

/*******************************************************************************
* Function Name  : list_if
* Description    : display ip address in serial port debug windows
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void list_if(void)
{
    printf("Default network interface: %c%c \r\n", _netif.name[0], _netif.name[1]);
    printf("ip address: %s \r\n", inet_ntoa(*((struct in_addr*)&(_netif.ip_addr))));
    printf("gw address: %s \r\n", inet_ntoa(*((struct in_addr*)&(_netif.gw))));
    printf("net mask  : %s \r\n", inet_ntoa(*((struct in_addr*)&(_netif.netmask))));
}

/*******************************************************************************
* Function Name  : TcpipInitDone
* Description    : TcpipInitDone wait for tcpip init being done
* Input          : - arg: the semaphore to be signaled
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void TcpipInitDone(void *arg)
{
    sys_sem_t *sem;
    sem = arg;
    sys_sem_signal(*sem);
}

/*******************************************************************************
* Function Name  : Init_lwIP
* Description    : Init_lwIP initialize the LwIP
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void Init_lwIP(void)
{
    struct ip_addr ipaddr;
    struct ip_addr netmask;
    struct ip_addr gw;

    sys_sem_t sem;
    
    sys_init();
    
    /* Initializes the dynamic memory heap defined by MEM_SIZE.*/
    mem_init();
    
    /* Initializes the memory pools defined by MEMP_NUM_x.*/
    memp_init();
    
    pbuf_init();	
    netif_init();
    
	USART_Configuration();

    printf("TCP/IP initializing... \r\n");
    sem = sys_sem_new(0);
    tcpip_init(TcpipInitDone, &sem);
    sys_sem_wait(sem);
    sys_sem_free(sem);
    printf("TCP/IP initialized. \r\n");
      
#if LWIP_DHCP
    /* 启用DHCP服务器 */
    ipaddr.addr = 0;
    netmask.addr = 0;
    gw.addr = 0;
#else
    /* 启用静态IP */
    IP4_ADDR(&ipaddr, emacIPADDR0, emacIPADDR1, emacIPADDR2, emacIPADDR3 );
    IP4_ADDR(&netmask,emacNET_MASK0, emacNET_MASK1, emacNET_MASK2, emacNET_MASK3 );
    IP4_ADDR(&gw, emacGATEWAY_ADDR0, emacGATEWAY_ADDR1, emacGATEWAY_ADDR2, emacGATEWAY_ADDR3 );
#endif
    
    netif_add(&_netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
    netif_set_default(&_netif);
    
#if LWIP_DHCP
    dhcp_start(&_netif);
#endif
    netif_set_up(&_netif);

	list_if();
}

/*******************************************************************************
* Function Name  : Display_IPAddress
* Description    : Display_IPAddress Display IP Address
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void Display_IPAddress(void)
{
    if(IPaddress != _netif.ip_addr.addr)
    {   
	    /* IP 地址发生改变*/
        __IO uint8_t iptab[4];
        uint8_t iptxt[20];
        
        /* read the new IP address */
        IPaddress = _netif.ip_addr.addr;
        
        iptab[0] = (uint8_t)(IPaddress >> 24);
        iptab[1] = (uint8_t)(IPaddress >> 16);
        iptab[2] = (uint8_t)(IPaddress >> 8);
        iptab[3] = (uint8_t)(IPaddress);
        
        sprintf((char*)iptxt, "   %d.%d.%d.%d    ", iptab[3], iptab[2], iptab[1], iptab[0]);
                
        /* Display the new IP address */
#if LWIP_DHCP
        if(netif.flags & NETIF_FLAG_DHCP)
        {   
		    /* IP地址由DHCP指定 */
            /* Display the IP address */
		    GUI_Text( ( MAX_X - ( strlen("  IP assigned   ") * 8 ) ) / 2 , MAX_Y/2 - 40,  "  IP assigned   ", White, Black);
		    GUI_Text( ( MAX_X - ( strlen("by DHCP server  ") * 8 ) ) / 2 , MAX_Y/2 - 20,  " by DHCP server ", White, Black);
		    GUI_Text( ( MAX_X - ( strlen((void*)iptxt) * 8 ) ) / 2       , MAX_Y/2     ,       iptxt        , White, Black);	
		    GUI_Text( ( MAX_X - ( strlen("                ") * 8 ) ) / 2 , MAX_Y/2 + 20,  "                ", White, Black);
        }
        else
#endif  
        /* 静态IP地址 */
        {   
		    /* Display the IP address */
		    GUI_Text( ( MAX_X - ( strlen(" Static IP Addr ") * 8 ) ) / 2 , MAX_Y/2 - 40,  " Static IP Addr ", White, Black);
		    GUI_Text( ( MAX_X - ( strlen("                ") * 8 ) ) / 2 , MAX_Y/2 - 20,  "                ", White, Black);
		    GUI_Text( ( MAX_X - ( strlen((void*)iptxt) * 8 ) ) / 2       , MAX_Y/2     ,       iptxt        , White, Black);
		    GUI_Text( ( MAX_X - ( strlen("                ") * 8 ) ) / 2 , MAX_Y/2 + 20,  "                ", White, Black);

        }
    }
#if LWIP_DHCP
    else if(IPaddress == 0)
    {   
	    /* 等待DHCP分配IP */
		GUI_Text( ( MAX_X - ( strlen("   Looking for  ") * 8 ) ) / 2 , MAX_Y/2 - 40,  "   Looking for  ", White, Black);
		GUI_Text( ( MAX_X - ( strlen("   DHCP server  ") * 8 ) ) / 2 , MAX_Y/2 - 20,  "   DHCP server  ", White, Black);
		GUI_Text( ( MAX_X - ( strlen(" please wait... ") * 8 ) ) / 2 , MAX_Y/2     ,  " please wait... ", White, Black);
		GUI_Text( ( MAX_X - ( strlen("                ") * 8 ) ) / 2 , MAX_Y/2 + 20,  "                ", White, Black);
        
        /* If no response from a DHCP server for MAX_DHCP_TRIES times */
        /* stop the dhcp client and set a static IP address */
        if(netif.dhcp->tries > MAX_DHCP_TRIES) 
        {   
		    /* 超出DHCP重试次数，改用静态IP设置 */
            struct ip_addr ipaddr;
            struct ip_addr netmask;
            struct ip_addr gw;
            
		    GUI_Text( ( MAX_X - ( strlen("  DHCP timeout  ") * 8 ) ) / 2, MAX_Y/2 + 20, "  DHCP timeout  ", White, Black);

            dhcp_stop(&netif);
            
            IP4_ADDR(&ipaddr, emacIPADDR0, emacIPADDR1, emacIPADDR2, emacIPADDR3 );
            IP4_ADDR(&netmask,emacNET_MASK0, emacNET_MASK1, emacNET_MASK2, emacNET_MASK3 );
            IP4_ADDR(&gw, emacGATEWAY_ADDR0, emacGATEWAY_ADDR1, emacGATEWAY_ADDR2, emacGATEWAY_ADDR3 );
            
            netif_set_addr(&netif, &ipaddr , &netmask, &gw);
            
            list_if();
        }
    }
#endif
}

/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configure USART1 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void USART_Configuration(void)
{ 
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure; 

  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);
  /*
  *  USART1_TX -> PA9 , USART1_RX ->	PA10
  */				
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);		   

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	        
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART1, &USART_InitStructure); 
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  USART_ClearFlag(USART1,USART_FLAG_TC);
  USART_Cmd(USART1, ENABLE);
}


#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

