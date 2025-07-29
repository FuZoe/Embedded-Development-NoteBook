/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               dso.c
** Descriptions:            The dso application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2011-3-16
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
#include "dso.h"
#include "key.h"
#include "glcd.h"
#include "SimHei.c"
#include <includes.h>            


/* Private define ------------------------------------------------------------*/
#define DSO_VER	 "V1.0"	  /* 当前版本 */
#define ATT_COUNT	6	  /* 衰减倍数表g_DSO.Ch1Attenuation = 23 幅度衰减因子: (1) 28 === 1V  (1:1) */


/* Private variables ---------------------------------------------------------*/

/* 时间分度表 g_DSO.TimeBaseId作为数组索引 */
const uint32_t g_TimeTable[] =
{
	20, 	50, 	100, 
	200,	500,	1000,
	2000,	5000,	10000,
	20000,	50000,	100000,
	200000,	500000,	1000000		
};


const float g_AttTable[ATT_COUNT][2] =
{
	/* {除数,每大格电压}  1:1 */
	{327.6,	5000},
	{130.8,	2000},
	{64.8,	1000},
	{32.4,	500},
	{12,	200},
	{6,	    100},
};

/* 
   示波器通道1同时连接到CPU的PC0、PC1、PC2
   PC0 = ADC1_IN10	
   PC1 = ADC2_IN11
   PC2 = ADC3_IN12

   示波器通道2连接到CPU的PC3
   PC1 = ADC2_IN13

   对于通道1的模拟信号，可以同时启动3个ADC进行交错采样，理论上可以实现3Mbps采样速率 

   通道2只能启动1个ADC进行采样。
   支持通道1和通道2进行同步采样。
*/

extern OS_EVENT *DSOHelp;
extern OS_EVENT *DSORun;
extern OS_EVENT *LCDRefresh;
DSO    g_DSO;


