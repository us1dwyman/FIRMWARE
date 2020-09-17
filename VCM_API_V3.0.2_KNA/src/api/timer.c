/**
 * Implements an interface to a system timer module
 *
 * Keeps track of time in milliseconds
 */

#include "timer.h"
#include "mcc_generated_files/tmr3.h"
#include <string.h>

//------------------------------//----------------------------------------------
// Definitions

typedef struct
{
    TICK_HANDLER handle;
    uint32_t rate;
    uint32_t count;
} TICK_REQUEST;

//------------------------------//----------------------------------------------
// Global variables

MS_TICK volatile TIMER_ticks = 0;

//------------------------------//----------------------------------------------
// Local variables
static TICK_REQUEST requests[ TIMER_MAX_CLIENTS ];
static bool configured = false;

//------------------------------//----------------------------------------------
// Local functions

/**
 * Called by the systick interrupt
 */
static void _TimerISR( void )
{
    // Increment the system time ticker
    TIMER_ticks += ( TIMER_TICK_PERIOD / 1000 );

    uint8_t i;

    // Look for clients whose timer has expired
    for( i = 0; i < TIMER_MAX_CLIENTS; i++ )
    {
        if( requests[ i ].handle != NULL )
        {
            requests[ i ].count++;

            if( requests[ i ].count == requests[ i ].rate )
            {
                // call the client's handler
                requests[ i ].handle();
                requests[ i ].count = 0;
            }
        }
    }
}

//------------------------------//----------------------------------------------
// Public functions

/**
 * Cancel a tick request
 *
 * Refer to header file for usage
 */
void TIMER_Cancel( TICK_HANDLER handle )
{
    uint8_t i;

    for( i = 0; i < TIMER_MAX_CLIENTS; i++ )
    {
        if( requests[ i ].handle == handle )
        {
            requests[ i ].handle = NULL;
        }
    }
}

/**
 * Request to receive a periodic interrupt callback
 *
 * Refer to header file for usage
 */
bool TIMER_Request( TICK_HANDLER handle, uint32_t rate )
{
    uint8_t i;

    if( configured == false )
    {
        return false;
    }

    for( i = 0; i < TIMER_MAX_CLIENTS; i++ )
    {
        if( requests[ i ].handle == NULL )
        {
            requests[ i ].handle = handle;
            requests[ i ].rate = rate;
            requests[ i ].count = 0;

            return true;
        }
    }

    return false;
}

/**
 * Configures the system ticker rate and enables it
 *
 * Refer to header file for usage
 */
void TIMER_Initialize()
{
    memset( requests, 0, sizeof( requests ) );

    TMR3_SetInterruptHandler( _TimerISR );
    
    configured = true;
}
