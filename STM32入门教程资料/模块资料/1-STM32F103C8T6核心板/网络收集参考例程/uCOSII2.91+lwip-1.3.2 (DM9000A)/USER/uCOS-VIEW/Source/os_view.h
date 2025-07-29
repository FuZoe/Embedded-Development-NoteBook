/*
***********************************************************************************************
*                                           uC/OS-View
*
*                            (c) Copyright 2005, Micrium, Weston, FL
*                                       All Rights Reserved
*
*
* Filename   : OS_VIEW.H
* Version    : V1.33
* Programmer : JJL
***********************************************************************************************
*/

#ifdef   OS_VIEW_GLOBALS
#define  OS_VIEW_EXT
#else
#define  OS_VIEW_EXT  extern
#endif


#ifndef  OS_VIEW_H
#define  OS_VIEW_H

/*
***********************************************************************************************
*                                         VARIABLES
***********************************************************************************************
*/

OS_VIEW_EXT  INT32U             OSView_CyclesCtr;
#if OS_VIEW_TMR_32_BITS > 0
OS_VIEW_EXT  INT32U             OSView_TmrCntsPrev;
#else
OS_VIEW_EXT  INT16U             OSView_TmrCntsPrev;
#endif

OS_VIEW_EXT  INT8U              OSView_RxState;
OS_VIEW_EXT  INT8U              OSView_RxRemainLen;
OS_VIEW_EXT  INT8U              OSView_RxChkSum;
OS_VIEW_EXT  INT8U              OSView_RxBuf[OS_VIEW_RX_BUF_SIZE];
OS_VIEW_EXT  INT8U              OSView_RxBufCnt;
OS_VIEW_EXT  INT8U              OSView_RxRdIx;
OS_VIEW_EXT  INT32U             OSView_RxCtr;
OS_VIEW_EXT  INT16U             OSView_RxPktCtr;

OS_VIEW_EXT  INT16U             OSView_TxLen;
OS_VIEW_EXT  INT16U             OSView_TxIx;
OS_VIEW_EXT  INT8U              OSView_TxChkSum;
OS_VIEW_EXT  INT8U              OSView_TxState;
OS_VIEW_EXT  BOOLEAN            OSView_TxActiveFlag;
OS_VIEW_EXT  INT8U              OSView_TxBuf[OS_VIEW_TX_BUF_SIZE];
OS_VIEW_EXT  INT16U             OSView_TxBufCnt;
OS_VIEW_EXT  INT8U             *OSView_TxBufPtr;
OS_VIEW_EXT  INT32U             OSView_TxCtr;
OS_VIEW_EXT  INT16U             OSView_TxPktCtr;

OS_VIEW_EXT  INT16U             OSView_TxStrLen;
OS_VIEW_EXT  INT8U              OSView_TxStrBuf[OS_VIEW_TX_STR_SIZE];
OS_VIEW_EXT  INT16U             OSView_TxStrDlyCtr;

OS_VIEW_EXT  void             (*OSView_TerminalRxCallbackFnct)(INT8U data);

/*
***********************************************************************************************
*                                        PROTOTYPES
*                                    (PORT INDEPENDENT)
***********************************************************************************************
*/

void    OSView_Init(INT32U baud_rate);              /* Initialize uC/OS-View                          */

void    OSView_RxHandler(INT8U data);               /* Processor independent Rx handler               */
void    OSView_TxHandler(void);                     /* Processor independent Tx handler               */

void    OSView_TxStr(INT8U *s, INT16U dly);         /* Send an ASCII string to terminal window        */

void    OSView_TaskSwHook(void);                    /* MUST be called by OSTaskSwHook()               */
void    OSView_TaskCreateHook(OS_TCB *ptcb);        /* MUST be called by OSTaskCreateHook()           */
void    OSView_TickHook(void);                      /* MUST be called by OSTimeTickHook()             */

void    OSView_Exit(void);                          /* Exit uC/OS-View                                */

void    OSView_InitTarget(INT32U baud_rate);        /* Initialize the target specifics                */

void    OSView_TerminalRxSetCallback(void (*call_back)(INT8U data));
void    OSView_TerminalTx(INT8U data);              /* Write data to output window                    */

INT32U  OSView_TimeGetCycles(void);                 /* Function to read fine resolution time base     */

void    OSView_RxTxISR(void);                       /* Assembly Language Rx/Tx ISR                    */
void    OSView_RxTxISRHandler(void);                /* C code to process Rx or Tx character           */

void    OSView_RxISR(void);                         /* Assembly Language Rx ISR                       */
void    OSView_RxISRHandler(void);                  /* C code to process character received           */
void    OSView_RxIntEn(void);                       /* Enable  Rx interrupts                          */
void    OSView_RxIntDis(void);                      /* Disable Rx interrupts                          */

void    OSView_TxISR(void);                         /* Assembly Language Tx ISR                       */
void    OSView_TxISRHandler(void);                  /* C code to process next character to send       */
void    OSView_TxIntEn(void);                       /* Enable  Tx interrupts                          */
void    OSView_TxIntDis(void);                      /* Disable Tx interrupts                          */
void    OSView_Tx1(INT8U c);                        /* Function to send a single character            */

void    OSView_GetCPUName(INT8U *s);                /* Returns the name of the CPU used               */
INT32U  OSView_GetIntStkBase(void);                 /* Get the address of the interrupt stack base    */
INT32U  OSView_GetIntStkSize(void);                 /* Get the address of the interrupt stack size    */


/*
*********************************************************************************************************
*                                      BSP FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void     OSView_TmrInit(void);                      /* Initialize the free running timer               */
INT32U   OSView_TmrRd(void);                        /* Read       the free running timer               */

/*
***********************************************************************************************
*                                         #error DIRECTIVES
***********************************************************************************************
*/

#if     OS_VIEW_PARSE_TASK > 0 && (OS_SEM_EN == 0 && OS_MBOX_EN == 0)
#error  "Semaphore or Mailbox management must be enabled.  In other words, OS_SEM_EN or OS_MBOX_EN must be set to 1."
#endif

#if     OS_TIME_GET_SET_EN == 0
#error  "OS_CFG.H, OS_TIME_GET_SET_EN needs to be set to 1 to include code for OSTimeGet()"
#endif

#if     OS_TASK_CREATE_EXT_EN == 0
#error  "OS_CFG.H, OS_TASK_CREATE_EXT_EN needs to be set to 1 to include code for OSTaskCreateExt()"
#endif

#if     OS_TASK_STAT_EN == 0
#error  "OS_CFG.H, OS_TASK_STAT_EN needs to be set to 1 to include code for the statistics task"
#endif

#if     OS_TASK_STAT_STK_CHK_EN == 0
#error  "OS_CFG.H, OS_TASK_STAT_STK_CHK_EN needs to be set to 1 to include code for task stack checking"
#endif

#if     OS_TASK_PROFILE_EN == 0
#error  "OS_CFG.H, OS_TASK_PROFILE_EN needs to be set to 1 to enable task profiling"
#endif

#if     OS_MAX_TASKS > 61
#error  "OS_CFG.H, OS_MAX_TASKS cannot be higher than 61 when using uC/OS-View"
#endif

#if     OS_SEM_EN == 0
#error  "OS_CFG.H, uC/OS-View requires to you enable semaphores, set OS_SEM_EN to 1"
#endif

#if     OS_VIEW_TMR_32_BITS > 1
#error  "OS_VIEW_TMR_32_BITS 'must' be defined to either 0 (for 16 bit timer) or 1 (for 32 bit timer)"
#endif

#endif