/*******************************************************************************
* Function Name  : DSO_Initializtion
* Description    : 对示波器通道1、通道2进行初始化配置。主要完成GPIO的配置、
*                  ADC的配置、定时器的配置以及DMA的配置。
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void DSO_Initializtion(void)
{
  DMA_InitTypeDef DMA_InitStructure;
  ADC_InitTypeDef	ADC_InitStructure;	
  GPIO_InitTypeDef GPIO_InitStructure; 

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC	| RCC_APB2Periph_AFIO, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* 配置DMA1_1 用于CH1 */
  /* Enable DMA1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  DMA_DeInit(DMA1_Channel1);		                                            /* 复位DMA1寄存器到缺省状态 */
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	                /* 选择ADC1的数据寄存器作为源 */
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_DSO.Ch1Buf;	            /* 目标地址 */
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	                        /* 设置DMA传输方向，外设(ADC)作为源 */
  DMA_InitStructure.DMA_BufferSize = SAMPLE_COUNT;	                            /* 设置缓冲区大小 */
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	            /* 外设地址不自增 */
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	                    /* 存储器地址需要自增 */
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	/* 选择外设传输单位：16bit */
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	        /* 选择内存传输单位：16bit */
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	                                /* 无需循环模式   */
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;	                        /* 选择DMA优先级 */
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	                                /* DMA传输类型，不是内存到内存 */
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);

  /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);

  /* 配置DMA2_5 用于CH2 */

  /* Enable DMA2 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

  DMA_DeInit(DMA2_Channel5);		                                            /* 复位DMA2寄存器到缺省状态 */
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC3_DR_Address;	                /* 选择ADC3的数据寄存器作为源 */
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_DSO.Ch2Buf;	            /* 目标地址 */
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	                        /* 设置DMA传输方向，外设(ADC)作为源 */
  DMA_InitStructure.DMA_BufferSize = SAMPLE_COUNT;	                            /* 设置缓冲区大小 */
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	            /* 外设地址不自增 */
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	                    /* 存储器地址需要自增 */
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	/* 选择外设传输单位：16bit */
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	        /* 选择内存传输单位：16bit */
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	                                /* 无需循环模式   */
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;	                        /* 选择DMA优先级 */
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	                                /* DMA传输类型，不是内存到内存 */
  DMA_Init(DMA2_Channel5, &DMA_InitStructure);

  /* Enable DMA2 channel5 */
  DMA_Cmd(DMA2_Channel5, ENABLE);

  /* 配置ADC1  */
	
  /* ADCCLK = PCLK2/2 */
  RCC_ADCCLKConfig(RCC_PCLK2_Div2);     /* ADCCLK = PCLK2/4 = 18MHz */

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;		                /* 连续转换静止 */
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC3;	    /* 选择TIM1的CC3做触发 */
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	                /* 数据右对齐,高位为0 */
  ADC_InitStructure.ADC_NbrOfChannel = 1;	                                /* 1个通道 */
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channels configuration */
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_1Cycles5);

  /* Enable ADC1 external trigger conversion */
  ADC_ExternalTrigConvCmd(ADC1, ENABLE);

  /* 使能 ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);

  /* 使能 ADC1 复位校准寄存器 */
  ADC_ResetCalibration(ADC1);
  /* 检测复位校准寄存器 */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* 开始 ADC1 校准 */
  ADC_StartCalibration(ADC1);
  /* 等待校准结束 */
  while(ADC_GetCalibrationStatus(ADC1));

  /* 配置ADC3  */
    
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;		                /* 连续转换静止 */
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC3;	    /* 选择TIM1的CC3做触发 */
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	                /* 数据右对齐,高位为0 */
  ADC_InitStructure.ADC_NbrOfChannel = 1;	                                /* 1个通道 */
  ADC_Init(ADC3, &ADC_InitStructure);

  /* ADC1 regular channels configuration */
  ADC_RegularChannelConfig(ADC3, ADC_Channel_13, 1, ADC_SampleTime_1Cycles5);

  /* Enable ADC3 external trigger conversion */
  ADC_ExternalTrigConvCmd(ADC3, ENABLE);

  /* 使能 ADC3 */
  ADC_Cmd(ADC3, ENABLE);

  /* Enable ADC3 DMA */
  ADC_DMACmd(ADC3, ENABLE);

  /* 使能 ADC3 复位校准寄存器 */
  ADC_ResetCalibration(ADC3);
  /* 检测复位校准寄存器 */
  while(ADC_GetResetCalibrationStatus(ADC3));

  /* 开始 ADC3 校准 */
  ADC_StartCalibration(ADC3);
  /* 等待校准结束 */
  while(ADC_GetCalibrationStatus(ADC3));

  /* 配置采样触发定时器，使用TIM1 CC1 */
  DSO_SampleRate(g_DSO.SampleFreq);	/* 修改采样频率(启动时100K) */
}

/*******************************************************************************
* Function Name  : DSO_SampleRate
* Description    : 修改采样频率
* Input          : - freq: 采样频率 单位Hz 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void DSO_SampleRate(uint32_t freq)
{
  /* 配置定时器1第3通道CC3作为ADC1的触发源 */	  
  uint16_t Period;	
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  TIM_Cmd(TIM1, DISABLE);

  //g_DSO.SampleRate = freq;	/* 将采样频率保存到全部变量中 */
	
  /* 采样频率计算公式 : period = 72000000 / freq ; 1200 == 60KHz 采样频率 */
  Period = 72000000 / freq;		

  /* Enable TIM1 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
  TIM_TimeBaseStructure.TIM_Period = Period;          
  TIM_TimeBaseStructure.TIM_Prescaler = 0;        /* 计数频率 = 72000 000 / 18 = 4000 000	 */
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  /* TIM1 channel1 configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;                
  TIM_OCInitStructure.TIM_Pulse = TIM_TimeBaseStructure.TIM_Period / 2; 
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;         
  TIM_OC3Init(TIM1, &TIM_OCInitStructure);

  /* TIM1 counter enable */
  TIM_Cmd(TIM1, ENABLE);

  /* TIM1 main Output Enable */
  TIM_CtrlPWMOutputs(TIM1, ENABLE);  
}

