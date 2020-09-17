/**
 * CANBUS module for CAN Interface(s)
 *
 * Provides API functions for accessing the CAN hardware.  Refer to the header file
 * for operational details.
 *
 * CAN1 uses the on-processor CAN peripheral and supports a send/receive message model.
 * Messages related to Phoenix motor controllers are passed to the api_canmotor module;
 * other incoming messages are passed to the API callback.
 *
 * Outgoing messages are queued using the first-available buffer in the hardware.
 * If there are no buffers available, the send function call fails.
 *
 * CAN2 uses an off-board CAN device connected via SPI bus.  A mailbox scheme is used
 * with this chip; a set of mailboxes is defined, with a transmission schedule.
 * A hardware timer schedules each buffer for transmission; however the actual load of
 * the buffer is performed in the task loop.
 *
 * Incoming messages on CAN2 are passed to the handler function.
 *
 * Copyright (c) 2018 Control Solutions LLC.  All Rights Reserved.
 */

#include "api_can.h"
#include "api.h"
#include "api_canmotor.h"
#include "can.h"
#include "mcc_generated_files/can1.h"
#include "mcp2515_can.h"
#include "mcp2515_mb.h"
#include "timer.h"
#include <string.h>
#include "../KNA/HMI_interface.h"

//------------------------------//----------------------------------------------
// Definitions
//

//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Local variables

// Error counts for system debugging
static volatile unsigned int _txErrorPassive = 0;
static volatile unsigned int _rxErrorPassive = 0;
static volatile unsigned int _rxMessageCount = 0;
static volatile unsigned int _rxBufferOverflow = 0;

#define CAN_REVERSE( VALUE )                                                                                           \
    ( VALUE >> 24 ) | ( ( VALUE >> 8 ) & 0X0000FF00 ) | ( ( VALUE << 8 ) & 0X00FF0000 ) | ( VALUE << 24 )
#define CAN_MASK29( VALUE )                                                                                            \
    CAN_REVERSE( ( ( ( ( VALUE >> 13 ) & 0XFFE0 ) | ( ( VALUE >> 16 ) & 0X03 ) ) << 16 ) | ( VALUE & 0XFFFF ) )
#define CAN_FILTER29( VALUE ) ( CAN_MASK29( VALUE ) | 0X0800 )
#define CAN_MASK11( VALUE ) CAN_REVERSE( ( VALUE << 21 ) )
#define CAN_FILTER11( VALUE ) CAN_MASK11( VALUE )

/**
 * Configuration for the SPI (CAN2) driver
 */
static const MCP2515_Config_t _can2Config = {
    // masks.  Note each mask corresponds to a specific receive buffer.
    // RB0 is more flexible, in that if a second message arrives for RB0 before
    // it has been read, it will roll the message over into RB1 rather than lose it.
    {
        CAN_MASK11( 0X7FFUL ),      // RB0 mask for a specific 11-bit id
        CAN_MASK29( 0X0000FF00UL )  // RB1 mask for J1939 message to a specific ID
    },

    // filters
    {
        // first 2 filters apply to RB0
        CAN_FILTER11( CAN_PING ),  // filter 11-bit id
        0UL,                       // not used

        // next 4 filters apply to RB1
        CAN_FILTER29( CANADDR_VCM << 8U ),  // filter message to VCM
        0UL,                                // not used
        0UL,                                // not used
        0UL                                 // not used
    }

};

//------------------------------//----------------------------------------------
// Local functions

//------------------------------//----------------------------------------------
// Public functions

/**
 * Callback functions for PIC24 CAN driver (CAN1)
 */
void CAN1_CallbackBusOff( void )
{
}
void CAN1_CallbackTxErrorPassive( void )
{
    _txErrorPassive++;
}
void CAN1_CallbackRxErrorPassive( void )
{
    _rxErrorPassive++;
}
void CAN1_CallbackMessageReceived( void )
{
    _rxMessageCount++;
}
void CAN1_CallbackRxBufferOverflow()
{
    _rxBufferOverflow++;
}

/**
 * Powerup Initialization
 */
void API_Can_Initialize( void )
{
    OPIN_CAN_STBY_OFF();

    CAN1_TransmitEnable();
    CAN1_ReceiveEnable();

#if defined( CAN2_ENABLE )

    if( !CAN2_Initialize( &_can2Config ) )
    {
        API_SetMajorFault( FAULT_CAN2_HW );
    }

    // Init mailboxes
    MCP2515MB_Initialize();

#endif

    API_CanMotor_Initialize();
}

#define MODE_MESSAGE          0x81
#define SERIAL_NUMBER_MESSAGE 0x91
#define SERVICE_LOG_MESSAGE   0xA1
/**
 * Module task loop
 */
