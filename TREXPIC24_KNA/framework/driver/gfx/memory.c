/******************************************************************************
 *
 * Description: These functions are the pages that will be called
 *
 * Author:      us1jkaze
 * Created:     February 27, 2019, 3:26 PM
 *
 * Changed:     
 * Changed by:  
 * Revision:    
 *
 * Copyright (C) 2013-2017 by Alfred Kaercher GmbH & Co. KG.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 ******************************************************************************/

#include "memory.h"

void Memory_Write(unsigned long *prog_address, unsigned int prog_data_low, unsigned char prog_data_high)                
{                    // C example using MPLAB C30
    progAddr = *prog_address           // Address of word to program
    progDataL = prog_data_low;             // Data to program lower word
    progDataH = prog_data_high;              // Data to program upper byte
                    //Set up NVMCON for word programming
 NVMCON = 0x4003;                             // Initialize NVMCON
                    //Set up pointer to the first memory location to be written
 TBLPAG = progAddr>>16;                       // Initialize PM Page Boundary SFR 
 offset = progAddr & 0xFFFF;                  // Initialize lower word of address
                    //Perform TBLWT instructions to write latches
 __builtin_tblwtl(offset, progDataL);         // Write to address low word
 __builtin_tblwth(offset, progDataH);         // Write to upper byte
 asm(“DISI #5”);                              // Block interrupts with priority <7
                                              // for next 5 instructions
 __builtin_write_NVM();                       // C30 function to perform unlock
                                              // sequence and set WR
}