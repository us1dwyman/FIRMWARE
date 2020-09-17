/**
 * Implements an interface to a system timer module
 *
 * Keeps track of time using a hardware timer.
 *
 * The hardware timer is assumed to already be set up and running
 */

#include "timer.h"
#include "system_config.h"
#include <string.h>

//------------------------------//----------------------------------------------
// Definitions

//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Local variables

// How many times has the hardware timer hit the period count?
uint16_t _rollovers = 0;

//------------------------------//----------------------------------------------
// Local functions

/**
 * Configures the system ticker rate and enables it
 *
 * Refer to header file for usage
 */
void TIMER_Initialize()
{
}

/**
 * Check for timer expirations
 *
 * This needs to be called if you are not using an interrupt-based hardware timer
 *
 */
void TIMER_Tasks( void )
{
    // Check if the Timer has hit the period match
    if( IFS0bits.T1IF )
    {
        // reset the period match
        TMR1 -= TIMER_PRELOAD;

        _rollovers++;

        IFS0bits.T1IF = false;
    }
}

/**
 * Returns the current tick counter
 * The upper byte is the rollover counter, and the lower
 * byte (number of ms) is determined by examining the hardware timer count
 *
 * Note: this is not a perfect timer, unless the preload counter is a multiple
 * of 256.  Otherwise there will be truncation errors that result in a tick that's
 * not exactly 1ms
 */
TIMER_TICK TIMER_Now( void )
{
    TIMER_Tasks();
    return ( _rollovers << 8 ) + ( TMR1 / ( TIMER_PRELOAD / 256 ) );
}

/**
 * Returns the number of ticks since the indicated start time
 */
TIMER_TICK TIMER_Since( TIMER_TICK start )
{
    return TIMER_Now() - start;
}