/*******************************************************************************
* Function Name  : Display_Help
* Description    : 显示help画面
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void Display_Help(void)
{
  uint16_t y;
  uint16_t LineCap;

  GUI_SetFont(&GUI_FontHZ_SimHei); 

  GUI_SetBkColor(GUI_BLACK);
  GUI_Clear();

  GUI_SetColor(GUI_WHITE);

  LineCap = 20; /* 行间距 */

  y = 4;	  	
  GUI_DispStringAt( "HY-RedBull V3.0 数字示波器演示程序" , 24, y ) ;
  y += LineCap;
  GUI_DispStringAt( "www.PowerMCU.com" , 98, y - 4 ) ;
  y += LineCap;
  GUI_DispStringAt( "操作提示" , 130, y - 4 ) ;	
  y += LineCap;
  GUI_DispStringAt( "TAMPER键 = 切换通道焦点(CH1或CH2)" , 5, y );
  y += LineCap;
  GUI_DispStringAt( "WAKEUP键 = 显示帮助或退出帮助" , 5, y );
  y += LineCap;
  GUI_DispStringAt( "USER  键 = 暂停或实时运行" , 5, y );
  y += LineCap;
  GUI_DispStringAt( "摇杆上键 = 放大波形垂直幅度或向上移动" , 5, y );
  y += LineCap;
  GUI_DispStringAt( "摇杆下键 = 缩小波形垂直幅度或向下移动" , 5, y );
  y += LineCap;
  GUI_DispStringAt( "摇杆左键 = 水平展宽波形" , 5, y );
  y += LineCap;
  GUI_DispStringAt( "摇杆右键 = 水平缩小波形" , 5, y );
  y += LineCap;
  GUI_DispStringAt( "摇杆OK键 = 切换摇杆调节模式(幅度或位置)" , 5, y );
  y += LineCap;
}

/*******************************************************************************
* Function Name  : DSO_Help
* Description    : 显示help画面
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void DSO_Help(void)
{
  uint8_t key;

  Display_Help();
  for(;;)
  {		  
    key = GetKey(); /* 读取按键，大于0表示有键按下 */
    if (key > 0)
	{
       /* 有键按下 */
	   switch (key)
	   {
		  case KEY_DOWN_JOY_LEFT:	 /* 摇杆LEFT键按下 */
		  case KEY_DOWN_JOY_RIGHT:   /* 摇杆RIGHT键按下 */
		  case KEY_DOWN_WAKEUP:
		  case KEY_DOWN_USER:
		  case KEY_DOWN_JOY_OK:	     /* 摇杆OK键 */
		       OSSemPost(DSORun);	 /* 发送信号量 退出,进入全速运行状态*/
			   return;
		  case KEY_DOWN_JOY_UP:	     /* 摇杆UP键按下 */
			   break;
		  case KEY_DOWN_JOY_DOWN:	 /* 摇杆DOWN键按下 */
			   break;
		  default:
			   break;
	   }
	} 
  }
}

/*******************************************************************************
* Function Name  : Display_Frame
* Description    : 显示波形窗口的边框和刻度线
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void Display_Frame(void)
{
  uint16_t x, y;

  GUI_SetColor(GUI_WHITE); 
  GUI_SetBkColor(GUI_BLACK);
	
  /* 绘制一个实线矩形框 x, y, h, w */
  GUI_DrawRect(9, 19, 9 + 302, 19 + 202);

  /* 刷屏 */
  GUI_ClearRect(9 + 1, 19 + 1, 9 + 302 - 1 , 19 + 202 -1 );
						   
  /* 绘制垂直刻度点 */
  for (x = 0; x < 13; x++)
  {
	 for (y = 0; y < 41; y++)
	 {
		 GUI_DrawPixel(10 + (x * 25), 20 + (y * 5));
	 }
  }

  /* 绘制水平刻度点 */
  for (y = 0; y < 9; y++)
  {
	 for (x = 0; x < 61; x++)
	 {
		 GUI_DrawPixel(10 + (x * 5), 20 + (y * 25));
	 }
  }

  /* 绘制垂直中心刻度点 */
  for (y = 0; y < 41; y++)
  {	 
	 GUI_DrawPixel(9 + (6 * 25), 20 + (y * 5));
	 GUI_DrawPixel(11 + (6 * 25), 20 + (y * 5));
  }

  /* 绘制水平中心刻度点 */
  for (x = 0; x < 61; x++)
  {	 
	 GUI_DrawPixel(10 + (x * 5), 19 + (4 * 25));
	 GUI_DrawPixel(10 + (x * 5), 21 + (4 * 25) );
  }
}

