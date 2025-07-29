/*****************************************************************************
 *  Module for Microchip Graphics Library
 *  Palette Support
 *****************************************************************************
 * FileName:        Palette.h
 * Dependencies:    Graphics.h
 * Processor:       PIC24, PIC32
 * Compiler:       	MPLAB C30, MPLAB C32
 * Linker:          MPLAB LINK30, MPLAB LINK32
 * Company:         Microchip Technology Incorporated
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
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Pradeep Budagutta    11/06/09    Initial Release
 *****************************************************************************/
#ifndef _PALETTE_H
    #define _PALETTE_H

    #include "Graphics.h"

    #ifdef USE_PALETTE

typedef union
{
    WORD    value;          //16 bits
    struct
    {
        BYTE    r : 5;
        BYTE    g : 6;
        BYTE    b : 5;
    } color;

    struct
    {
        BYTE    luma : 4;
    } monchrome;

} PALETTE_ENTRY;

typedef struct
{
    WORD            id;
    WORD            length;
} PALETTE_HEADER;

typedef struct
{
    SHORT           type;   // must be FLASH
    PALETTE_HEADER  header;
    PALETTE_ENTRY   *pPaletteEntry;
} PALETTE_FLASH;

#define PALETTE_EXTERNAL   EXTDATA     /* PALETTE_EXTERNAL = { PALETTE_HEADER, PALETTE_ENTRIES } */

/*********************************************************************
* Function: void PaletteInit(void)
*
* Overview: Initializes the CLUT.
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: Drawing mode will change to support palettes
*
********************************************************************/
void    PaletteInit(void);

/*********************************************************************
* Function: void EnablePalette(void)
*
* Overview: Enables the Palette mode
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects:
*
********************************************************************/
void    EnablePalette(void);

/*********************************************************************
* Function: void DisablePalette(void)
*
* Overview: Disables the Palette mode
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects:
*
********************************************************************/
void    DisablePalette(void);

/*********************************************************************
* Function: BYTE IsPaletteEnabled(void)
*
* Overview: Returns if the Palette mode is enabled or not
*
* PreCondition: none
*
* Input: none
*
* Output: Enabled -> 1, Disabled -> 0
*
* Side Effects:
*
********************************************************************/
BYTE    IsPaletteEnabled(void);

/*********************************************************************
* Function: BYTE GetPaletteChangeError(void)
*
* Overview: Returns the Palette change error status
*
* PreCondition: none
*
* Input: none
*
* Output: NoError -> Zero; Error -> Non Zero
*
* Side Effects: none
*
********************************************************************/
BYTE    GetPaletteChangeError(void);

/*********************************************************************
* Function: void ClearPaletteChangeError(void)
*
* Overview: Clears the Palette change error status
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
void    ClearPaletteChangeError(void);

/*********************************************************************
* Function: BYTE SetPaletteBpp(BYTE bpp)
*
* Overview: Sets the CLUT's number of valid entries.
*
* PreCondition: PaletteInit() must be called before.
*
* Input: bpp -> Bits per pixel
*
* Output: Status: Zero -> Success, Non-zero -> Error.
*
* Side Effects: Drawing mode will change to support palettes
*
********************************************************************/
BYTE    SetPaletteBpp(BYTE bpp);

/*********************************************************************
* Function: void RequestPaletteChange(void *pPalette, WORD startEntry, WORD length)
*
* Overview: Loads the palettes from the flash during vertical blanking period
*           if possible, otherwise loads immediately.
*
* PreCondition: PaletteInit() must be called before.
*
* Input: pPalette   - Pointer to the palette structure
*        startEntry - Start entry to load (inclusive)
*        length     - Number of entries
*
* Output: none
*
* Side Effects: There may be a slight flicker when the Palette entries
*               are getting loaded one by one.
*
********************************************************************/
void    RequestPaletteChange(void *pPalette, WORD startEntry, WORD length);

/*********************************************************************
* Macro: RequestEntirePaletteChange(pPalette)
*
* Overview: Loads all the palette entries from the flash during
*           vertical blanking period if possible, otherwise
*           loads immediately.
*
* PreCondition: PaletteInit() must be called before.
*
* Input: pPalette - Pointer to the palette structure
*
* Output: none
*
* Side Effects: There may be a slight flicker when the Palette entries
*               are getting loaded one by one.
*
********************************************************************/
        #define RequestEntirePaletteChange(pPalette)    RequestPaletteChange(pPalette, 0, 256)

/*********************************************************************
* Function: BYTE SetPalette(void *pPalette, WORD startEntry, WORD length)
*
* Overview: Loads the palettes from the flash immediately.
*
* PreCondition: PaletteInit() must be called before.
*
* Input: pPalette   - Pointer to the palette structure
*        startEntry - Start entry to load (inclusive)
*        length     - Number of entries
*
* Output: Status: Zero -> Success, Non-zero -> Error.
*
* Side Effects: There may be a slight flicker when the Palette entries
*               are getting loaded one by one.
*
********************************************************************/
BYTE    SetPalette(void *pPalette, WORD startEntry, WORD length);

/*********************************************************************
* Macro: SetEntirePalette(pPalette)
*
* Overview: Loads all the palette entries from the flash immediately.
*
* PreCondition: PaletteInit() must be called before.
*
* Input: pPalette - Pointer to the palette structure
*
* Output: Status: Zero -> Success, Non-zero -> Error.
*
* Side Effects: There may be a slight flicker when the Palette entries
*               are getting loaded one by one.
*
********************************************************************/
        #define SetEntirePalette(pPalette)  SetPalette(pPalette, 0, 256)

/*********************************************************************
* Function: BYTE SetPaletteFlash(PALETTE_ENTRY *pPaletteEntry, WORD startEntry, WORD length)
*
* Overview: Loads the palettes from the flash immediately.
*
* PreCondition: PaletteInit() must be called before.
*
* Input: pPaletteEntry   - Pointer to the palette table in ROM
*        startEntry      - Start entry to load (inclusive)
*        length          - Number of entries
*
* Output: Status: Zero -> Success, Non-zero -> Error.
*
* Side Effects: There may be a slight flicker when the Palette entries
*               are getting loaded one by one.
*
********************************************************************/
BYTE    SetPaletteFlash(PALETTE_ENTRY *pPaletteEntry, WORD startEntry, WORD length);

    #endif //USE_PALETTE
#endif
