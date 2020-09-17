/**
 * Tests the CAN2 interface
 *
 * Sets up periodic TX messages and listens for RX messages
 *
 */

#include "api.h"
#include "KNA/test.h"

#include "timer.h"
#include <stdlib.h>

//------------------------------//----------------------------------------------
// Definitions

//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Local variables

static MS_TICK _lastSentPacket;

//------------------------------//----------------------------------------------
// Local functions

//------------------------------//----------------------------------------------
// Public functions

/**
 * @brief      Callback function that receives each incoming message on CAN2
 *
 * @param      pMsg  pointer to the message
 */
void APICB_Can2Rx( can_t *pMsg )
{
    switch( pMsg->id )
    {
        // don't forget the EFF flag will be set for 29-bit messages
        case CAN_EFF_FLAG | 0x10EF8090:
            printf( "CAN2 RX %08lX\r\n", pMsg->id );
            break;

        case CAN_PING:
            // respond to the ping; we keep the first byte and increment
            // the second, then send back
            pMsg->buf[ 1 ]++;
            pMsg->id = CAN_PONG;
            if( !API_SendCan2Message( pMsg ) )
            {
                printf( "Tx failed: %02X\r\n", pMsg->buf[ 1 ] );
            }
            break;

        default:
            break;
    }
}

/**
 * Initialize the test runner
 */
void Can2_Initialize( void )
{
#if defined( CAN2_ENABLE )

    API_EnableCan2();

    _lastSentPacket = TIMER_Now();

    // first, initialize the data portion of the mailbox.
    // use a pattern, but a different pattern for each mailbox so
    // when we see the data on the CANBUS sniffer, we can see
    // that all mailboxes are working
    uint8_t buf[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    API_Can2_UpdateTxMailbox( 0, &buf[ 0 ], 8, false );
    API_Can2_UpdateTxMailbox( 1, &buf[ 1 ], 7, false );
    API_Can2_UpdateTxMailbox( 2, &buf[ 2 ], 6, false );
    API_Can2_UpdateTxMailbox( 3, &buf[ 3 ], 5, false );
    API_Can2_UpdateTxMailbox( 4, &buf[ 4 ], 4, false );
    API_Can2_UpdateTxMailbox( 5, &buf[ 5 ], 3, false );
    API_Can2_UpdateTxMailbox( 6, &buf[ 6 ], 2, false );
    API_Can2_UpdateTxMailbox( 7, &buf[ 7 ], 1, false );

    // now, configure the mailboxes to send periodically
    API_Can2_EnableTxMailbox( 0, 0x500, 50 );
    API_Can2_EnableTxMailbox( 1, CAN_EFF_FLAG | 0x10FF0100 | CANADDR_VCM, 50 );
    API_Can2_EnableTxMailbox( 2, CAN_EFF_FLAG | 0x10FF0200 | CANADDR_VCM, 100 );
    API_Can2_EnableTxMailbox( 3, CAN_EFF_FLAG | 0x10FF0300 | CANADDR_VCM, 150 );
    API_Can2_EnableTxMailbox( 4, CAN_EFF_FLAG | 0x10FF0400 | CANADDR_VCM, 200 );
    API_Can2_EnableTxMailbox( 5, CAN_EFF_FLAG | 0x10FF0500 | CANADDR_VCM, 250 );
    API_Can2_EnableTxMailbox( 6, CAN_EFF_FLAG | 0x10FF0600 | CANADDR_VCM, 500 );

    // the last one is configured, but sends only when commanded to do so
    API_Can2_EnableTxMailbox( 7, CAN_EFF_FLAG | 0x10FF0700 | CANADDR_VCM, 0 );

#endif
}

/**
 * Run the task loop for the test
 *
 * For this to do anything useful, API_EnableCan2() has to be called (once)
 */
void TEST_Can2_Tasks( void )
{
#if defined( CAN2_ENABLE )
    if( TIMER_Since( _lastSentPacket ) >= 5000 )
    {
        _lastSentPacket = TIMER_Now();

        uint8_t buf[] = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02};

        // Update mailbox 7 and send ASAP
        API_Can2_UpdateTxMailbox( 7, buf, 8, true );

        // Also queue an arbitrary message for ASAP transmission
        can_t msg = {
            0x601,                     // id
            0,                         // not used
            5,                         // data length
            {'H', 'e', 'l', 'l', 'o'}  // data
        };

       API_SendCan2Message( &msg );
    }
#endif
}
