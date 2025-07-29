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
* Filename   : OS_VIEWc.C
* Version    : V1.21
* Programmer : zmt
*********************************************************************************************************
*/

#include <includes.h>

#if ( OS_VIEW_MODULE == DEF_ENABLED )


/*
*********************************************************************************************************
*                                              OSView_TmrInit()
*
* Description : Select & initialize a timer for use with uC/OS-View.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/
void  OSView_TmrInit (void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseStructure.TIM_Period        = 0xFFFF;
    TIM_TimeBaseStructure.TIM_Prescaler     = 0x00;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_SetCounter(TIM3, 0);
    TIM_PrescalerConfig(TIM3, 256, TIM_PSCReloadMode_Immediate);
    TIM_Cmd(TIM3, ENABLE);
}	

/*
*********************************************************************************************************
*                                         OSView_TmrRd()
*
* Description : Read the current counts of a 32-bit free running timer.
*
* Argument(s) : none.
*
* Return(s)   : The 32 bit counts of the timer.
*********************************************************************************************************
*/

CPU_INT32U  OSView_TmrRd (void)
{
   return ((CPU_INT32U)TIM_GetCounter(TIM3));
}


/*
*********************************************************************************************************
*                                           EXIT uC/OS-View
*
* Description: This function is called if your target needs to 'uninstall' uC/OS-View.
*
* Note(s)    :
*********************************************************************************************************
*/

void  OSView_Exit (void)
{
	//NULL
}


/*
*********************************************************************************************************
*                                           Obtain CPU name
*********************************************************************************************************
*/

void  OSView_GetCPUName (INT8U *s)
{
    INT8U  cpu_clk_freq;
    RCC_ClocksTypeDef  RCC_Clocks;

    RCC_GetClocksFreq(&RCC_Clocks);

    cpu_clk_freq = (INT8U)( ((CPU_INT32U)RCC_Clocks.SYSCLK_Frequency) / 1000000L);
    strcpy((char *)s, "STM32F10x (xx MHz)");
    s[18] = cpu_clk_freq / 10 + '0';
    s[19] = cpu_clk_freq % 10 + '0';
	s[20] = '\0' ;
}

/*
*********************************************************************************************************
*                                  Obtain Interrupt Stack information
*********************************************************************************************************
*/

INT32U  OSView_GetIntStkBase (void)
{
    return (0);                                       /* We are not using an ISR stack                 */
}


INT32U  OSView_GetIntStkSize (void)
{
    return (0);                                       /* We are not using an ISR stack                 */
}



/*$PAGE*/
/*
*********************************************************************************************************
*                                    INITIALISE uC/OS-View COM PORT
*
* Description: Initialize the hardware required for the OS to run. This will work on any target hardware,
*              but may have to be tailored a little (regarding the clock frequency). Of course the same
*              holds true if for some reason you choose to use another timer.
*
* Note(s)    : 1) This function assumes that a free running timer has been initialized.  The timer can
*                 either be a 16 bits or 32 bits timer.  Your application needs to provide a function
*                 called OSView_TmrRd() that reads the current counts of this timer.  The free running
*                 timer is initialized by the BSP function OSView_TmrInit().
*********************************************************************************************************
*/

void  OSView_InitTarget (INT32U baud_rate)
{
    GPIO_InitTypeDef        GPIO_InitStructure;
    USART_InitTypeDef       USART_InitStructure;
    NVIC_InitTypeDef        NVIC_InitStructure;	

	OSView_TmrInit();                                    /* Initialize the free running timer          */

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* Configure USARTx_Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;			
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USARTx_Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	  
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate            = baud_rate;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);

    /* Configure the NVIC Preemption Priority Bits */  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 
    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                       Disable Rx Interrupts
*********************************************************************************************************
*/

void  OSView_RxIntDis (void)
{
    USART_ITConfig(OS_VIEW_USART, USART_IT_RXNE, DISABLE);
}

/*
*********************************************************************************************************
*                                       Enable Rx Interrupts
*********************************************************************************************************
*/

void  OSView_RxIntEn (void)
{
    USART_ITConfig(OS_VIEW_USART, USART_IT_RXNE, ENABLE);
}

/*
*********************************************************************************************************
*                                 Rx Communication handler for uC/OS-View
*
* Description: This function is called by OSView_RxISR (see OS_VIEWa.ASM) to process a received
*              character interrupt.
*
* Note(s)    : This adaptation of uC/OS-View assumes that a 'combined' interrupt is generated by the UART
*              and thus this function is not needed.
*********************************************************************************************************
*/

void  OSView_RxISRHandler (void)
{
	//NULL
}

/*
*********************************************************************************************************
*                                Rx/Tx Communication handler for uC/OS-View
*
* Description: This function is  called in stm32f10x_it.c 
*********************************************************************************************************
*/ 
void  OSView_RxTxISRHandler (void)
{
    CPU_SR          cpu_sr;

    CPU_CRITICAL_ENTER();                                       /* Tell uC/OS-II that we are starting an ISR            */
    OSIntNesting++;
    CPU_CRITICAL_EXIT();


    if (USART_GetITStatus(OS_VIEW_USART, USART_IT_RXNE) != RESET) {
                      /* Read one byte from the receive data register         */
        OSView_RxHandler( USART_ReceiveData(OS_VIEW_USART) & 0xFF );

        USART_ClearITPendingBit(OS_VIEW_USART, USART_IT_RXNE);          /* Clear the USART Receive interrupt                   */

    }

    if (USART_GetITStatus(OS_VIEW_USART, USART_IT_TXE) != RESET) {
        OSView_TxHandler();

        USART_ClearITPendingBit(OS_VIEW_USART, USART_IT_TXE);           /* Clear the USART transmit interrupt                  */

    }

    OSIntExit();                                                /* Tell uC/OS-II that we are leaving the ISR            */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                      Communication for uC/OS-View
*
* Description: Send 1 character to COM Port
*********************************************************************************************************
*/

void  OSView_Tx1 (INT8U c)
{
	USART_SendData(OS_VIEW_USART, c);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                       Disable Tx Interrupts
*********************************************************************************************************
*/

void  OSView_TxIntDis (void)
{
    USART_ITConfig(OS_VIEW_USART, USART_IT_TXE, DISABLE);
}

/*
*********************************************************************************************************
*                                       Enable Tx Interrupts
*********************************************************************************************************
*/

void  OSView_TxIntEn (void)
{
    USART_ITConfig(OS_VIEW_USART, USART_IT_TXE, ENABLE);
}






#endif	   /*  end of  #if ( OS_VIEW_MODULE == DEF_ENABLED ) */

