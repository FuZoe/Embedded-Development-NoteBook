/*****************************************************************************
 *  Module for Microchip Graphics Library
 *  Standard color defines
 *****************************************************************************
 * FileName:            PaletteColorDefines.h
 * Dependencies:    p24Fxxxx.h or plib.h
 * Processor:           PIC24F
 * Compiler:       	MPLAB C30
 * Linker:                MPLAB LINK30
 * Company:            Microchip Technology Incorporated
 *
 * Software License Agreement
 *
 * Copyright © 2008 Microchip Technology Inc.  All rights reserved.
 * Microchip licenses to you the right to use, modify, copy and distribute
 * Software only when embedded on a Microchip microcontroller or digital
 * signal controller, which is integrated into your product or third party
 * product (pursuant to the sublicense terms in the accompanying license
 * agreement).  
 *
 * You should refer to the license agreement accompanying this Software
 * for additional information regarding your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
 * OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
 * PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR
 * OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,
 * BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT
 * DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL,
 * INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
 * COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY
 * CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF),
 * OR OTHER SIMILAR COSTS.
 *
 * Author                                    Date                               Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Pradeep Budagutta                20 Aug 2009                 Initial Version
 *****************************************************************************/
#ifndef _PALETTECOLORDEFINES_H
#define _PALETTECOLORDEFINES_H

#include "GraphicsConfig.h"

/*********************************************************************
* Overview: Some basic colors definitions.
*********************************************************************/
#ifdef USE_PALETTE

    #define BLACK               0
    #define BRIGHTBLUE          1
    #define BRIGHTGREEN         2
    #define BRIGHTCYAN          3
    #define BRIGHTRED           4
    #define BRIGHTMAGENTA       5
    #define BRIGHTYELLOW        6
    #define BLUE                7
    #define GREEN               8
    #define CYAN                9
    #define RED                 10
    #define MAGENTA             11
    #define BROWN               12
    #define LIGHTGRAY           13
    #define DARKGRAY            14
    #define LIGHTBLUE           15
    #define LIGHTGREEN          16
    #define LIGHTCYAN           17
    #define LIGHTRED            18
    #define LIGHTMAGENTA        19
    #define YELLOW              20
    #define WHITE               21
    
    #define GRAY0       	    22
    #define GRAY1         	    23
    #define GRAY2               24
    #define GRAY3               25
    #define GRAY4               26
    #define GRAY5               27
    #define GRAY6	            28

    #define GRAY20              29
    #define GRAY40              30
    #define GRAY80              31
    #define GRAY90              32
    #define GRAY95              33
    #define RED4                34
    #define FIREBRICK1          35
    #define DARKGREEN           36
    #define PALEGREEN           37
    #define LIGHTYELLOW         38
    #define GOLD                39
    #define DARKORANGE          40

    // Brown color scheme
    #define SADDLEBROWN         41
    #define SIENNA              42
    #define PERU                43
    #define BURLYWOOD           44
    #define WHEAT               45
    #define TAN                 46
    #define GRAY80              31
    #define GRAY90              32
    #define GRAY95              33

    /* default settings for TRULY display */

// Emboss dark color default value.
    #define EMBOSSDKCOLORDEFAULT            61

// Emboss light color default value.
    #define EMBOSSLTCOLORDEFAULT            62

// Text color 0 default value.
    #define TEXTCOLOR0DEFAULT               63

// Text color 1 default value.
    #define TEXTCOLOR1DEFAULT               WHITE

// Text color disabled default value.
    #define TEXTCOLORDISABLEDDEFAULT        WHEAT

// Color 0 default value.								
    #define COLOR0DEFAULT                   64

// Color 1 default value.
    #define COLOR1DEFAULT                   65

// Disabled color default value.
    #define COLORDISABLEDDEFAULT            66

// Common background color default value.
    #define COMMONBACKGROUNDCOLORDEFAULT    67

// Chart color definitions
    #define CH_CLR0             68
    #define CH_CLR1             69
    #define CH_CLR2             70
    #define CH_CLR3             71
    #define CH_CLR4             72
    #define CH_CLR5             73
    #define CH_CLR6             74
    #define CH_CLR7             75
    #define CH_CLR8             76
    #define CH_CLR9             77
    #define CH_CLR10            78
    #define CH_CLR11            79
    #define CH_CLR12            80
    #define CH_CLR13            81
    #define CH_CLR14            82
    #define CH_CLR15            83

#endif // USE_PALETTE

#endif // _PALETTECOLORDEFINES_H