/*******************************************************************************
* Function Name  : LCD_DrawPoints
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_DrawPoints( uint16_t *x, uint16_t *y, uint16_t Size )
{
	uint16_t i;

	for (i = 0 ; i < Size - 1; i++)
	{
		GUI_DrawLine(x[i], y[i], x[i + 1], y[i + 1] );
	}
}

/*******************************************************************************
* Function Name  : Display_CH_Info
* Description    : 显示通道信息
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/	
void Display_CH_Info(void)
{
	char buf[32];   /* 字符显示缓冲区 */

	GUI_SetFont(&GUI_FontHZ_SimHei); 

	/* 显示示波器程序版本 */
	GUI_SetColor(GUI_WHITE);   
	GUI_SetBkColor(GUI_BLACK);		
	GUI_DispStringAt( DSO_VER, 10, 2 );

	/* 显示通道1信息 */
	if (g_DSO.Ch1DC == 1)
	{
		strcpy(buf, "CH1 DC ");
	}
	else
	{
		strcpy(buf, "CH1 DC ");
	}

	if (g_DSO.Ch1VScale >= 1000)
	{
		sprintf(&buf[7], "%d.00V", g_DSO.Ch1VScale / 1000);
	}
	else
	{
		sprintf(&buf[7], "%dmV", g_DSO.Ch1VScale);
	}

	if (g_DSO.ActiveCH == 1)
	{
		GUI_SetColor(GUI_WHITE);  
		GUI_SetBkColor(GUI_MAGENTA);	
	}
	else
	{
		GUI_SetColor(GUI_WHITE);   
		GUI_SetBkColor(GUI_BLACK);		
	}

	GUI_DispStringAt( buf, 10, 224 );

	/* 显示通道2信息 */
	if (g_DSO.Ch2DC == 1)
	{
		strcpy(buf, "CH2 DC ");
	}
	else
	{
		strcpy(buf, "CH2 DC ");
	}

	if (g_DSO.Ch2VScale >= 1000)
	{
		sprintf(&buf[7], "%d.00V", g_DSO.Ch2VScale / 1000);
	}
	else
	{
		sprintf(&buf[7], "%dmV", g_DSO.Ch2VScale);
	}
	if (g_DSO.ActiveCH == 2)
	{
		GUI_SetColor(GUI_WHITE);   
		GUI_SetBkColor(GUI_MAGENTA);
	}
	else
	{
		GUI_SetColor(GUI_WHITE);  
		GUI_SetBkColor(GUI_BLACK);	
	}
	GUI_DispStringAt( buf, 120, 224 );

	/* 显示时基 */
	GUI_SetColor(GUI_WHITE);      /* 时基白色 */
	GUI_SetBkColor(GUI_BLACK);	 

	if (g_DSO.TimeBase < 1000)
	{
		sprintf(buf, "Time %3dus", g_DSO.TimeBase);
	}
	else if (g_DSO.TimeBase < 1000000)
	{
		sprintf(buf, "Time %3dms", g_DSO.TimeBase / 1000);
	}
	else
	{
		sprintf(buf, "Time %3ds ", g_DSO.TimeBase / 1000000);	
	}
	GUI_DispStringAt( buf, 230, 224 );


	/* 显示调节模式 */
	GUI_SetColor(GUI_WHITE);    /* 白字 */
	GUI_SetBkColor(GUI_BLACK);	/* 蓝底 */
	if (g_DSO.AdjustMode == 1)
	{
		GUI_DispStringAt((uint8_t *)"调节位置", 245, 2 );
	}
	else
	{
		GUI_DispStringAt((uint8_t *)"调节幅度", 245, 2 );
	}

	sprintf(buf, "采样频率:%7dHz",	g_DSO.SampleFreq);
	GUI_DispStringAt( buf, 75, 2 );
}

