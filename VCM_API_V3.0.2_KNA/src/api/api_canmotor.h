/**
 * @file api_canmotor.h
 * @brief CANBUS Motor Controller Interface
 *
 * @copyright Copyright (c) 2018 Control Solutions LLC.  All Rights Reserved.
 *
 */

#ifndef _API_CANMOTOR_H_
#define _API_CANMOTOR_H_

#include "app.h"
#include "mcc_generated_files/can_types.h"

//------------------------------//----------------------------------------------
// Definitions

/**
 * stores a copy of an external CANBUS motor status
 */
typedef struct
{
    uint8_t tbd[ 8 ];
} ApiCanMotorStatus_t;

typedef enum
{
    API_FORWARD = 0,
    API_REVERSE,
} ApiCanMotorDirection_e;

typedef struct
{
    uint8_t busAddress;

    bool bIsBidirectional;

} CanMotorConfig_t;

//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Public functions
//
/** \defgroup    canmotor Phoenix Motor Interface via CAN
 * Interface to Phoenix motor controller(s) via CAN
 *
 * Zero or more Phoenix motor controllers may be connected to the CAN
 * bus to perform drive or chore functions.  Each of these controllers
 * has a unique ID.  In order to use a motor, you must know its ID (assigning
 * IDs to the motor controllers is part of system design).
 * Within the VCM, each motor is referenced by an index (handle).
 * First, enable the motor.  This will start CAN communication with that motor.  When
 * enabling the motor, the bus ID is associated with that index.  For future calls,
 * you use the index to refer to a specific motor.
 * Shortly afterward (say 500ms) you can start checking the MotorStatus.
 * Control the motor by updating its parameters (throttle, direction, etc).
 * If the motor is not needed, you can stop communicating with it via CAN
 * using the CanMotorDisable() function.
 */

/**
 * Defines the maximum number of CAN-connected motors that can be used
 */
#define API_MAX_MOTORS 6

/** \ingroup canmotor
 * Enable communication with a specific motor
 *
 * @param[in] index the index [0..API_MAX_MOTORS-1] of the motor.
 * @param[in] pConfig pointer to config of the motor to be enabled.
 */
void API_CanMotorEnable( uint8_t index, const CanMotorConfig_t *pConfig );

/** \ingroup canmotor
 * Retrieve last known status of the specified motor
 *
 * @param[in] index the index [0..API_MAX_MOTORS-1] of the motor.
 * @param[out] pStatus pointer to status structure to be filled.
 * @return boolean true if status is available.
 */
bool API_CanMotorStatus( uint8_t index, ApiCanMotorStatus_t *pStatus );

/** \ingroup canmotor
 * Retrieve last known pwm value of the specified motor
 *
 * @param[in] index the index [0..API_MAX_MOTORS-1] of the motor.
 * @param[out] pSwm pointer to pwm structure to be filled.
 * @return boolean true if pwm is available.
 */
bool API_CanMotorPwm( uint8_t index, uint8_t *pPwm );

/** \ingroup canmotor
 * Sets the throttle of the specified motor
 *
 * The throttle command is a value between zero and 0xFFF.
 * These values correspond to zero and full speed respectively.
 *
 * @param[in] index the index [0..API_MAX_MOTORS-1] of the motor.
 * @param[in] throttle command [0..0xFFF] requested
 */
void API_CanMotorSetThrottle( uint8_t index, uint16_t throttle );

/** \ingroup canmotor
 * Sets the direction (forward/reverse) of the specified motor
 *
 * @param[in] index the index [0..API_MAX_MOTORS-1] of the motor.
 * @param[in] direction the requested direction
 */
void API_CanMotorSetDirection( uint8_t index, ApiCanMotorDirection_e direction );

/** \ingroup canmotor
 * Disable communication with a specific motor
 *
 * @param[in] index the index [0..API_MAX_MOTORS-1] of the motor.
 */
void API_CanMotorDisable( uint8_t index );

/**
 * @brief      Powerup initialization
 *
 * Call once at system startup
 * @private
 */
void API_CanMotor_Initialize( void );

/**
 * @brief      Module task loop
 *
 * Call periodically to execute the module task loop
 * @private
 */
void API_CanMotor_Tasks( void );

/**
 * @brief      Interface to receive CAN status messages from controllers
 *
 * This function is used internally by the API, to process incoming CAN messages
 * that come from the controllers on the CANBUS.
 *
 * @param[in]  from  Address of motor that sent the message
 * @param[in]  pgn   Message identifier
 * @param[in]  pMsg  The message that was received
 *
 * @return     true if message was from a recognized controller and was processed
 */
bool API_CanMotor_RxStatus( uint8_t from, uint16_t pgn, uCAN_MSG *pMsg );

uint8_t API_CanMotor_Return_Status ( uint8_t from, uint16_t pgn, uint8_t data );
#endif
