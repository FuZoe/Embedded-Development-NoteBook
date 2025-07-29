/*
*********************************************************************************************************
*                                           uC/OS-View
*
*                             (c) Copyright 2005, Micrium, Weston, FL
*                                       All Rights Reserved
*
*
* Filename   : OS_VIEW.C
* Version    : V1.33
* Programmer : JJL
*********************************************************************************************************
*/

#define  OS_VIEW_GLOBALS
#include <includes.h>

#if OS_VIEW_MODULE == DEF_ENABLED

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#ifndef  OS_VIEW_SUPPORT_MEM
#define  OS_VIEW_SUPPORT_MEM               0                /* Current version doesn't support memory Rd/Wr    */
#endif
                                                            /* Inbound  packets (from PC)                      */
#define  OS_VIEW_PROTOCOL_RX_SD0        0xED                /* Start delimiters                                */
#define  OS_VIEW_PROTOCOL_RX_SD1        0x8C
#define  OS_VIEW_PROTOCOL_RX_ED         0x8D                /* End   delimiter                                 */

                                                            /* Outbound packets (to PC)                        */
#define  OS_VIEW_PROTOCOL_TX_SD0        0x8C                /* Start delimiters                                */
#define  OS_VIEW_PROTOCOL_TX_SD1        0xED
#define  OS_VIEW_PROTOCOL_TX_ED         0x8D                /* End   delimiter                                 */


#define  OS_VIEW_RX_STATE_SD0              0                /* waiting for start first  start delimiter (SD0)  */
#define  OS_VIEW_RX_STATE_SD1              1                /* waiting for start second start delimiter (SD1)  */
#define  OS_VIEW_RX_STATE_LEN              2                /* waiting for len byte                            */
#define  OS_VIEW_RX_STATE_DATA             3                /* waiting for data                                */
#define  OS_VIEW_RX_STATE_CHKSUM           4                /* waiting for checksum                            */
#define  OS_VIEW_RX_STATE_ED               5                /* waiting for end delimiter                       */

#define  OS_VIEW_TX_STATE_SD0              0                /* Transmit state machine states                   */
#define  OS_VIEW_TX_STATE_SD1              1
#define  OS_VIEW_TX_STATE_DATA_LEN         2
#define  OS_VIEW_TX_STATE_DATA             3
#define  OS_VIEW_TX_STATE_CHKSUM           4
#define  OS_VIEW_TX_STATE_ED               5
#define  OS_VIEW_TX_STATE_STR_LEN          6
#define  OS_VIEW_TX_STATE_STR_TYPE         7
#define  OS_VIEW_TX_STATE_STR_DATA         8

#define  OS_VIEW_CMD_STEP_TICK_ON          0                /* Commands for tick stepping                      */
#define  OS_VIEW_CMD_STEP_TICK_OFF         1
#define  OS_VIEW_CMD_STEP_TICK_NEXT        2

#define  OS_VIEW_CPU_NAME_SIZE            30

/*$PAGE*/
/*
*********************************************************************************************************
*                                           LOCAL VARIABLES
*********************************************************************************************************
*/

#if OS_VIEW_PARSE_TASK > 0
static  OS_STK     OSView_TaskStk[OS_VIEW_TASK_STK_SIZE];

#if OS_SEM_EN > 0
static  OS_EVENT  *OSView_Sem;
#endif

#endif


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

#if OS_VIEW_PARSE_TASK > 0
static  void    OSView_InitOS(void);
static  void    OSView_Task(void *p_arg);
#endif

static  INT8U   OSView_CmdParsePkt(void);
static  void    OSView_CmdSendToApp(void);
static  void    OSView_CmdGetSysInfo(void);
static  void    OSView_CmdGetTaskList(void);
static  void    OSView_CmdGetTaskInfo(void);
static  void    OSView_CmdGetTaskInfo_InvalidTask(void);
static  INT8U   OSView_CmdGetTaskInfo_TCBStat(OS_TCB *ptcb);
static  void    OSView_CmdMemRdINT8U(void);
static  void    OSView_CmdMemRdINT16U(void);
static  void    OSView_CmdMemRdINT32U(void);
static  void    OSView_CmdMemWrINT8U(void);
static  void    OSView_CmdMemWrINT16U(void);
static  void    OSView_CmdMemWrINT32U(void);
static  void    OSView_CmdTickStep(void);
static  void    OSView_CmdTrace(void);

static  void    OSView_RxPkt(void);
static  void    OSView_RxStoINT8U(INT8U rx_data);
static  void    OSView_RxBufClr(void);
static  INT8U   OSView_RxGetINT8U(void);
static  INT16U  OSView_RxGetINT16U(void);
static  INT32U  OSView_RxGetINT32U(void);

