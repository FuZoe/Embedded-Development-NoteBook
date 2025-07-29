/*
*********************************************************************************************************
*                                               uC/OS-View
*
*                                 (c) Copyright 2005, Micrium, Weston, FL
*                                           All Rights Reserved
*
*                                            ST STM32 (Cortex-M3)
*                                             IAR C Compiler
*
*
* Filename   : OS_VIEWc.H
* Version    : V1.21.1
* Programmer : ZMT
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                              CONSTANTS
*********************************************************************************************************
*/
#define  OS_VIEW_PARSE_TASK              DEF_ENABLED           	/* Parsing of received packets will be done by a task   */

#define  OS_VIEW_BAUDRATE				 115200u	    /* RS232 communications baud rate between the windows application and embeded system  */

#define  OS_VIEW_USART				     USART1	    


/*
*********************************************************************************************************
*                                         uC/OS-View CONFIGURATION
*********************************************************************************************************
*/

#define  OS_VIEW_RX_BUF_SIZE             128u	  	/* Rx buffer size (should not need to change)       */
#define  OS_VIEW_TX_BUF_SIZE             128u   	/* Tx buffer size ( 64~255 should not need to change)       */
#define  OS_VIEW_TX_STR_SIZE             128u    	/* Tx string size ( 64~255 should not need to change)       */




