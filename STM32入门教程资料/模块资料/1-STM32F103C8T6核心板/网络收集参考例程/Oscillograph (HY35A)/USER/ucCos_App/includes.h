/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               includes.h
** Descriptions:            includes.h for ucos
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-9
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


#ifndef  __INCLUDES_H__
#define  __INCLUDES_H__

/* Includes ------------------------------------------------------------------*/
#include  <stdio.h>
#include  <string.h>
#include  <ctype.h>
#include  <stdlib.h>
#include  <stdarg.h>
#include  <math.h>

#include  <uCOS-II\Source\ucos_ii.h>
#include  <uCOS-II\Ports\ARM-Cortex-M3\RealView\os_cpu.h>
#include  <uC-CPU\ARM-Cortex-M3\RealView\cpu.h>
#include  <lib_def.h>
#include  <lib_mem.h>
#include  <lib_str.h>

#if (OS_VIEW_MODULE == DEF_ENABLED)
#include    <uCOS-VIEW\Ports\ARM-Cortex-M3\Realview\os_viewc.h>
#include    <uCOS-VIEW\Source\os_view.h>
#endif

#include    <GUI.h>
#include    "TouchPanel.h"
#include    <stm32f10x.h>
#include    "LCDConf.h"
#include    "GLCD.h"

#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/


