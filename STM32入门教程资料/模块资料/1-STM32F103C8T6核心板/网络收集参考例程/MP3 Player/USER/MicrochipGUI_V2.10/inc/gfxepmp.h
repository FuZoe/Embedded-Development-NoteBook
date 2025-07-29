/*****************************************************************************
 *  Module for Microchip Graphics Library
 *  EPMP driver
 *****************************************************************************
 * FileName:        gfxepmp.h
 * Dependencies:    Graphics.h
 * Processor:       PIC24, PIC32
 * Compiler:       	MPLAB C30, MPLAB C32
 * Linker:          MPLAB LINK30, MPLAB LINK32
 * Company:         Microchip Technology Incorporated
 *
 * Software License Agreement
 *
 * Copyright © 2010 Microchip Technology Inc.  All rights reserved.
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
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Anton Alkhimenok     01/12/10
 *****************************************************************************/
#ifndef _GFX_EPMP_H_FILE
#define _GFX_EPMP_H_FILE

#include "Graphics\Graphics.h"
#include "Compiler.h"

#ifdef USE_GFX_EPMP

#ifdef USE_16BIT_PMP
extern volatile __eds__ WORD __attribute__((space(eds),address(0x00020000ul),noload)) pmp_data;
#else
extern volatile __eds__ BYTE __attribute__((space(eds),address(0x00020000ul),noload)) pmp_data;
#endif

/*********************************************************************
* Macros:  PMPWaitBusy()
*
* Overview: waits for PMP cycle end.
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
#define PMPWaitBusy()   while(PMCON2bits.BUSY); 

/*********************************************************************
* Function:  void DeviceSetCommand()
*
* Overview: set RS line to access a control register space
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
extern inline void __attribute__ ((always_inline)) DeviceSetCommand(){
	RS_LAT_BIT = 0;
}

/*********************************************************************
* Function:  void DeviceSetData()
*
* Overview: set RS line to access a data space
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
extern inline void __attribute__ ((always_inline)) DeviceSetData(){
	RS_LAT_BIT = 1;
}

/*********************************************************************
* Function:  void DeviceSelect()
*
* Overview: asserts the chip select line
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
extern inline void __attribute__ ((always_inline)) DeviceSelect(){
	CS_LAT_BIT = 0;
}

/*********************************************************************
* Function:  void DeviceDeselect()
*
* Overview: puts the chip select line in inactive state
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
extern inline void __attribute__ ((always_inline)) DeviceDeselect(){
	CS_LAT_BIT = 1;
}

/*********************************************************************
* Macros:  DeviceWrite(data)
*
* PreCondition:  none
*
* Input: data - value to be written to RAM
*
* Output: none
*
* Side Effects: none
*
* Overview: writes data into controller's RAM
*
* Note: chip select should be enabled
*
********************************************************************/
#ifdef USE_16BIT_PMP

extern inline void __attribute__ ((always_inline)) DeviceWrite(WORD data) 
{ 
	pmp_data = data;
	PMPWaitBusy();
}

#else

extern inline void __attribute__ ((always_inline)) DeviceWrite(BYTE data)
{
	pmp_data = data;
	PMPWaitBusy();
}

#endif

/*********************************************************************
* Macros:  DeviceRead()
*
* PreCondition:  none
*
* Input: none
*
* Output: data read
*
* Side Effects: none
*
* Overview: reads data from controller's RAM
*
* Note: chip select should be enabled
*
********************************************************************/
#ifdef USE_16BIT_PMP

extern inline WORD __attribute__ ((always_inline)) DeviceRead()
{
WORD value;
	value = pmp_data;
	PMPWaitBusy();
	return PMDIN1;
}

#else


extern inline BYTE __attribute__ ((always_inline)) DeviceRead(){
BYTE value;
	value = pmp_data;
	PMPWaitBusy();
	return PMDIN1;
}

#endif


