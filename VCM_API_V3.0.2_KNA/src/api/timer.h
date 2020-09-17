/*
 * Maintains a timer for calculating elapsed time as well as a list of periodic interrupt based timers
 *
 * TIMER_Now() returns the current time mark.  Save this, and later
 * call TIMER_Since() with that time mark.  What you get back is the number of
 * milliseconds elapsed since the time mark.  Note that the size of a 32-bit unsigned int
 * determines the maximum interval that can be timed.  With a timer tick of 10ms, the range
 * of the timer is many days.
 *
 * Client who wish to receive callbacks at a multiple of the TIMER_TICK_PERIOD
 * may call TIMER_Request().  This request remains active until TIMER_Cancel() is called.
 *
 * NOTE: tick handlers are called on an interrupt service routine context.  You should
 * be careful about what your handler changes, and also how long it takes to execute.
 *
 * Copyright (c) 2015 Control Solutions LLC. All Rights Reserved
 */

#ifndef _TIMER_H_
#define _TIMER_H_

#include "api.h"

//------------------------------//----------------------------------------------
// Definitions

// Number of microseconds in a system ticker
#define TIMER_TICK_PERIOD 1000

// Max number of clients that can register for a timer
#define TIMER_MAX_CLIENTS 10

// function prototype used for callback functions
typedef void ( *TICK_HANDLER )( void );

typedef uint32_t MS_TICK;

//------------------------------//----------------------------------------------
// Global variables

extern volatile MS_TICK TIMER_ticks;

//------------------------------//----------------------------------------------
// Public functions

/**
 * Cancel a tick request
 *
 * @param handle the function that was previously registered
 * @return void
 */
void TIMER_Cancel( TICK_HANDLER handle );

/**
 * Request to receive a periodic interrupt callback
 *
 * @param handle - function to be called when timer event occurs
 * @param rate - the number of ticks between handle calls
 * @return bool true if request was accepted
 */
bool TIMER_Request( TICK_HANDLER handle, uint32_t rate );

/**
 * Start the hardware timer
 */
void TIMER_Initialize( void );

/**
 * Returns the current tick counter
 */
#define TIMER_Now() TIMER_ticks

/**
 * Returns the number of milliseconds since the indicated start time
 */
#define TIMER_Since( start ) ( TIMER_ticks - start )

#endif
