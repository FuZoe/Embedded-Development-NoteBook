/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.4.0
  * @date    10/15/2010
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include <includes.h>


#if ( OS_VIEW_MODULE == DEF_ENABLED )
extern	void OSView_RxTxISRHandler(void);
#endif


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/* hard fault handler in C, */  
/* with stack frame location as input parameter */  
void hard_fault_handler_c(unsigned int * hardfault_args)  
{  
  unsigned int stacked_r0;  
  unsigned int stacked_r1;  
  unsigned int stacked_r2;  
  unsigned int stacked_r3;  
  unsigned int stacked_r12;  
  unsigned int stacked_lr;  
  unsigned int stacked_pc;  
  unsigned int stacked_psr;  

  stacked_r0 = ((unsigned long) hardfault_args[0]);  
  stacked_r1 = ((unsigned long) hardfault_args[1]);  
  stacked_r2 = ((unsigned long) hardfault_args[2]);  
  stacked_r3 = ((unsigned long) hardfault_args[3]);  

  stacked_r12 = ((unsigned long) hardfault_args[4]);  
  stacked_lr = ((unsigned long) hardfault_args[5]);  
  stacked_pc = ((unsigned long) hardfault_args[6]);  
  stacked_psr = ((unsigned long) hardfault_args[7]);  
  
  printf ("[Hard fault handler]\n");  
  printf ("R0 = %x\n", stacked_r0);  
  printf ("R1 = %x\n", stacked_r1);  
  printf ("R2 = %x\n", stacked_r2);  
  printf ("R3 = %x\n", stacked_r3);  
  printf ("R12 = %x\n", stacked_r12);  
  printf ("LR = %x\n", stacked_lr);  
  printf ("PC = %x\n", stacked_pc);  
  printf ("PSR = %x\n", stacked_psr);  
  printf ("BFAR = %x\n", (*((volatile unsigned long *)(0xE000ED38))));  
  printf ("CFSR = %x\n", (*((volatile unsigned long *)(0xE000ED28))));  
  printf ("HFSR = %x\n", (*((volatile unsigned long *)(0xE000ED2C))));  
  printf ("DFSR = %x\n", (*((volatile unsigned long *)(0xE000ED30))));  
  printf ("AFSR = %x\n", (*((volatile unsigned long *)(0xE000ED3C))));  
   
  while(1) 
  { 
     ;
  }  
}  

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}



/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	CPU_SR         cpu_sr;
	OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
    OSIntNesting++;
    OS_EXIT_CRITICAL();

    OSTimeTick();                                /* Call uC/OS-II's OSTimeTick()                       */

    OSIntExit();                                 /* Tell uC/OS-II that we are leaving the ISR          */
}


/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USART1_IRQHandler(void)
{
	#if ( OS_VIEW_MODULE == DEF_ENABLED )
		OSView_RxTxISRHandler();
	#endif
}


/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