static  void    OSView_TxStart(void);
static  void    OSView_TxStoINT8U(INT8U tx_data);
static  void    OSView_TxStoINT16U(INT16U tx_data);
static  void    OSView_TxStoINT32U(INT32U tx_data);
static  void    OSView_TxStoPtr(void *ptr);
static  void    OSView_TxStoStr(INT8U *s);

/*$PAGE*/
/*
*********************************************************************************************************
*                                       INITIALIZE uC/OS-View
*********************************************************************************************************
*/

void  OSView_Init (INT32U baud_rate)
{
#if OS_TICK_STEP_EN > 0
    OSTickStepState     = OS_TICK_STEP_DIS;                 /* Disable the tick stepping mode                  */
#endif

    OSView_CyclesCtr    = 0;

    OSView_RxState      = OS_VIEW_RX_STATE_SD0;             /* Setup Rx and Tx state machines                  */
    OSView_TxState      = OS_VIEW_TX_STATE_SD0;

    OSView_TxLen        = 0;
    OSView_TxStrLen     = 0;
    OSView_TxActiveFlag = OS_FALSE;

    OSView_TerminalRxSetCallback((void (*)(INT8U))0);       /* Force terminal callback function to 'nothing'   */

    OSView_TmrCntsPrev  = 0;

#if OS_VIEW_PARSE_TASK > 0
    OSView_InitOS();
#endif

    OSView_InitTarget(baud_rate);                           /* Initialize target specific code                 */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                             INITIALIZE THE uC/OS-View TASK AND OS OBJECTS
*********************************************************************************************************
*/

#if OS_VIEW_PARSE_TASK > 0
static  void  OSView_InitOS (void)
{
#if OS_TASK_NAME_EN > 0 || OS_EVENT_NAME_SIZE > 7
    INT8U  err;
#endif


#if OS_SEM_EN > 0
    OSView_Sem = OSSemCreate(0);
#if OS_EVENT_NAME_EN > 0
    OSEventNameSet(OSView_Sem, (INT8U *)"uC/OS-View Signal", &err);
#endif
#endif

#if OS_TASK_CREATE_EXT_EN > 0
    #if OS_STK_GROWTH == 1
    (void)OSTaskCreateExt(OSView_Task,
                          (void *)0,                                  /* No arguments passed to OSView_Task() */
                          &OSView_TaskStk[OS_VIEW_TASK_STK_SIZE - 1], /* Set Top-Of-Stack                     */
                          OS_VIEW_TASK_PRIO,                          /* Lowest priority level                */
                          OS_VIEW_TASK_PRIO,
                          &OSView_TaskStk[0],                         /* Set Bottom-Of-Stack                  */
                          OS_VIEW_TASK_STK_SIZE,
                          (void *)0,                                  /* No TCB extension                     */
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR); /* Enable stack checking + clear stack  */
    #else
    (void)OSTaskCreateExt(OSView_Task,
                          (void *)0,                                  /* No arguments passed to OSView_Task() */
                          &OSView_TaskStk[0],                         /* Set Top-Of-Stack                     */
                          OS_VIEW_TASK_PRIO,                          /* Lowest priority level                */
                          OS_VIEW_TASK_PRIO,
                          &OSView_TaskStk[OS_VIEW_TASK_STK_SIZE - 1], /* Set Bottom-Of-Stack                  */
                          OS_VIEW_TASK_STK_SIZE,
                          (void *)0,                                  /* No TCB extension                     */
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR); /* Enable stack checking + clear stack  */
    #endif

#else
    #if OS_STK_GROWTH == 1
    (void)OSTaskCreate(OSView_Task,
                       (void *)0,
                       &OSView_TaskStk[OS_VIEW_TASK_STK_SIZE - 1],
                       OS_VIEW_TASK_PRIO);
    #else
    (void)OSTaskCreate(OSView_Task,
                       (void *)0,
                       &OSView_TaskStk[0],
                       OS_VIEW_TASK_PRIO);
    #endif
#endif

#if OS_TASK_NAME_EN > 0
    OSTaskNameSet(OS_VIEW_TASK_PRIO, (INT8U *)"uC/OS-View", &err);
#endif
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                           uC/OS-View TASK
*********************************************************************************************************
*/

#if OS_VIEW_PARSE_TASK > 0
static  void  OSView_Task (void *p_arg)
{
    INT16U     len;
    INT8U      err;
#if OS_SEM_EN == 0 && OS_MBOX_EN > 0
    void      *msg;
#endif
#if OS_CRITICAL_METHOD == 3                       /* Allocate storage for CPU status register          */
    OS_CPU_SR  cpu_sr = 0;
#endif


    (void)p_arg;
    while (1) {
#if OS_SEM_EN > 0
        (void)OSSemPend(OSView_Sem, 0, &err);     /* Wait for a packet to be received                  */
#else
#if OS_MBOX_EN > 0
        (void)OSMboxPend(OSView_Mbox, 0, &err);   /* Wait for a packet to be received                  */
#endif
#endif
        len = OSView_CmdParsePkt();               /* Parse packet and formulate a response             */
        if (len > 0) {                            /* If we have a response                             */
            OS_ENTER_CRITICAL();                  /* Setup transmission of the response                */
            OSView_TxLen = len;
            OS_EXIT_CRITICAL();
            OSView_TxStart();
        }
    }
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                     uC/OS-View TASK CREATE HOOK
*
* Description: This function MUST be called by uC/OS-II's Task Create Hook function: OSTaskCreateHook().
*********************************************************************************************************
*/

void  OSView_TaskCreateHook (OS_TCB *ptcb)
{
    ptcb->OSTCBCyclesStart = OSView_TimeGetCycles();        /* Get the current start time for this task.       */
    ptcb->OSTCBCyclesTot   = 0;                             /* Update the task's total execution time          */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                     uC/OS-View TASK SWITCH HOOK
*
* Description: This function MUST be called by uC/OS-II's Task Switch Hook function: OSTaskSwHook().
*********************************************************************************************************
*/

void  OSView_TaskSwHook (void)
{
    INT32U  cycles;


    cycles                         = OSView_TimeGetCycles();    /* This task is done                           */
    OSTCBCur->OSTCBCyclesTot      += cycles - OSTCBCur->OSTCBCyclesStart;
    OSTCBHighRdy->OSTCBCyclesStart = cycles;                    /* Save absolute #cycles at task activation    */
}

/*
*********************************************************************************************************
*                                       Update 32-bits cycles counter
*
* Description: This function must be called by uC/OS-II's OSTimeTickHook() and is used to maintain a
*              32-bit counter of clock or other cycles.
*
* Returns    : None
*
* Note(s)    : Changes the global variable OSView_CyclesCtr
*********************************************************************************************************
*/

void  OSView_TickHook (void)
{
    (void)OSView_TimeGetCycles();
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                           Get time [cycles]
*
* Description: This routine is required for task execution time measurement.  This function needs to
*              return time as accurately as possible and in a 32-bit variable.
*
* Returns    : A 32-bit representation of time.
*********************************************************************************************************
*/

INT32U  OSView_TimeGetCycles (void)
{
    INT32U     cycles;
#if OS_VIEW_TMR_32_BITS > 0
    INT32U     cnts32;
    INT32U     cnts32_delta;
#else
    INT16U     cnts16;
    INT16U     cnts16_delta;
#endif
#if OS_CRITICAL_METHOD == 3                                    /* Allocate storage for CPU status register         */
    OS_CPU_SR  cpu_sr = 0;
#endif



    OS_ENTER_CRITICAL();
#if OS_VIEW_TMR_32_BITS > 0
    cnts32              = OSView_TmrRd();                      /* Read current counts of the free running counter  */
    cnts32_delta        = cnts32 - OSView_TmrCntsPrev;
    OSView_TmrCntsPrev  = cnts32;                              /* Save current counts for next time                */
    OSView_CyclesCtr   += cnts32_delta;
#else
    cnts16              = (INT16U)OSView_TmrRd();              /* Read current counts of the free running counter  */
    cnts16_delta        = cnts16 - OSView_TmrCntsPrev;
    OSView_TmrCntsPrev  = cnts16;                              /* Save current counts for next time                */
    OSView_CyclesCtr   += (INT32U)cnts16_delta;
#endif
    cycles              = OSView_CyclesCtr;
    OS_EXIT_CRITICAL();
    return (cycles);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                       uC/OS-View COMMAND HANDLER
*                                            (Parse Packet)
*********************************************************************************************************
*/

static  INT8U  OSView_CmdParsePkt (void)
{
    INT8U cmd0;


    cmd0 = OSView_RxGetINT8U();                             /* First byte contains command                     */
    if (cmd0 == 'C') {
        OSView_CmdSendToApp();
        OSView_TxBufCnt = 0;
        return (0);
    }
    if (OSView_TxBufCnt > 0) {                              /* Do cmds only if Tx of the last one is done      */
        return (0);
    }
    switch (cmd0) {
        case 's':
             OSView_TxStoINT8U(cmd0);
             OSView_CmdGetSysInfo();
             break;

        case 't':
             OSView_TxStoINT8U(cmd0);
             OSView_CmdGetTaskInfo();
             break;

        case 'l':
             OSView_TxStoINT8U(cmd0);
             OSView_CmdGetTaskList();
             break;

        case 'S':
             OSView_TxStoINT8U(cmd0);
             OSView_CmdTickStep();
             break;

        case 'T':
             OSView_TxStoINT8U(cmd0);
             OSView_CmdTrace();
             break;

        case 'b':
             OSView_TxStoINT8U(cmd0);
             OSView_CmdMemRdINT8U();
             break;

        case 'w':
             OSView_TxStoINT8U(cmd0);
             OSView_CmdMemRdINT16U();
             break;

        case 0:
             OSView_TxStoINT8U(cmd0);
             OSView_CmdMemRdINT32U();
             break;

        case '1':
             OSView_TxStoINT8U(cmd0);
             OSView_CmdMemWrINT8U();
             break;

        case '2':
             OSView_TxStoINT8U(cmd0);
             OSView_CmdMemWrINT16U();
             break;

        case '4':
             OSView_TxStoINT8U(cmd0);
             OSView_CmdMemWrINT32U();
             break;

        default:
             break;
    }
    return (OSView_TxBufCnt);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                       uC/OS-View COMMAND HANDLER
*                                         (Send to Application)
*
* Description: This routine transfers a character to the application.
*********************************************************************************************************
*/

static  void  OSView_CmdSendToApp (void)
{
    INT8U  rx_data;


    rx_data = OSView_RxGetINT8U();                               /* Read data typed at terminal                */
    if ((void *)OSView_TerminalRxCallbackFnct != (void *)0) {    /* See if callback is defined                 */
        (*OSView_TerminalRxCallbackFnct)(rx_data);               /* Pass character received to application     */
    }
}


/*
*********************************************************************************************************
*                                       uC/OS-View COMMAND HANDLER
*                                        (Get System Information)
*********************************************************************************************************
*/

static  void  OSView_CmdGetSysInfo (void)
{
    INT8U   s[OS_VIEW_CPU_NAME_SIZE+1];
    INT8U   i;
    INT32U  stk_base;
    INT32U  stk_size;
    INT32U  time;


    time = OSTimeGet();                                     /* Send current value of tick counter              */
    OSView_TxStoINT32U(time);
    OSView_TxStoPtr(OSTCBCur);                              /* Current task's OS_TCB                           */
    OSView_TxStoINT16U(OS_VERSION);                         /* Send uC/OS-II's version number                  */
    OSView_TxStoINT8U(1);                                   /* Indicate profiling information will be sent     */

    for (i = 0; i < OS_VIEW_CPU_NAME_SIZE; i++) {           /* Clear buffer to help with debug.                */
        s[i] = 0x00;
    }
    OSView_GetCPUName(s);                                   /* Get Processor Name in string 's'                */
    OSView_TxStoStr(s);

    stk_base = OSView_GetIntStkBase();                      /* Interrupt stack base from port specific file    */
    OSView_TxStoINT32U(stk_base);

    stk_size = OSView_GetIntStkSize();                      /* Interrupt stack size from port specific file    */
    OSView_TxStoINT32U(stk_size);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                       uC/OS-View COMMAND HANDLER
*                                            (Get Task List)
*********************************************************************************************************
*/

static  void  OSView_CmdGetTaskList (void)
{
    INT8U   prio;
    OS_TCB *ptcb;


    OSView_TxStoINT8U('t');                                 /* Identifier for data type                        */
    for (prio = 0; prio <= OS_LOWEST_PRIO; prio++) {
        ptcb = OSTCBPrioTbl[prio];
        if (ptcb != (OS_TCB *)0 && ptcb != (OS_TCB *)1) {   /* Does task exist? (Not a Mutex reserved task)    */
            OSView_TxStoINT32U((INT32U)ptcb);               /* Yes, send the task's TCB address                */
        }
    }
}

/*
*********************************************************************************************************
*                                       uC/OS-View COMMAND HANDLER
*                                         (Get Task Information)
*********************************************************************************************************
*/

static  void  OSView_CmdGetTaskInfo (void)
{
    OS_TCB  *ptcb;
    INT32U   cycles;
    INT8U    stat;


    ptcb   = (OS_TCB *)OSView_RxGetINT32U();                         /* Get the desired task's TCB address     */
    cycles = OSView_TimeGetCycles();                                 /* Get timestamp                          */
    OSView_TxStoINT32U(cycles);
    if (ptcb != (OS_TCB *)0 && ptcb != (OS_TCB *)1) {                /* Make sure we have a valid TCB pointer  */
        if (ptcb == OSTCBCur) {
            ptcb->OSTCBCyclesTot  += cycles - ptcb->OSTCBCyclesStart;
            ptcb->OSTCBCyclesStart = cycles;
        }
        OSView_TxStoStr(ptcb->OSTCBTaskName); 						 /* Task name                              */
        OSView_TxStoINT8U(ptcb->OSTCBPrio);                          /* Prio                                   */
        stat = OSView_CmdGetTaskInfo_TCBStat(ptcb);                  /* Task status                            */
        OSView_TxStoINT8U(stat);
        OSView_TxStoINT32U((INT32U)ptcb->OSTCBEventPtr);             /* Pointer to event task is waiting for.  */
        OSView_TxStoINT32U((INT32U)ptcb->OSTCBDly);                  /* Timeout (i.e. ticks of delayed task)   */
        OSView_TxStoINT32U(ptcb->OSTCBCyclesTot);                    /* ExecTime                               */
        OSView_TxStoINT32U(ptcb->OSTCBCtxSwCtr);                     /* NumActivations                         */

        OSView_TxStoINT32U((INT32U)ptcb->OSTCBStkBase);              /* Stack base address                     */
        OSView_TxStoINT32U(ptcb->OSTCBStkSize * sizeof(OS_STK));     /* Stack size (in #bytes)                 */
        OSView_TxStoINT32U(ptcb->OSTCBStkUsed);                      /* Number of bytes used                   */
        OSView_TxStoINT32U((INT32U)ptcb->OSTCBStkPtr);               /* Current stack pointer address          */
    } else {
        OSView_CmdGetTaskInfo_InvalidTask();
    }
    OSView_TxStoINT16U((INT16U)OSTimeGet());
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                       uC/OS-View COMMAND HANDLER
*                                             (Invalid Task)
*********************************************************************************************************
*/

static  void  OSView_CmdGetTaskInfo_InvalidTask (void)
{
    OSView_TxStoStr((INT8U *)"INVALID TASK!");		        /* Task name                                       */
    OSView_TxStoINT8U(0);                                   /* Prio                                            */
    OSView_TxStoINT8U(0);                                   /* Stat                                            */
    OSView_TxStoINT32U(0);                                  /* Data                                            */
    OSView_TxStoINT32U(0);                                  /* Timeout                                         */
    OSView_TxStoINT32U(0);                                  /* StackBase                                       */
    OSView_TxStoINT32U(0);                                  /* StackSize                                       */
    OSView_TxStoINT32U(0);                                  /* #Bytes used on stack                            */
    OSView_TxStoINT32U(0);                                  /* Current SP address                              */
    OSView_TxStoINT32U(0);                                  /* ExecTime                                        */
    OSView_TxStoINT32U(0);                                  /* NumActivations                                  */
}

/*
*********************************************************************************************************
*                                       uC/OS-View COMMAND HANDLER
*                                         (Get ptcb->OSTCBStat)
*********************************************************************************************************
*/

static  INT8U  OSView_CmdGetTaskInfo_TCBStat (OS_TCB *ptcb)
{
    INT8U stat;


    if (ptcb->OSTCBStat != OS_STAT_RDY) {
        stat = ptcb->OSTCBStat;
    } else {
        if (ptcb->OSTCBDly > 0) {                           /* Task status                                     */
            stat = 0xF0;
        } else {
            stat = ptcb->OSTCBStat;
        }
    }
    return (stat);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                       uC/OS-View COMMAND HANDLER
*                                              (Memory)
*********************************************************************************************************
*/

static  void  OSView_CmdMemRdINT8U (void)
{
    INT32U addr;
    INT8U  nbytes;


    addr   = OSView_RxGetINT32U();
    nbytes = OSView_RxGetINT8U();
    while (nbytes--) {
        OSView_TxStoINT8U(*(INT8U *)addr);
        addr++;
    }
}


static  void  OSView_CmdMemRdINT16U (void)
{
    INT32U addr;
    INT8U  nbytes;


    addr   = OSView_RxGetINT32U();
    nbytes = OSView_RxGetINT8U();
    while (nbytes--) {
        OSView_TxStoINT16U(*(INT16U *)addr);
        addr += 2;
    }
}


static  void  OSView_CmdMemRdINT32U (void)
{
    INT32U addr;
    INT8U  nbytes;


    addr   = OSView_RxGetINT32U();
    nbytes = OSView_RxGetINT8U();
    while (nbytes--) {
        OSView_TxStoINT32U(*(INT32U *)addr);
        addr += 4;
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                       uC/OS-View COMMAND HANDLER
*                                           (Write Functions)
*********************************************************************************************************
*/

static  void  OSView_CmdMemWrINT8U (void)
{
    INT8U *p;


    p    = (INT8U *)OSView_RxGetINT32U();
    (*p) = OSView_RxGetINT8U();
}


static  void  OSView_CmdMemWrINT16U (void)
{
    INT16U *p;


    p    = (INT16U *)OSView_RxGetINT32U();
    (*p) = OSView_RxGetINT16U();
}


static  void  OSView_CmdMemWrINT32U (void)
{
    INT32U *p;


    p    = (INT32U *)OSView_RxGetINT32U();
    (*p) = OSView_RxGetINT32U();
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                       uC/OS-View COMMAND HANDLER
*                                          (Tick Step command)
*********************************************************************************************************
*/

static  void  OSView_CmdTickStep (void)
{
#if OS_TICK_STEP_EN > 0
    INT8U sub_cmd;


    sub_cmd = OSView_RxGetINT8U();
    switch (sub_cmd) {
        case 0:                                             /* Enable stepping                                 */
             OSTickStepState = OS_TICK_STEP_WAIT;
             break;

        case 1:                                             /* Disable stepping, let tick ISR handle the ticks */
             OSTickStepState = OS_TICK_STEP_DIS;
             break;

        case 2:                                             /* Simulate the occurrence of one tick             */
             OSTickStepState = OS_TICK_STEP_ONCE;
             break;
    }
#endif
}

/*
*********************************************************************************************************
*                                       uC/OS-View COMMAND HANDLER
*                                             (Trace command)
*
* Note(s): Command not currently supported.
*********************************************************************************************************
*/

static  void  OSView_CmdTrace (void)
{
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                              Rx Handler
*
* Description: This routine is called from the Rx interrupt service handler.
*********************************************************************************************************
*/

void  OSView_RxHandler (INT8U rx_data)
{
    OSView_RxCtr++;
    switch (OSView_RxState) {
        case OS_VIEW_RX_STATE_SD0:                          /* waiting for start first  start delimiter (SD0)  */
             if (rx_data == OS_VIEW_PROTOCOL_RX_SD0) {
                 OSView_RxState = OS_VIEW_RX_STATE_SD1;
                 OSView_RxBufClr();
             }
             break;

        case OS_VIEW_RX_STATE_SD1:                          /* waiting for start second start delimiter (SD1)  */
             if (rx_data == OS_VIEW_PROTOCOL_RX_SD1) {
                 OSView_RxState = OS_VIEW_RX_STATE_LEN;
             } else {
                 OSView_RxState = OS_VIEW_RX_STATE_SD0;
             }
             break;

        case OS_VIEW_RX_STATE_LEN:                          /* waiting for 'len' byte                          */
             if ((rx_data == 0x00) | (rx_data > OS_VIEW_RX_BUF_SIZE)) {
                 OSView_RxState  = OS_VIEW_RX_STATE_SD0;    /* Can not handle this size ...                    */
             } else {
                 OSView_RxChkSum    = rx_data;
                 OSView_RxRemainLen = rx_data;
                 OSView_RxState     = OS_VIEW_RX_STATE_DATA;
             }
             break;

        case OS_VIEW_RX_STATE_DATA:                         /* waiting for data                                */
             OSView_RxStoINT8U(rx_data);
             OSView_RxChkSum += rx_data;
             if (--OSView_RxRemainLen == 0) {
                 OSView_RxState = OS_VIEW_RX_STATE_CHKSUM;
             }
             break;

        case OS_VIEW_RX_STATE_CHKSUM:                       /* waiting for checksum                            */
             OSView_RxChkSum += rx_data;
             if ((OSView_RxChkSum & 0xFF) == 0x00) {
                 OSView_RxState = OS_VIEW_RX_STATE_ED;
             } else {
                 OSView_RxState = OS_VIEW_RX_STATE_SD0;
             }
             break;

        case OS_VIEW_RX_STATE_ED:
             if (rx_data == OS_VIEW_PROTOCOL_RX_ED) {       /* End delimiter ?                                 */
                 OSView_RxPktCtr++;
                 OSView_RxPkt();                            /* Parse received packet                           */
             }
             OSView_RxState = OS_VIEW_RX_STATE_SD0;
             break;

        default:
             OSView_RxState = OS_VIEW_RX_STATE_SD0;
             break;
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        PARSE RECEIVED PACKET
*
* Description: This function is called when a packet has been received and needs to be processed.
*********************************************************************************************************
*/

static  void  OSView_RxPkt (void)
{
#if OS_VIEW_PARSE_TASK > 0
#if OS_SEM_EN > 0
    OSSemPost(OSView_Sem);                         /* We have a whole packet, signal task to parse it! */
#else
#if OS_MBOX_EN > 0
    OSMboxPost(OSView_Mbox, (void *)1);
#endif
#endif

#else
    INT16U   len;


    len = OSView_CmdParsePkt();                    /* We have a whole packet, parse it!                */
    if (len > 0) {
        OSView_TxLen = len;
        OSView_TxStart();
    }
#endif
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        STORE DATA IN Rx BUFFER
*
* Description: This routine is called whenever a valid byte has been received.
*********************************************************************************************************
*/

static  void  OSView_RxStoINT8U (INT8U rx_data)
{
    if (OSView_RxBufCnt < OS_VIEW_RX_BUF_SIZE) {
        OSView_RxBuf[OSView_RxBufCnt++] = rx_data;
    }
}


/*
*********************************************************************************************************
*                                           CLEAR Rx BUFFER
*********************************************************************************************************
*/

static  void  OSView_RxBufClr (void)
{
    OSView_RxBufCnt = 0;
    OSView_RxRdIx   = 0;
}

/*
*********************************************************************************************************
*                                     GET 8, 16 or 32-bit DATA
*********************************************************************************************************
*/

static  INT8U  OSView_RxGetINT8U (void)
{
    return (OSView_RxBuf[OSView_RxRdIx++]);
}


static  INT16U  OSView_RxGetINT16U (void)
{
    INT16U  lowbyte;
    INT16U  highbyte;


    highbyte = OSView_RxGetINT8U();
    lowbyte  = OSView_RxGetINT8U();
    return ((highbyte << 8) | lowbyte);
}


static  INT32U  OSView_RxGetINT32U (void)
{
    INT32U  highword;
    INT32U  lowword;


    highword = OSView_RxGetINT16U();
    lowword  = OSView_RxGetINT16U();
    return ((highword << 16) | lowword);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                  START TRANSMISSION OF REPLY PACKET
*********************************************************************************************************
*/

void  OSView_TerminalRxSetCallback (void (*call_back)(INT8U rx_data))
{
    OSView_TerminalRxCallbackFnct = call_back;
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                 SEND AN ASCII STRING TO TERMINAL WINDOW
*
* Description: This function allows an application to send strings to the TERMINAL window in the viewer.
*
* Arguments  : s     is a pointer to the string to send.
*
*              dly   allows the calling task to delay itself for 'dly' ticks until the current string
*                    is sent.  If 'dly' is set to 0, then the string will not be sent if a string is
*                    currently in the process of being sent.  In other words, if there is a string currently
*                    being sent and you sent 'dly' to 0, OSView_TxStr() will return to the caller and the
*                    string will not be sent.
*
* Note(s)    : 1) This function MUST be called from a TASK and NOT an ISR.
*********************************************************************************************************
*/

void  OSView_TxStr (INT8U *s, INT16U dly)
{
    INT16U  len;


    len = (INT16U)OS_StrLen(s);                           /* Make sure string fits in buffer           */
    if (len >= OS_VIEW_TX_STR_SIZE) {                     /* Force end of string if string too long    */
        len    = OS_VIEW_TX_STR_SIZE - 1;
        s[len] = '\0';
    }
    if (OSView_TxStrLen > 0) {                            /* Are currently busy sending a string?      */
        if (dly > 0) {                                    /* Yes, does caller want to wait?            */
            while (OSView_TxStrLen > 0) {                 /* Yes, let other tasks run                  */
                OSView_TxStrDlyCtr++;
                OSTimeDly(dly);
            }
            OSView_TxStrLen = len;                        /* Update the string length                  */
            strcpy((char *)&OSView_TxStrBuf[0], (char *)s);           /* Copy string to send to buffer             */
            if (OSView_TxStrLen > 0) {
                OSView_TxStart();
            }
        }
    } else {
        OSView_TxStrLen = len;                            /* Update the string length                  */
        strcpy((char *)&OSView_TxStrBuf[0], (char *)s);               /* Copy string to send to buffer             */
        if (OSView_TxStrLen > 0) {
            OSView_TxStart();
        }
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                  START TRANSMISSION OF REPLY PACKET
*********************************************************************************************************
*/

static  void  OSView_TxStart (void)
{
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr = 0;
#endif


    OS_ENTER_CRITICAL();
    if (OSView_TxActiveFlag == OS_FALSE) {
        OSView_TxHandler();
        OSView_TxIntEn();
    }
    OS_EXIT_CRITICAL();
}

/*
*********************************************************************************************************
*                                               Tx Handler
*
* Description: This routine is called from the transmitter buffer empty interrupt service handler.
*              It will send out the next byte in the buffer.
*
* Returns:     none
*********************************************************************************************************
*/

void  OSView_TxHandler (void)
{
    INT8U  tx_data;


    switch (OSView_TxState) {
        case OS_VIEW_TX_STATE_SD0:
             if (OSView_TxLen > 0 || OSView_TxStrLen > 0) { /* Packet in buffer or string waiting to be send ? */
                 OSView_Tx1(OS_VIEW_PROTOCOL_TX_SD0);
                 OSView_TxCtr++;
                 OSView_TxActiveFlag = OS_TRUE;
                 OSView_TxState      = OS_VIEW_TX_STATE_SD1;
                 OSView_TxIx         = 0;
             } else {                                       /* If there is nothing to do end transmission      */
                 OSView_TxActiveFlag = OS_FALSE;
                 OSView_TxIntDis();                         /* No more data to send, disable Tx interrupts     */
                 break;
             }
             break;

        case OS_VIEW_TX_STATE_SD1:
             OSView_Tx1(OS_VIEW_PROTOCOL_TX_SD1);
             OSView_TxCtr++;
             if (OSView_TxLen > 0) {
                 OSView_TxState = OS_VIEW_TX_STATE_DATA_LEN;
             } else {
                 OSView_TxState = OS_VIEW_TX_STATE_STR_LEN;
             }
             break;

        case OS_VIEW_TX_STATE_DATA_LEN:                       /* Include the packet length in the packet         */
             OSView_Tx1(OSView_TxLen);
             OSView_TxCtr++;
             OSView_TxState  = OS_VIEW_TX_STATE_DATA;
             OSView_TxChkSum = OSView_TxLen;
             break;

        case OS_VIEW_TX_STATE_DATA:
             tx_data = OSView_TxBuf[OSView_TxIx];
             OSView_Tx1(tx_data);
             OSView_TxCtr++;
             OSView_TxChkSum += tx_data;
             OSView_TxIx++;
             if (OSView_TxIx >= OSView_TxLen) {               /* See if we are done sending the packet           */
                 OSView_TxState  = OS_VIEW_TX_STATE_CHKSUM;
                 OSView_TxLen    = 0;
             }
             break;

        case OS_VIEW_TX_STATE_STR_LEN:                        /* String sending                                  */
             OSView_Tx1(OSView_TxStrLen + 1);
             OSView_TxCtr++;
             OSView_TxState  = OS_VIEW_TX_STATE_STR_TYPE;
             OSView_TxChkSum = OSView_TxStrLen + 1;
             break;

        case OS_VIEW_TX_STATE_STR_TYPE:
             OSView_Tx1('C');
             OSView_TxCtr++;
             OSView_TxState   = OS_VIEW_TX_STATE_STR_DATA;
             OSView_TxChkSum += 'C';
             break;

        case OS_VIEW_TX_STATE_STR_DATA:
             tx_data = OSView_TxStrBuf[OSView_TxIx];
             OSView_Tx1(tx_data);
             OSView_TxCtr++;
             OSView_TxChkSum += tx_data;
             if (++OSView_TxIx >= OSView_TxStrLen) {
                 OSView_TxState  = OS_VIEW_TX_STATE_CHKSUM;
                 OSView_TxStrLen = 0;
             }
             break;

        case OS_VIEW_TX_STATE_CHKSUM:                         /* Epilog ... checksum & end delimiter             */
             OSView_Tx1(OSView_TxChkSum);
             OSView_TxCtr++;
             OSView_TxState = OS_VIEW_TX_STATE_ED;
             break;

        case OS_VIEW_TX_STATE_ED:
             OSView_Tx1(OS_VIEW_PROTOCOL_TX_ED);
             OSView_TxCtr++;
             OSView_TxState  = OS_VIEW_TX_STATE_SD0;
             OSView_TxBufCnt = 0;                             /* Clear the Tx buffer                             */
             OSView_TxPktCtr++;
             break;

        default:
             OSView_TxState      = OS_VIEW_TX_STATE_SD0;
             OSView_TxActiveFlag = OS_FALSE;
             OSView_TxIntDis();                               /* No more data to send, disable Tx interrupts     */
             break;
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                     STORE 8, 16 or 32-bit DATA
*********************************************************************************************************
*/

static  void  OSView_TxStoINT8U (INT8U tx_data)
{
    if (OSView_TxBufCnt < OS_VIEW_TX_BUF_SIZE) {
        OSView_TxBuf[OSView_TxBufCnt++] = tx_data;
    }
}


static  void  OSView_TxStoINT16U (INT16U tx_data)
{
    OSView_TxStoINT8U(tx_data >> 8);
    OSView_TxStoINT8U(tx_data & 0x00FF);
}


static  void  OSView_TxStoINT32U (INT32U tx_data)
{
    OSView_TxStoINT16U(tx_data >> 16);
    OSView_TxStoINT16U(tx_data & 0x0000FFFFL);
}


static  void  OSView_TxStoPtr (void *ptr)
{
    INT32U  tx_data;


    tx_data = (INT32U)ptr;
    OSView_TxStoINT16U(tx_data >> 16);
    OSView_TxStoINT16U(tx_data & 0x0000FFFFL);
}


static  void  OSView_TxStoStr (INT8U *s)
{
    INT8U   len;
    INT8U   i;
    INT8U  *ps;


    len = 0;
    ps  = s;
    if (ps != (INT8U *)0) {                                 /* Calculate length of string                      */
        while (*ps) {
            ps++;
            len++;
        }
    }
    OSView_TxStoINT8U(len);                                 /* Store string, length first                      */
    ps = s;
    for (i = 0; i < len; i++) {
        OSView_TxStoINT8U((INT8U)*ps);
        ps++;
    }
}

#endif