/*********************************************************************
* Function:  DeviceInit()
*
* PreCondition:  none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: initializes the device
*
* Note: none
*
********************************************************************/

extern inline void __attribute__ ((always_inline)) DeviceInit(void)
{ 
	RST_LAT_BIT = 0;            // hold in reset by default
    RST_TRIS_BIT = 0;           // enable RESET line
    RS_TRIS_BIT = 0;            // enable RS line
    CS_LAT_BIT = 1;             // not selected by default
    CS_TRIS_BIT = 0;            // enable chip select line

	ANSDbits.ANSD7 = 0;			// PMD15
	ANSDbits.ANSD6 = 0;			// PMD14
	ANSFbits.ANSF0 = 0;			// PMD11

	ANSBbits.ANSB15 = 0;		// PMA0
	ANSBbits.ANSB14 = 0;		// PMA1
	ANSGbits.ANSG9  = 0;		// PMA2
	ANSBbits.ANSB13 = 0;		// PMA10
	ANSBbits.ANSB12 = 0;		// PMA11
	ANSBbits.ANSB11 = 0;		// PMA12
	ANSBbits.ANSB10 = 0;		// PMA13
	ANSAbits.ANSA7 = 0;			// PMA17
	ANSGbits.ANSG6 = 0;			// PMA18

	PMCON1bits.ADRMUX = 0;	    // address is not multiplexed
	PMCON1bits.MODE = 3;        // master mode
	PMCON1bits.CSF = 0;         // PMCS1 pin used for chip select 1, PMCS2 pin used for chip select 2
	PMCON1bits.ALP = 0;         // set address latch strobes to high active level (for sn74lvc16373)
	PMCON1bits.ALMODE = 0;      // "smart" address strobes are not used
	PMCON1bits.BUSKEEP = 1;     // bus keeper is used
	
	PMCS1BS = 0x0200; 	        // CS1 start address
	PMCS2BS = 0xff00;			// set CS1 end address and CS2 start address
	PMCON2bits.RADDR = 0xff;	// set CS2 end address

	PMCON4 = 0;                 // PMA0 - PMA15 address lines are disabled
	PMCON3 |= 0;                // PMA16 - PMA17 address line is disabled

	PMCS2CFbits.CSDIS = 1;       // disable CS2 functionality  

	PMCS1CFbits.CSDIS = 0;      // enable CS
	PMCS1CFbits.CSP = 0;        // CS active low
	PMCS1CFbits.CSPTEN = 0;     // disable CS port
	PMCS1CFbits.BEP = 0;        // byte enable active low
	PMCS1CFbits.WRSP = 0;       // write strobe active low
	PMCS1CFbits.RDSP = 0;       // read strobe active low
	PMCS1CFbits.SM = 0;         // read and write strobes on separate lines 

#ifdef USE_16BIT_PMP
	PMCS1CFbits.PTSZ = 2;       // data bus width is 16-bit 
#else
	PMCS1CFbits.PTSZ = 0;       // data bus width is 8-bit 
#endif

	PMCS1MDbits.ACKM = 0;        // PMACK is not used
	PMCS1MDbits.DWAITB = 0;      
	PMCS1MDbits.DWAITM = 2;
	PMCS1MDbits.DWAITE = 0;
	
	PMCON3bits.PTWREN = 1;      // enable write strobe port
	PMCON3bits.PTRDEN = 1;      // enable read strobe port
	PMCON3bits.PTBE0EN = 0;     // disable byte enable port
	PMCON3bits.PTBE1EN = 0;     // disable byte enable port
	PMCON3bits.AWAITM = 0;      // don't care
	PMCON3bits.AWAITE = 0;      // don't care
	
	PMCON1bits.PMPEN = 1;

    DelayMs(40);
    RST_LAT_BIT = 1;            // release from reset
    DelayMs(40);
}

#endif // USE_GFX_EPMP
#endif