/*******************************************************************************
* Function Name  : DSO_Configuration
* Description    : 初始化参数
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void DSO_Configuration(void)
{
  g_DSO.Ch1Attenuation = 23;	/* 设置缺省衰减系数 */
  g_DSO.Ch2Attenuation = 23;	/* 设置缺省衰减系数 */
  g_DSO.Ch1VScale = 1000;	    /* 缺省是1V */
  g_DSO.Ch2VScale = 1000;	    /* 缺省是1V */

  g_DSO.Ch1VOffset = 70; 	    /* 通道1 GND线位置 */
  g_DSO.Ch2VOffset = 170;       /* 通道2 GND线位置 */

  g_DSO.ActiveCH = 1;		    /* 缺省是CH1 */
  g_DSO.AdjustMode = 1;	        /* 缺省是调节垂直偏移 可以切换到2调节幅度 */

  g_DSO.HoldEn = 0;

  g_DSO.TimeBaseId = 0;									 
  g_DSO.TimeBase = 	g_TimeTable[g_DSO.TimeBaseId];
  g_DSO.SampleFreq = 25000000 / g_DSO.TimeBase;


  g_DSO.Ch1AttId = 2;
  g_DSO.Ch1Attenuation = g_AttTable[g_DSO.Ch1AttId][0];
  g_DSO.Ch1VScale =  g_AttTable[g_DSO.Ch1AttId][1];

  g_DSO.Ch2AttId = 2;
  g_DSO.Ch2Attenuation = g_AttTable[g_DSO.Ch2AttId][0];
  g_DSO.Ch2VScale =  g_AttTable[g_DSO.Ch2AttId][1];
}

/*******************************************************************************
* Function Name  : Inc_SampleFreq
* Description    : Sets the cursor position.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position. 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void Inc_SampleFreq(void)
{
  if (g_DSO.TimeBaseId < (sizeof(g_TimeTable) / 4) - 1)
  {
	 g_DSO.TimeBaseId++;
  }
													 
  g_DSO.TimeBase = 	g_TimeTable[g_DSO.TimeBaseId];
  g_DSO.SampleFreq = 25000000 / g_DSO.TimeBase;

  DSO_SampleRate(g_DSO.SampleFreq);	/* 改变采样频率 */
}

/*******************************************************************************
* Function Name  : Dec_SampleFreq
* Description    : 降低采样频率，按 1-2-5
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void Dec_SampleFreq(void)
{
  if (g_DSO.TimeBaseId > 0)
  {
	 g_DSO.TimeBaseId--;
  }													 
  g_DSO.TimeBase = g_TimeTable[g_DSO.TimeBaseId];
  g_DSO.SampleFreq = 25000000 / g_DSO.TimeBase;

  DSO_SampleRate(g_DSO.SampleFreq);	/* 改变采样频率 */
}