void API_Can_Tasks( void )
{
    uCAN_MSG msg;
    uint8_t message_id_from_hmi;

    while( CAN1_receive( &msg ) )
    {
        bool bProcessed = false;

        if( CAN_FRAME_EXT == msg.frame.idType )
        {
            switch( msg.frame.id & 0x00FFFF00 )
            {
                case 0x00FF0000U:
                case 0x00FF0100U:
                case 0x00FF0200U:
                case 0x00FF0300U:
                case 0x00FF0400U:
                case 0x00FF0500U:
                {
                    // Phoenix controller status message

                    uint8_t from = msg.frame.id & 0xFF;
                    uint16_t pgn = ( msg.frame.id >> 8 ) & 0x1ffff;

                    API_CanMotor_RxStatus( from, pgn, &msg );
                    bProcessed = true;

                    break;
                }
                case 0x00FF1000U:   // here is where we filter out messages 81,91....DRW 
                    message_id_from_hmi = msg.frame.id & 0xFF;
                    switch(message_id_from_hmi){
                        case MODE_MESSAGE:
                            handle_mode_message_81h( msg );
                            bProcessed = true;
                            break;
                        case SERIAL_NUMBER_MESSAGE:
                            break;
                        case SERVICE_LOG_MESSAGE:
                            break;
                        default:
                            break;  
                    }
            }
        }

        // If we didn't handle this message, pass it to the API client
        if( !bProcessed )
        {
            APICB_CanRx( &msg );
        }
    }

#if defined( CAN2_ENABLE )

    CAN2_Tasks();
    MCP2515MB_Tasks();

    can_t msg2;

    if( 0 == CAN2_Rx( &msg2 ) )
    {
        APICB_Can2Rx( &msg2 );
    }

#endif

    API_CanMotor_Tasks();
}

/**
 * Temporarily shut off CAN
 */
void API_Can_Sleep( void )
{
    // Put in standby mode
    // We can't put CAN in standby, because the CAN clock is the source of our clock
    // OPIN_CAN_STBY_ON();
}

/**
 * Restore CAN operation after Sleep
 */
void API_Can_Wakeup( void )
{
    CAN1_Initialize();

#if defined( CAN2_ENABLE )
    CAN2_Initialize( &_can2Config );
#endif
}

/**
 * Enable CAN bus 1
 *
 * Initializes the CAN driver and sets up receive filters
 */
void API_EnableCan( void )
{
    // Standby pin to inactive
    OPIN_CAN_STBY_OFF();
}

/** \ingroup canbus
 * Disable CAN bus 1
 *
 * Disabling takes the interface off bus (in standby) and deactivates the TCU_En
 * output.
 */
void API_DisableCan( void )
{
    // Standby pin to active
    // We can't put CAN in standby, because the CAN clock is the source of our clock
    // OPIN_CAN_STBY_ON();
    CAN1_sleep();
}

/**
 * Send message on CAN bus 1
 *
 */
bool API_SendCanMessage( uCAN_MSG *pMsg )
{
    assert( pMsg->frame.dlc <= 8 );

    return CAN1_transmit( CAN_PRIORITY_MEDIUM, pMsg );
}

/**
 * Enable CAN bus 2
 */
void API_EnableCan2( void )
{
#if defined( CAN2_ENABLE )
    if( !CAN2_Initialize( &_can2Config ) )
    {
        API_SetMajorFault( FAULT_CAN2_HW );
    }
#endif
}

/**
 * Disable CAN bus 2
 *
 * Disabling takes the interface off bus (in standby)
 */
void API_DisableCan2( void )
{
#if defined( CAN2_ENABLE )
    CAN2_Sleep();
#endif
}

/**
 * Send message on CAN bus 2
 *
 */
bool API_SendCan2Message( can_t *pMsg )
{
#if defined( CAN2_ENABLE )
    return !CAN2_Tx( pMsg );
#else
    return false;
#endif
}

void API_Can2_UpdateTxMailbox( unsigned index, uint8_t *pData, uint8_t dlc, bool bSendNow )
{
#if defined( CAN2_ENABLE )
    MCP2515MB_UpdateTxMailbox( index, pData, dlc, bSendNow );
#endif
}

void API_Can2_EnableTxMailbox( unsigned index, uint32_t id, uint16_t interval )
{
#if defined( CAN2_ENABLE )
    MCP2515MB_EnableTxMailbox( index, id, interval );
#endif
}

void API_Can2_DisableTxMailbox( unsigned index )
{
#if defined( CAN2_ENABLE )
    MCP2515MB_DisableTxMailbox( index );
#endif
}

/**
 * Debug dump function for this module
 *
 * @private
 */
void API_Can_Dump( void )
{
    TRACE( ( "---API_Can1---\r\n" ) );
    TRACE( ( "RxErr: %u\r\n", _rxErrorPassive ) );
    TRACE( ( "TxErr: %u\r\n", _txErrorPassive ) );
    TRACE( ( "RxOver: %u\r\n", _rxBufferOverflow ) );
    TRACE( ( "RxCount: %u\r\n", _rxMessageCount ) );
    TRACE( ( "isBusOff: %u\r\n", CAN1_isBusOff() ) );
#if defined( CAN2_ENABLE )
    TRACE( ( "---API_Can2---\r\n" ) );
    TRACE( ( "RxQOver: %u\r\n", MCP2515_diag.rxQueueOverflows ) );
    TRACE( ( "TxQOver: %u\r\n", MCP2515_diag.txQueueOverflows ) );
    TRACE( ( "rxBOver: %u\r\n", MCP2515_diag.rxBufferOverflow ) );
    TRACE( ( "isBusOff: %u\r\n", CAN2_isBusOff() ) );

#endif
}
