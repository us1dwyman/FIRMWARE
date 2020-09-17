/**
 * Hardware management interface
 *
 * Handles
 *
 * Copyright (c) 2019 Control Solutions LLC.  All Rights Reserved.
 */

#ifndef __SYSTEM_H_
#define __SYSTEM_H_

#include "system_config.h"

//------------------------------//----------------------------------------------
// Definitions

#define SYSTEM_EnableInterrupts()                                                                                       \
    INTCONbits.GIE = 1;                                                                                                 \
    INTCONbits.PEIE = 1;                                                                                                \
    PIE0bits.IOCIE = 1;                                                                                                 \
    PIR0bits.IOCIF = 0;                                                                                                 \
    INTCONbits.INTEDG = 0;                                                                                              \
    PIE0bits.TMR0IE = 1;                                                                                                \
    TMR1IE = 1;                                                                                                         \
    T1CONbits.TMR1ON = 1

//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Public functions

/**
 * @brief      Hardware powerup initialization
 */
void SYSTEM_Initialize( void );

/**
 * @brief      Hardware to low-power mode
 *
 * This function does not exit until a valid pin-change interrupt is detected
 */
void SYSTEM_Sleep( void );

/**
 * @brief      Undo the low-power mode
 *
 */
void SYSTEM_Wakeup( void );

#endif