/*******************************************************************************
* Function Name  : Adjust_DSO
* Description    : 降低衰减，按 1-2-5 
* Input          : - ch: 通道号，1或2
*                  - mode: 0 降低 1增加
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void Adjust_DSO(uint8_t ch, uint8_t mode)
{

  if (ch == 1)
  {
	 if (mode == 0) /* 降低 */
	 {
		 if (g_DSO.Ch1AttId > 0)
		 {
			 g_DSO.Ch1AttId--;
		 }
	 }
	 else		    /* 增加 */
	 {
		 if (g_DSO.Ch1AttId < ATT_COUNT - 1)
		 {
			 g_DSO.Ch1AttId++;
		 }
	 }
	 g_DSO.Ch1Attenuation = g_AttTable[g_DSO.Ch1AttId][0];
	 g_DSO.Ch1VScale =  g_AttTable[g_DSO.Ch1AttId][1];
  }
  else if (ch == 2)
  {
	 if (mode == 0) /* 降低 */
	 {
		 if (g_DSO.Ch2AttId > 0)
		 {
			 g_DSO.Ch2AttId--;
		 }
	 }
	 else		    /* 增加 */
	 {
		 if (g_DSO.Ch2AttId < ATT_COUNT - 1)
		 {
			 g_DSO.Ch2AttId++;
		 }
	 }
	 g_DSO.Ch2Attenuation = g_AttTable[g_DSO.Ch2AttId][0];
	 g_DSO.Ch2VScale =  g_AttTable[g_DSO.Ch2AttId][1];
  }
}

/*******************************************************************************
* Function Name  : Stop_ADC
* Description    : 暂停ADC采样
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void Stop_ADC(void)
{
  TIM_Cmd(TIM1, DISABLE);

  /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, DISABLE);
	
  /* Enable DMA2 channel5 */
  DMA_Cmd(DMA2_Channel5, DISABLE);
}

/*******************************************************************************
* Function Name  : Start_ADC
* Description    : 开始ADC采样
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void Start_ADC(void)
{
  DMA_InitTypeDef DMA_InitStructure;

  DMA_DeInit(DMA1_Channel1);		                                            /* 复位DMA1寄存器到缺省状态 */
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	                /* 选择ADC1的数据寄存器作为源 */
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_DSO.Ch1Buf;	            /* 目标地址 */
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	                        /* 设置DMA传输方向，外设(ADC)作为源 */
  DMA_InitStructure.DMA_BufferSize = SAMPLE_COUNT;	                            /* 设置缓冲区大小 */
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	            /* 外设地址不自增 */
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	                    /* 存储器地址需要自增 */
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	/* 选择外设传输单位：16bit */
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	        /* 选择内存传输单位：16bit */
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	                                /* 无需循环模式   */
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;	                        /* 选择DMA优先级 */
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	                                /* DMA传输类型，不是内存到内存 */
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);

  DMA_DeInit(DMA2_Channel5);		                                            /* 复位DMA2寄存器到缺省状态 */
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC3_DR_Address;	                /* 选择ADC1的数据寄存器作为源 */
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_DSO.Ch2Buf;	            /* 目标地址 */
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	                        /* 设置DMA传输方向，外设(ADC)作为源 */
  DMA_InitStructure.DMA_BufferSize = SAMPLE_COUNT;	                            /* 设置缓冲区大小 */
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	            /* 外设地址不自增 */
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	                    /* 存储器地址需要自增 */
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	/* 选择外设传输单位：16bit */
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	        /* 选择内存传输单位：16bit */
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	                                /* 无需循环模式   */
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;	                        /* 选择DMA优先级 */
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	                                /* DMA传输类型，不是内存到内存 */
  DMA_Init(DMA2_Channel5, &DMA_InitStructure);

  /* Enable DMA2 channel5 */
  DMA_Cmd(DMA1_Channel1, ENABLE);

  /* Enable DMA2 channel5 */
  DMA_Cmd(DMA2_Channel5, ENABLE);

  TIM_Cmd(TIM1, ENABLE);
}

