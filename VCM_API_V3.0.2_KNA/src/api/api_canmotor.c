/**
 * Implementation of module used to communicate with Phoenix controllers via CANBUS
 *
 *
 * Copyright (c) 2018 Control Solutions LLC.  All Rights Reserved.
 */

#include "api_canmotor.h"
#include "adc.h"
#include "api.h"
#include "mcc_generated_files/can1.h"
#include <string.h>

//------------------------------//----------------------------------------------
// Definitions
//

// Bit masks for Phoenix digital control
#define DCONTROL_FWD_MASK 0b11011111;
#define DCONTROL_QSTOP_MASK 0b11101111;

// Structure for keeping track of a specific motor's state
typedef struct
{
    // Whether this table entry is enabled.  If not, the rest of the data elements don't mean anything.
    bool bEnabled;

    // CAN device ID of the motor
    uint8_t address;

    // Keep track of when we received the last status message from the motor controller
    MS_TICK lastStatusReceived;

    // Keep a copy of the throttle message we send periodically
    uCAN_MSG throttleMessage;

    // Keep Phoenix Status Messages
    uCAN_MSG lastStatus[ 5 ];

    // Which way the motor is supposed to turn when throttle applied
    ApiCanMotorDirection_e direction;

    // Whether the motor is capable of moving in both directions
    bool bIsBidirectional;

} CanMotor_t;

//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Local variables

static CanMotor_t _motors[ API_MAX_MOTORS ];

// keep track of when we last sent throttle messages
static MS_TICK _throttleTick;

//------------------------------//----------------------------------------------
// Local functions

//------------------------------//----------------------------------------------
// Public functions

bool API_CanMotor_RxStatus( uint8_t from, uint16_t pgn, uCAN_MSG *pMsg )
{
    int i;

    // so far we only need these.  If you change these limits check the
    // size of the _motors[i].lastStatus array]
    if( pgn < 0xFF00U || pgn > 0xFF03 )
    {
        return false;
    }

    // look up the motor that sent us the message
    for( i = 0; i < API_MAX_MOTORS; i++ )
    {
        if( _motors[ i ].bEnabled && _motors[ i ].address == from )
        {
            // remember when we received status from this controller
            _motors[ i ].lastStatusReceived = TIMER_Now();

            memcpy( &_motors[ i ].lastStatus[ pgn & 0x03 ], pMsg, sizeof( uCAN_MSG ) );

            // early return, tell caller we processed the message
            return true;
        }
    }

    // the message was not from a motor we care about
    return false;
}

/**
 * Enable communication with a specific motor
 *
 */
void API_CanMotorEnable( uint8_t index, const CanMotorConfig_t *pConfig )
{
    assert( index < API_MAX_MOTORS );

    if( index < API_MAX_MOTORS )
    {
        _motors[ index ].bEnabled = true;
        _motors[ index ].address = pConfig->busAddress;
        _motors[ index ].direction = API_FORWARD;
        _motors[ index ].bIsBidirectional = pConfig->bIsBidirectional;

        TRACE( ( "CanMotor %d enabled\r\n", pConfig->busAddress ) );

        // Set up the CAN packet that will be sent periodically
        uint16_t addresses = ( ( (uint16_t)pConfig->busAddress ) << 8 ) + CANADDR_VCM;
        _motors[ index ].throttleMessage.frame.id = 0x10EF0000 + addresses;
        _motors[ index ].throttleMessage.frame.idType = CAN_FRAME_EXT;
        _motors[ index ].throttleMessage.frame.dlc = 4;
        _motors[ index ].throttleMessage.frame.data0 = 0x50;
        _motors[ index ].throttleMessage.frame.data1 = 0x00;
        _motors[ index ].throttleMessage.frame.data2 = 0xFF;
        _motors[ index ].throttleMessage.frame.data3 = 0xFF;

        API_CanMotorSetThrottle( index, 0 );

        _motors[ index ].lastStatusReceived = TIMER_Now();
    }
}

/**
 * Retrieve last known status of the specified motor
 *
 */
bool API_CanMotorStatus( uint8_t index, ApiCanMotorStatus_t *pStatus )
{
    return false;
}

/**
 * Retrieve last known pwm value of the specified motor
 *
 */
bool API_CanMotorPwm( uint8_t index, uint8_t *pPwm )
{
    assert( index < API_MAX_MOTORS );

    if( index < API_MAX_MOTORS )
    {
        // we know that pwm is the data2 field of the STATUS0 msg
        *pPwm = _motors[ index ].lastStatus[ CAN_PGN_PHOENIX_STATUS0 & 0x03 ].frame.data2;

        return true;
    }
    return false;
}

/**
 * Sets the throttle of the specified motor
 *
 * @param[in] index the index [0..API_MAX_MOTORS-1] of the motor.
 * @param[in] throttle command requested
 */
void API_CanMotorSetThrottle( uint8_t index, uint16_t throttle )
{
    assert( index < API_MAX_MOTORS );

    if( index < API_MAX_MOTORS )
    {
        uint16_t scaledThrottle = throttle >> 4;

        _motors[ index ].throttleMessage.frame.data1 = ( scaledThrottle & 0xFF );
    }
}

/**
 * Sets the direction (forward/reverse) of the specified motor
 *
 * @param[in] index the index [0..API_MAX_MOTORS-1] of the motor.
 * @param[in] direction the requested direction
 */
void API_CanMotorSetDirection( uint8_t index, ApiCanMotorDirection_e direction )
{
    assert( index < API_MAX_MOTORS );

    if( index < API_MAX_MOTORS )
    {
        uint8_t byte = _motors[ index ].throttleMessage.frame.data2;
        if( API_FORWARD == direction )
        {
            byte |= ~DCONTROL_FWD_MASK;
        }
        else
        {
            byte &= DCONTROL_FWD_MASK;
        }
        _motors[ index ].throttleMessage.frame.data2 = byte;
        _motors[ index ].direction = direction;
    }
}

/**
 * Disable communication with a specific motor
 *
 */
void API_CanMotorDisable( uint8_t index )
{
    if( index < API_MAX_MOTORS )
    {
        _motors[ index ].bEnabled = false;
    }
}
/**
 * @brief      Task loop
 */
void API_CanMotor_Tasks( void )
{
    if( TIMER_Since( _throttleTick ) >= 50 )
    {
        unsigned i;

        for( i = 0; i < API_MAX_MOTORS; i++ )
        {

            if( _motors[ i ].bEnabled )
            {
                CAN1_transmit( CAN_PRIORITY_MEDIUM, &_motors[ i ].throttleMessage );

                if( TIMER_Since( _motors[ i ].lastStatusReceived ) > 1000 )
                {
                    // Are not (or stopped) receiving messages from this motor controller.  This is
                    // an error
                    //
                }
            }
        }

        _throttleTick = TIMER_Now();
    }
}

/**
 * Powerup initialization
 *
 */
void API_CanMotor_Initialize( void )
{
    unsigned i;

    for( i = 0; i < API_MAX_MOTORS; i++ )
    {

        _motors[ i ].bEnabled = false;
    }

    _throttleTick = TIMER_Now();
}

// from is the motor index 0,1,2,3
// pgn is the status index 0,1,2,3  code is limiting recording only 4 status messages
// data is frame data
uint8_t API_CanMotor_Return_Status ( uint8_t from, uint16_t pgn, uint8_t data )
{
    uint8_t *data_ptr;
    uint8_t result;
    data_ptr = (uint8_t *) &_motors[ from ].lastStatus[ pgn ].frame.data0;
    data_ptr += data;
    result = *data_ptr;
    return result;
}