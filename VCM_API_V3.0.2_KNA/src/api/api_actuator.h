/**
 * @file api_actuator.h
 * @brief Deck Actuator Controller Module
 *
 * The actuator is a PWM output that drives a brushed motor.
 *
 * Copyright (c) 2018 Control Solutions LLC.  All Rights Reserved.
 *
 */
#ifndef _API_ACTUATOR_H_
#define _API_ACTUATOR_H_

#include "adc.h"
#include "api.h"
#include <stdbool.h>

//------------------------------//----------------------------------------------
// Definitions
typedef struct
{
    uint16_t upAccelerationTenthsPercent;
    uint16_t downAccelerationTenthsPercent;
    uint16_t upDecelerationTenthsPercent;
    uint16_t downDecelerationTenthsPercent;
    uint8_t maxUpSpeedPct;
    uint8_t maxDownSpeedPct;
} ApiActuatorCfg_t;

/**
 * define Actuator Subsystem states
 */
typedef enum {
    API_ACTUATOR_IDLE = 0,
    API_ACTUATOR_RAISING,
    API_ACTUATOR_RAISING_DECEL,
    API_ACTUATOR_LOWERING,
    API_ACTUATOR_LOWERING_DECEL,
} ApiActuatorState_e;

/**
 * define Actuator Subsystem Status
 */
typedef struct
{
    ApiActuatorState_e state;
    uint16_t presentDutyCycleTenthsPercent;
    bool overCurrentOccurred;
    uint16_t largestCurrent;  // as aid in setting ACTUATOR_OVERCURRENT_ADC
} ApiActuatorStatus_t;

/**
 * define Actuator Subsystem constants
 */
#define ACT_PWM_PERIOD               PWM_PERIOD_FOR_22KHZ
#define PWM_MAX_DUTY_CYCLE_PERCENT   95


//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Public functions

/** \defgroup liftactuator Deck Lift Actuator
 * Motor Drive Outputs: J9 ACT+/ACT-
 *
 * The ACT+ and ACT- pins provide a PWM motor drive signal.
 * The Configuration of the lift actuator should be set before requesting
 * a Raise or Lower command (likely set once upon system startup to values
 * that are appropriate to the system).
 *
 * The measured current being supplied to the actuator can be retrieved
 * using the API_ReadActIsense() function.
 *
 */
/** @ingroup    liftactuator
 *
 * Configures the speed and acceleration parameters of the actuator output. This
 * function does not cause movement; it simply configures the drive parameters.
 *
 * Note: the acceleration and deceleration parameters are specified in
 * percentage*10. These values will be added (accel) or subtracted (decel) from
 * the PWM duty cycle percent every 10mS until the maximum speed is reached. For
 * example, an upAcceleration setting of 100 with a maxUpSpeed of 80 will result
 * in the output PWM being incremented by 10.0% every 10ms, until 80% is reached
 * (80 ms)
 *
 * @param[in]  upAcceleration    The up acceleration (percent*10 per 10ms)
 * @param[in]  downAcceleration  The down acceleration (percent*10 per 10ms)
 * @param[in]  upDeceleration    The up deceleration (percent*10 per 10ms)
 * @param[in]  downDeceleration  The down deceleration (percent*10 per 10ms)
 * @param[in]  maxUpSpeed        The maximum up speed (percent)
 * @param[in]  maxDownSpeed      The maximum down speed (percent)
 */
void API_ConfigureDeckLift( uint16_t upAcceleration, uint16_t downAcceleration, uint16_t upDeceleration,
                            uint16_t downDeceleration, uint8_t maxUpSpeed, uint8_t maxDownSpeed );

/** @ingroup    liftactuator
 * Raise the deck using the previously set configuration.
 *
 * Activates the ACT+/- output until the current exceeds a pre-set threshold.
 */
void API_RaiseDeckLift( void );

/** @ingroup    liftactuator
 * Lower the deck using the previously set configuration.
 *
 * Activates the ACT+/- output until the current exceeds a pre-set threshold.
 */
void API_LowerDeckLift( void );

/** @ingroup    liftactuator
 * Immediately stops the deck drive
 *
 * Disables the deck lift output.
 */
void API_StopDeckLift( void );

/** @ingroup    liftactuator
 * Stop deck movement using the previously set configuration.
 *
 * Lower PWM duty cycle until stopped.
 */
void API_DecelAndStopDeckLift( void );

/** @ingroup    liftactuator
 * Return actuator state
 *
 * @return enumerated value of actuator state
 */
ApiActuatorState_e API_getActuatorState( void );

/**
 * Powerup Initialization
 * @private
 */
void API_Actuator_Initialize( void );

/**
 * Module task loop
 * @private
 */
void API_Actuator_Tasks( void );

#endif
