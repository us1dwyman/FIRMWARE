/**
 * Implements a serial console
 *
 * This is commonly used for debugging purposes.
 *
 * Transmit and receive are buffered; should the buffers overflow the
 * newest data is silently discarded.
 *
 * Initialization (or flush/re-initialization) is done via
 *  CONSOLE_Initialize();
 *
 * Copyright (c) 2015 Control Solutions LLC. All Rights Reserved
 */

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <stdbool.h>

/**
 * Initializes the console
 *
 * This initializes / clears the Console
 *
 */
bool CONSOLE_Initialize( void );

/**
 * Task runner
 *
 * Call this periodically to check for new commands and process them.
 * You need to call it frequently enough that the console buffers don't overflow.
 */
void CONSOLE_Tasks( void );

/**
 * Does not return until the TX buffer is empty
 */
void CONSOLE_WaitForTxIdle( void );

#endif
