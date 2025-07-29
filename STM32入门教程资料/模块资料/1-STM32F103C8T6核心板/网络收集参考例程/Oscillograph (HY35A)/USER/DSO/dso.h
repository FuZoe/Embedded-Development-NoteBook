/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               DSO.h
** Descriptions:            The DSO application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2011-2-20
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

#ifndef __DSO_H 
#define __DSO_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Private define ------------------------------------------------------------*/
#define ADC1_DR_Address    ((uint32_t)0x4001244C)
#define ADC2_DR_Address    ((uint32_t)0x4001284C)
#define ADC3_DR_Address    ((uint32_t)0x40013C4C)

#define Horizontal         0x00
#define Vertical           0x01

#define	SAMPLE_COUNT	   (4 * 1024) 	/* 采样深度 即最大样本个数 */

/* Private typedef -----------------------------------------------------------*/
	
/* 示波器相关的数据结构 */
typedef struct
{
  uint16_t Ch1Buf[SAMPLE_COUNT];	 /* 通道1数据缓冲区 */
  uint16_t Ch2Buf[SAMPLE_COUNT];	 /* 通道2数据缓冲区 */

  uint32_t TimeBaseIdHold;			 /* 暂停时的时基 */

  uint32_t TimeBaseId;		    	 /* 时基索引 查表可得到 us单位的时基 */
  uint32_t SampleFreq;			     /* 采样频率 单位Hz */
  uint32_t TimeBase;				 /* 时基查表可得到 us单位的时基 */

  uint8_t  Ch1AttId;				 /* CH1 衰减倍数索引 */
  uint8_t  Ch2AttId;				 /* CH2 衰减倍数索引 */
  float    Ch1Attenuation; 		     /* 波形1幅度衰减系数(原始数据x10后，再除以这个数)  */
  float    Ch2Attenuation; 		     /* 波形2幅度衰减系数(原始数据x10后，再除以这个数)  */
  uint16_t Ch1VScale;				 /* 通道1垂直分度值mV单位 */
  uint16_t Ch2VScale;				 /* 通道2垂直分度值mV单位 */

  uint32_t TriggerLevel;			 /* 触发电平(ADC采样结果比较值) */
  uint32_t TriggerUpEdge;			 /* 1表示触发模式上跳沿，0表示下跳沿 */

  uint8_t Ch1DC;					 /* 0是交流 1表示CH1是直流 */
  uint8_t Ch2DC;					 /* 0是交流 1表示CH2是直流 */

  int16_t Ch1VOffset;				 /* 通道1 GND线位置, 可以为负数 */
  int16_t Ch2VOffset;				 /* 通道1 GND线位置, 可以为负数 */

  uint8_t ActiveCH;				     /* 当前焦点通道 1表示CH1, 2表示CH2 */
  uint8_t AdjustMode;				 /* 当前调节模式，0表示调节幅度，1表示调节偏移 */

  /* 使用2个缓冲区完成波形的擦除和重现 */
  int16_t xCh1Buf1[310];		     /* 波形数据，坐标数组 */
  int16_t yCh1Buf1[310];		     /* 波形数据，坐标数组 */
  int16_t xCh1Buf2[310];		     /* 波形数据，坐标数组 */
  int16_t yCh1Buf2[310];		     /* 波形数据，坐标数组 */

  int16_t xCh2Buf1[310];		     /* 波形数据，坐标数组 */
  int16_t yCh2Buf1[310];		     /* 波形数据，坐标数组 */
  int16_t xCh2Buf2[310];		     /* 波形数据，坐标数组 */
  int16_t yCh2Buf2[310];	 	     /* 波形数据，坐标数组 */
  uint8_t BufUsed;			         /* 0表示当前用Buf1， 1表示用Buf2 */

  uint8_t HoldEn;				     /* 波形采集暂停标志 1表示暂停，0表示运行 */
}
DSO;

typedef struct 
{
   int16_t * CH1x;
   int16_t * CH1y;
   int16_t * CH2x;
   int16_t * CH2y;
} 
PARAM;
	
/* 定义支持的采样频率 */
typedef enum
{
	SR_1K = 0,	
}
SAMPLE_RATE_E;

/* 主程序状态字 */
enum
{
	S_HELP		= 0,	/* 帮助页面 */
	S_RUN		= 1,	/* 采集状态 */
	S_HOLD		= 2		/* 暂停状态 */
};

/* Private function prototypes -----------------------------------------------*/
void DSO_SampleRate(uint32_t freq);
void DSO_Initializtion(void);
void Display_Help(void);
void DSO_Help(void);
void Display_Frame(void);
void Display_CH1_Wave(void);
void Display_CH2_Wave(void);

void Display_CH_Info(void);
void Display_DSO(void);
void DSO_Configuration(void);
void Inc_SampleFreq(void);
void Dec_SampleFreq(void);
void Adjust_DSO(uint8_t ch, uint8_t mode);
void Stop_ADC(void);
void Start_ADC(void);
void DSO_Run(void);
void _DrawDSO(void) ;
void _Draw(void * p);
#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/


