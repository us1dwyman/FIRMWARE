/*
 * Maintains a timer for calculating elapsed time as well as a list of periodic interrupt based timers
 *
 * TIMER_Now() returns the current time mark.  Save this, and later
 * call TIMER_Since() with that time mark.  What you get back is the number of
 * ticks elapsed since the time mark.  Note that the size of a TIMER_TICK
 * determines the maximum interval that can be timed.  With a timer tick of 10ms, and a 32-bit TIMER_TICK,
 * the range of the timer is many days.
 *
 *
 * Copyright (c) 2015 Control Solutions LLC. All Rights Reserved
 */

#ifndef _TIMER_H_
#define _TIMER_H_

#include "app.h"

//------------------------------//----------------------------------------------
// Definitions

// Number of microseconds in a system ticker
#define TIMER_TICK_PERIOD 1000

typedef uint16_t TIMER_TICK;

//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Public functions

/**
 * Start the hardware timer
 */
void TIMER_Initialize( void );

/**
 * Check for timer expirations
 *
 * This needs to be called if you are not using an interrupt-based hardware timer
 *
 */
void TIMER_Tasks( void );

/**
 * Returns the current tick counter
 */
TIMER_TICK TIMER_Now( void );

/**
 * Returns the number of ticks since the indicated start time
 */
TIMER_TICK TIMER_Since( TIMER_TICK start );

#endif