/*******************************************************************************
* Function Name  : DSO_Run
* Description    : DSO全速运行状态
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void DSO_Run(void)
{
  INT8U   err;
  uint8_t key;

  DSO_Configuration();	 /* 初始化示波器参数 */
  DSO_Initializtion();	 /* 配置示波器用到的外设：ADC,TIM,DMA等 */

  for(;;)
  {
     OSSemPend(DSORun,10,&err);	/* 获取信号量 延时10毫秒 */
	 if( err == OS_ERR_NONE )
	 {
        GUI_SetBkColor(GUI_BLACK);
        GUI_Clear();		   
	    GUI_SetColor(GUI_WHITE);   
	 }
     /* 读取按键 大于0表示有键按下 */
	 key = GetKey();
	 if (key > 0)
	 {
		/* 有键按下 */
		switch (key)
		{
		   case  KEY_DOWN_TAMPER:	        /* TAMPER键 通道选择(CH1或CH2) */
				 if (g_DSO.ActiveCH == 1)
				 {
					 g_DSO.ActiveCH = 2;
				 }
				 else
				 {
					 g_DSO.ActiveCH = 1;
				 }
				 OSSemPost(LCDRefresh);	    /* 发送信号量 请求刷新LCD */
				 break;

		   case  KEY_DOWN_WAKEUP:	        /* WAKEUP键 调节模式选择(幅度或者垂直偏移) */
				 OSSemPost(DSOHelp);	    /* 发送信号量 退出 进入全速运行状态 */
				 break;

		   case  KEY_DOWN_USER:	            /* USER键 暂时或实时 */
				 if ( g_DSO.HoldEn == 0 )
				 {
					 g_DSO.HoldEn = 1;
					 /* 保存暂停时的时基 为了水平扩展用 */
					 g_DSO.TimeBaseIdHold = g_DSO.TimeBaseId;	
                     Stop_ADC();
				 }
				 else
				 {
					 g_DSO.HoldEn = 0;
					 Start_ADC();				
				 }
				 OSSemPost(LCDRefresh);	    /* 发送信号量 请求刷新LCD */
				 break;

		   case  KEY_DOWN_JOY_LEFT:	        /* 摇杆LEFT键按下 */
				 if (g_DSO.HoldEn == 0)
				 {
					 Dec_SampleFreq();	    /* 递减采样频率 */
				     OSSemPost(LCDRefresh); /* 发送信号量 请求刷新LCD */
				 }
				 else
				 {
				     /* 波形水平移动 待完善 */
				 }
				 break;

		   case  KEY_DOWN_JOY_RIGHT:	    /* 摇杆RIGHT键按下 */
				 if (g_DSO.HoldEn == 0)
				 {
					 Inc_SampleFreq();  	/* 递增采样频率 */					
				     OSSemPost(LCDRefresh); /* 发送信号量 请求刷新LCD */
				 }
				 else
				 {
					 /* 波形水平移动 待完善 */
				 }
				 break;

		   case  KEY_DOWN_JOY_OK:	        /* 摇杆OK键 */
				 if (g_DSO.AdjustMode == 0)
				 {
					 g_DSO.AdjustMode = 1;
				 }
				 else
				 {
					 g_DSO.AdjustMode = 0;
				 }
				 OSSemPost(LCDRefresh);     /* 发送信号量 请求刷新LCD */
				 break;

		   case  KEY_DOWN_JOY_UP:	        /* 摇杆UP键按下 */
				 if (g_DSO.ActiveCH == 1) 	/* 当前激活的是CH1 */
				 {
					if (g_DSO.AdjustMode == 0)	/* 调节幅度放大倍数 */
					{
						Adjust_DSO(1, 1);
					}
					else 	/* 调节上下偏移 */
					{
						g_DSO.Ch1VOffset -= 5;
					}
				 }
				 else	/* 当前激活的是CH2 */
				 {
					 if (g_DSO.AdjustMode == 0)	/* 调节幅度放大倍数 */
					 {
						Adjust_DSO(2, 1);
					 }
					 else 	/* 调节上下偏移 */
					 {
						g_DSO.Ch2VOffset -= 5;
					 }
				 }
				 OSSemPost(LCDRefresh); /* 发送信号量 请求刷新LCD */
				 break;

		   case  KEY_DOWN_JOY_DOWN:		/* 摇杆DOWN键按下 */
				 if (g_DSO.ActiveCH == 1) 	/* 当前激活的是CH1 */
				 {
					if (g_DSO.AdjustMode == 0)	/* 调节幅度放大倍数 */
					{
						Adjust_DSO(1, 0);
					}
					else 	/* 调节上下偏移 */
					{
						g_DSO.Ch1VOffset += 5;
					}
				 }
				 else	/* 当前激活的是CH2 */
				 {
					if (g_DSO.AdjustMode == 0)	/* 调节幅度放大倍数 */
					{
							Adjust_DSO(2, 0);
					}
					else 	/* 调节上下偏移 */
					{
							g_DSO.Ch2VOffset += 5;
					}
				  }
				 OSSemPost(LCDRefresh); /* 发送信号量 请求刷新LCD */
				 break;

			default:
				 break;
		 }
	  }
  }
}

