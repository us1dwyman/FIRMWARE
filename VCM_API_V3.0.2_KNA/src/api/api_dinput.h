/**
 * @file api_dinput.h
 * @brief Defines an interface to debounced discrete inputs
 *
 * This module samples a number of digital inputs, and evaluates their
 * current state.  A pin is determined to have changed state only when it
 * has been at a stable value for a number of samples.
 *
 * @copyright Copyright (c) 2018 Control Solutions LLC.  All Rights Reserved.
 *
 */
#ifndef _API_DINPUT_H_
#define _API_DINPUT_H_

#include "api.h"

//------------------------------//----------------------------------------------
// Definitions

/**
 * Defines a structure containing the state of debounced inputs
 *
 */
typedef union {
    uint16_t word;
    struct
    {
        unsigned int interlock : 1;
        unsigned int oppres1 : 1;
        unsigned int oppres2 : 1;
        unsigned int brkrel : 1;
        unsigned int gpin01 : 1;
        unsigned int dmodfault : 1;
        unsigned int estop : 1;
        unsigned int keysw : 1;
        unsigned int csled : 1;
        unsigned int reserved : 7;
    } bits;

} ApiInputs_t;

/**
 * Definition of Operator Presence Switch state
 */
typedef enum
{
    OPERATOR_INVALID1 = 0b00,
    OPERATOR_ON = 0b01,
    OPERATOR_OFF = 0b10,
    OPERATOR_INVALID2 = 0b11,
    OPERATOR_NO_EVENT = 0b100,
} API_OpPresence_e;

//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Public functions

/** \defgroup discreteinputs Discrete Inputs
 * This section allows reading various discrete inputs.
 *
 * The pins managed by this module are initialized as inputs, and polled
 * periodically.  Debouncing is done by taking a number of samples, and declaring
 * a pin state only when several matching samples have been taken.
 * The debounced state is stored in the global variable API_dinputs.
 *
 */

/** \ingroup    discreteinputs
 * Return the value of all digital inputs
 *
 * @return     boolean true if the pin is active
 */
uint16_t API_GetDinputWord( void );

/**
 * Powerup Initialization
 * @private
 */
void API_Dinput_Initialize( void );

/**
 * Module task loop
 * @private
 */
void API_Dinput_Tasks( void );

#endif
