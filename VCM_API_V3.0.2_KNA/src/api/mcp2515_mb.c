/**
 * Mailbox manager for MCP2515-based CAN
 *
 * Implements RX and TX mailboxes for CANBUS. Refer to the header file for usage.
 *
 * Copyright (c) 2019 Control Solutions LLC.  All Rights Reserved.
 */

#include "app.h"
#if defined( CAN2_ENABLE )

#include "mcp2515_mb.h"
#include "timer.h"
#include <string.h>

//------------------------------//----------------------------------------------
// Definitions

// how many milliseconds between checks to see if we need to send a message
// from a transmit mailbox. this can't be shorter than the system tick rate
#define CAN2_TX_MB_TICK 10

//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Local variables

static CAN_Mailbox_t _txMailboxes[ CAN2_TX_MAILBOXES ];

//------------------------------//----------------------------------------------
// Local functions

/**
 * Called by the systick interrupt
 *
 * This is called on an ISR context
 */
static void _Timer_ISR( void )
{

    int i;
    for( i = 0; i < CAN2_TX_MAILBOXES; i++ )
    {
        volatile CAN_Mailbox_t *pMb = &_txMailboxes[ i ];
        if( pMb->interval && ( --pMb->tickCount == 0 ) )
        {
            // signal message is to be sent
            pMb->Status = 1;
            pMb->tickCount = pMb->interval;
        }
    }
}

static void _SendNext( void )
{
    unsigned i;

    for( i = 0; i < CAN2_TX_MAILBOXES; i++ )
    {
        CAN_Mailbox_t *pMb = &_txMailboxes[ i ];

        if( pMb->Status )
        {
            if( 0 == CAN2_Tx( &pMb->msg ) )
            {
                pMb->Status = 0;
            }
        }
    }
};

//------------------------------//----------------------------------------------
// Public functions

void MCP2515MB_UpdateTxMailbox( unsigned index, uint8_t *pData, uint8_t dlc, bool bSendNow )
{
    if( index < CAN2_TX_MAILBOXES && dlc <= CAN_MAX_DLC )
    {
        CAN_Mailbox_t *pMb = &_txMailboxes[ index ];
        memcpy( pMb->msg.buf, pData, dlc );
        pMb->msg.buf_len = dlc;

        if( bSendNow )
        {
            pMb->Status = 1;
        }
    }
}

void MCP2515MB_EnableTxMailbox( unsigned index, uint32_t id, uint16_t interval )
{
    if( index < CAN2_TX_MAILBOXES )
    {
        volatile CAN_Mailbox_t *pMb = &_txMailboxes[ index ];
        pMb->interval = interval / CAN2_TX_MB_TICK;
        pMb->tickCount = pMb->interval;
        pMb->msg.id = id;
    }
}

void MCP2515MB_DisableTxMailbox( unsigned index )
{
    if( index < CAN2_TX_MAILBOXES )
    {
        volatile CAN_Mailbox_t *pMb = &_txMailboxes[ index ];
        pMb->Status = 0;
        pMb->interval = 0;
    }
}

/**
 * Powerup Initialization
 */
void MCP2515MB_Initialize( void )
{

    unsigned i;

    // Initialize TX mailboxes
    for( i = 0; i < CAN2_TX_MAILBOXES; i++ )
    {
        _txMailboxes[ i ].Status = 0;
        _txMailboxes[ i ].interval = 0;
    }

    // Request a periodic interrupt
    TIMER_Request( &_Timer_ISR, CAN2_TX_MB_TICK );
}

/**
 * Module task loop
 */
void MCP2515MB_Tasks( void )
{
    _SendNext();
}

#endif