/*******************************************************************************
* Function Name  : Display_DSO
* Description    : 刷新整个窗口
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/		
void Display_DSO(void)
{
	Display_CH_Info();	/* 显示通道信息(幅度，时间档位) */
    _DrawDSO();
}

/*******************************************************************************
* Function Name  : _Draw
* Description    : 回调函数
* Input          : - p: 参数
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void _Draw(void * p)
{
  PARAM * pParam = (PARAM *)p;
  Display_Frame();	/* 绘制刻度框 */
  
  GUI_SetColor(GUI_LIGHTYELLOW);     
  LCD_DrawPoints( (void*)pParam->CH1x, (void*)pParam->CH1y, 300 );
  GUI_SetColor(GUI_LIGHTMAGENTA);     
  LCD_DrawPoints( (void*)pParam->CH2x, (void*)pParam->CH2y, 300 );
}

/*******************************************************************************
* Function Name  : _DrawDSO
* Description    : 画示波器图形
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void _DrawDSO(void) 
{
  PARAM Param;	
  int16_t index;	
  int16_t iTemp;

  GUI_RECT Rect =  {  9, 19, 9 + 302, 19 + 202 };

  if (g_DSO.BufUsed == 0)
  {
	 g_DSO.BufUsed = 1;
  }
  else
  {
	 g_DSO.BufUsed = 0;
  }

  if (g_DSO.BufUsed == 0)
  {
	 Param.CH1x = g_DSO.xCh1Buf1;
	 Param.CH1y = g_DSO.yCh1Buf1;
	 Param.CH2x = g_DSO.xCh2Buf1;
	 Param.CH2y = g_DSO.yCh2Buf1;
  }
  else
  {
	 Param.CH1x = g_DSO.xCh1Buf2;
	 Param.CH1y = g_DSO.yCh1Buf2;
	 Param.CH2x = g_DSO.xCh2Buf2;
	 Param.CH2y = g_DSO.yCh2Buf2;
  }

  /* CH1通道, 输入0V时，ADC采样值 = 2070 */
  for (index = 0; index < 300; index++)
  {										 
	 Param.CH1x[index] = Param.CH2x[index] = 10 + index;
	 iTemp = g_DSO.Ch1VOffset - (int16_t)(( g_DSO.Ch1Buf[index + 1] - 2070 ) * 10) / g_DSO.Ch1Attenuation;

	 if (iTemp > 220)
	 {
		 iTemp = 220;
	 }
	 else if (iTemp < 20)
	 {
		 iTemp = 20;
	 }
	 Param.CH1y[index] = iTemp;

	 iTemp = g_DSO.Ch2VOffset - (int16_t)(( g_DSO.Ch2Buf[index + 1] - 2070 ) * 10) / g_DSO.Ch2Attenuation;

	 if (iTemp > 220)
	 {
		 iTemp = 220;
	 }
	 else if (iTemp < 20)
	 {
		 iTemp = 20;
	 }
	 Param.CH2y[index] = iTemp;
  }

  GUI_MEMDEV_Draw(&Rect, _Draw, &Param, 0, 0);
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/


