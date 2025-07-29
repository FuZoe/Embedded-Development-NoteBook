/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The STM32 USB bootloader application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2011-2-12
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
#include "stm32f10x.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "mass_mal.h"


/* Private variables ---------------------------------------------------------*/
extern uint8_t  Fat_RootDir[FAT_SIZE + ROOT_DIR_SIZE];  /* RAM to store the file allocation table */
extern const uint8_t RootDirEntry[DIR_ENTRY] ;
typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;

/* Private function prototypes -----------------------------------------------*/
void enter_usb_isp(void);
void execute_user_code(void);
uint8_t check_isp_entry_pin(void);


/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int main(void)
{
  
  /* check_isp_entry_pin() function does not return if isp entry is not requested */
  if( !check_isp_entry_pin() )
  {
	execute_user_code();
	while(1);
  }

  enter_usb_isp();
}

uint8_t check_isp_entry_pin(void)
{ 
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC , ENABLE); 						 
  /**
  *  Tamper Button -> PC13 , Wakeup Button -> PA0 , User Button -> PA8
  */				
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  if( GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) || !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) || \
      !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) )
  {   
     return 1;
  }
  else
  {
     return 0;
  }
}

void execute_user_code(void)
{
	if (((*(vu32*)USER_FLASH_START) & 0x2FFF0000 ) == 0x20000000)
    { /* Jump to user application */

      JumpAddress = *(vu32*) (USER_FLASH_START + 4);
      Jump_To_Application = (pFunction) JumpAddress;
      /* Initialize user application's Stack Pointer */
      __set_MSP(*(vu32*) USER_FLASH_START);
      Jump_To_Application();
    }    
}

void enter_usb_isp(void)
{
  uint32_t n,m,next_cluster;

  /* generate File Allocation Table to save Flash space */
  /* First Two FAT entries are reserved */
  Fat_RootDir[0]= 0xF8;
  Fat_RootDir[1]= 0xFF;
  Fat_RootDir[2]= 0xFF;

  /* Start cluster of a file is indicated by the Directory entry = 2 */
  m = 3;
  for ( n = 3;n < NO_OF_CLUSTERS+2;n+=2) 
  {
	if( n == (NO_OF_CLUSTERS-1) )
	{
	  next_cluster = 0xFFF;
	}
	else
	{
	  next_cluster = n + 1;
	}
  	Fat_RootDir[m] = (uint8_t)n & 0xFF;
  	Fat_RootDir[m+1] = (((uint8_t)next_cluster & 0xF) << 4) | ((uint8_t)(n>>8)&0xF);
  	Fat_RootDir[m+2] = (uint8_t)(next_cluster >> 4) & 0xFF;
    
	m = m+3;
  }
  /* Copy root directory entries */
  for (n = 0; n < DIR_ENTRY ; n++) 				  /* Copy Initial Disk Image */
  {             
    Fat_RootDir[(FAT_SIZE+n)] = RootDirEntry[n];  /*   from Flash to RAM     */
  }

  /* Correct file size entry for file firmware.bin */
  Fat_RootDir[FAT_SIZE+60] = (uint8_t)(USER_FLASH_SIZE & 0xFF);
  Fat_RootDir[FAT_SIZE+61] = (uint8_t)(USER_FLASH_SIZE >> 8);
  Fat_RootDir[FAT_SIZE+62] = (uint8_t)(USER_FLASH_SIZE >> 16);
  Fat_RootDir[FAT_SIZE+63] = (uint8_t)(USER_FLASH_SIZE >> 24);

  FLASH_Unlock(); 

  for( n = USER_FLASH_START ; n < USER_FLASH_END; n += FLASH_PAGE_SIZE )
  {
	  FLASH_ErasePage( n );  
  }	

  Set_System();
  Set_USBClock();
  USB_Interrupts_Config();

  USB_Init();
  while (bDeviceState != CONFIGURED);

  USB_Configured_LED();

  while (1)
  {}
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

