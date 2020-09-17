/**
 * Implementation of Actuator module
 *
 * The actuator is a PWM output that drives a brushed motor.
 * Refer to the header file for usage details.
 *
 * Copyright (c) 2018 Control Solutions LLC.  All Rights Reserved.
 */

#include "api_actuator.h"
#include "adc.h"
#include "api.h"
#include "timer.h"

//------------------------------//----------------------------------------------
// Definitions
//
// in practice: AHO, BHO = 1. (regen/brake mode) ALO, BLO = 0.
#define CFG_IDLE_BRAKE_MODE __builtin_write_PWMSFR( &IOCON1, 0xC300, &PWMKEY )

// in practice: AHO, ALO = PWM. BHO=1, BLO=0.
#define CFG_RAISE_MODE __builtin_write_PWMSFR( &IOCON1, 0xC100, &PWMKEY )

// in practice: BHO, BLO = PWM. AHO=1, ALO=0.
#define CFG_LOWER_MODE __builtin_write_PWMSFR( &IOCON1, 0xC140, &PWMKEY )

//------------------------------//----------------------------------------------
// Global variables
ApiActuatorCfg_t API_ActuatorCfg;
ApiActuatorStatus_t API_ActuatorStatus;

//------------------------------//----------------------------------------------
// Local variables
static MS_TICK _cycleStart;

//------------------------------//----------------------------------------------
// Local functions

/**
 * Return the average reading of the ADC monitoring the total FET current.
 */
static ADC_SAMPLE _ActuatorCurrentRead( void )
{
    return ( ADC_ChannelAverage( ADC_CHANNEL_ACTUATOR_CURRENT ) );
}

/**
 * Set the over current indication.
 */
// static void _SetActuatorOverCurrentOccurred( void )
//{
//    API_ActuatorStatus.overCurrentOccurred = true;
//}

//------------------------------//----------------------------------------------
// Public functions

/**
 * Powerup Initialization
 */
void API_Actuator_Initialize( void )
{
    // complete the initialization started by the MCC PWM_Initialize() function

    // set PWM period
    PHASE1 = ACT_PWM_PERIOD;  // period measured: 45.3 uS (22075 Hz)

    // set duty cycle
    PDC1 = ( uint16_t )( ( PHASE1 / 100 ) * ACT_PWM_START_DUTY_CYCLE_PERCENT );

    // enable pwm module - still no PWM signals out of HIP4081A due to:
    //    1) The HIP4081A DIS pin is still high (coast mode) AHO=ALO=BHO=BLO=0
    // and then once the DIS pin is cleared, the PWM IOCON1 register has override enabled
    // and configured to set both AHO, BHO high  (Brake mode: AHO = BHO = 1)
    PTCON = 0x8000;

    // set ACT_EN low here which will clear the active high DIS
    // input of the HIP4081A - releasing coast mode and setting us to brake mode
    ACT_EN_IO_RE12_SetLow();

    API_ActuatorStatus.state = API_ACTUATOR_IDLE;
    API_ActuatorStatus.presentDutyCycleTenthsPercent = ACT_PWM_START_DUTY_CYCLE_PERCENT * 10;
    API_ActuatorStatus.overCurrentOccurred = false;
    API_ActuatorStatus.largestCurrent = 0;

    _cycleStart = TIMER_Now();
}

/**
 * Module task loop
 */
void API_Actuator_Tasks( void )
{
    uint16_t newDutyCycleTenthsPct;
    ADC_SAMPLE presentCurrent;

    // How long has it been since we started the cycle?
    MS_TICK elapsed = TIMER_Since( _cycleStart );

    // every 10 ms we adjust duty cycle if necessary
    if( elapsed >= 10 )
    {
        presentCurrent = _ActuatorCurrentRead();

        // store the highest current read as aid in tuning
        if( presentCurrent > API_ActuatorStatus.largestCurrent )
        {
            API_ActuatorStatus.largestCurrent = presentCurrent;
        }

        // test against threshold, if over current call the application callback
        if( _ActuatorCurrentRead() > ACTUATOR_OVERCURRENT_ADC )
        {
            if( false == API_ActuatorStatus.overCurrentOccurred )
            {
                // transition from no overcurrent to overcurrent
                APICB_ActuatorOverCurrentOccurred( true );
            }

            // store present state of over current
            API_ActuatorStatus.overCurrentOccurred = true;
        }
        else
        {
            if( true == API_ActuatorStatus.overCurrentOccurred )
            {
                // transition from overcurrent to no overcurrent
                APICB_ActuatorOverCurrentOccurred( false );
            }

            // store present state of over current
            API_ActuatorStatus.overCurrentOccurred = false;
        }

        // stop here if over current until requirements change
        if( true == API_ActuatorStatus.overCurrentOccurred )
        {
            API_StopDeckLift();
        }

        switch( API_ActuatorStatus.state )
        {
            case API_ACTUATOR_RAISING:
                // calculate the new duty cycle
                newDutyCycleTenthsPct =
                    API_ActuatorStatus.presentDutyCycleTenthsPercent + ( API_ActuatorCfg.upAccelerationTenthsPercent );

                // always cap rogue max speed configurations (assert here?)
                if( API_ActuatorCfg.maxUpSpeedPct > PWM_MAX_DUTY_CYCLE_PERCENT )
                {
                    API_ActuatorCfg.maxUpSpeedPct = PWM_MAX_DUTY_CYCLE_PERCENT;
                }

                // Set the new config value if we are still accelerating.
                // Catch where > maxUpSpeedPct config value || > absolute
                // max PWM allowed
                if( newDutyCycleTenthsPct > ( API_ActuatorCfg.maxUpSpeedPct * 10 ) )
                {
                    newDutyCycleTenthsPct = ( API_ActuatorCfg.maxUpSpeedPct * 10 );
                }

                // now convert percent to actual tics
                PDC1 = ( uint16_t )( ( PHASE1 / 100 ) * ( newDutyCycleTenthsPct / 10 ) );
                API_ActuatorStatus.presentDutyCycleTenthsPercent = newDutyCycleTenthsPct;

                break;

            case API_ACTUATOR_RAISING_DECEL:
                // produce new duty cycle - if duty cycle is going to land below MIN
                // then set to MIN - next cycle go to IDLE.

                // if already at or below the minimum duty cycle move to IDLE
                if( API_ActuatorStatus.presentDutyCycleTenthsPercent <= ( ACT_PWM_START_DUTY_CYCLE_PERCENT * 10 ) )
                {
                    API_ActuatorStatus.state = API_ACTUATOR_IDLE;

                    newDutyCycleTenthsPct = ACT_PWM_START_DUTY_CYCLE_PERCENT * 10;
                }
                else if( API_ActuatorStatus.presentDutyCycleTenthsPercent <=
                         ( ( API_ActuatorCfg.upDecelerationTenthsPercent ) +
                           ( ACT_PWM_START_DUTY_CYCLE_PERCENT * 10 ) ) )
                {
                    newDutyCycleTenthsPct = ACT_PWM_START_DUTY_CYCLE_PERCENT * 10;
                }
                else
                {
                    // calculate the new duty cycle
                    newDutyCycleTenthsPct = API_ActuatorStatus.presentDutyCycleTenthsPercent -
                                            ( API_ActuatorCfg.upDecelerationTenthsPercent );
                }

                // now convert percent to actual tics
                PDC1 = ( uint16_t )( ( PHASE1 / 100 ) * ( newDutyCycleTenthsPct / 10 ) );
                API_ActuatorStatus.presentDutyCycleTenthsPercent = newDutyCycleTenthsPct;

                // if we are moving to idle state, shut down PWM
                if( API_ActuatorStatus.state == API_ACTUATOR_IDLE )
                {
                    CFG_IDLE_BRAKE_MODE;
                }

                break;

            case API_ACTUATOR_LOWERING:
                // calculate the new duty cycle
                newDutyCycleTenthsPct = API_ActuatorStatus.presentDutyCycleTenthsPercent +
                                        ( API_ActuatorCfg.downAccelerationTenthsPercent );

                // always cap rogue max speed configurations (assert here?)
                if( API_ActuatorCfg.maxDownSpeedPct > PWM_MAX_DUTY_CYCLE_PERCENT )
                {
                    API_ActuatorCfg.maxDownSpeedPct = PWM_MAX_DUTY_CYCLE_PERCENT;
                }

                // Set the new config value if we are still decelerating.
                // Catch where > maxDownSpeedPct config value || > absolute
                // max PWM allowed
                if( newDutyCycleTenthsPct > ( API_ActuatorCfg.maxDownSpeedPct * 10 ) )
                {
                    newDutyCycleTenthsPct = ( API_ActuatorCfg.maxDownSpeedPct * 10 );
                }

                // now convert percent to actual tics
                PDC1 = ( uint16_t )( ( PHASE1 / 100 ) * ( 100 - ( newDutyCycleTenthsPct / 10 ) ) );
                API_ActuatorStatus.presentDutyCycleTenthsPercent = newDutyCycleTenthsPct;

                break;

            case API_ACTUATOR_LOWERING_DECEL:
                // produce new duty cycle - if duty cycle is going to land below MIN
                // then set to MIN - next cycle go to IDLE.

                // if already at or below the minimum duty cycle move to IDLE
                if( API_ActuatorStatus.presentDutyCycleTenthsPercent <= ( ACT_PWM_START_DUTY_CYCLE_PERCENT * 10 ) )
                {
                    API_ActuatorStatus.state = API_ACTUATOR_IDLE;

                    newDutyCycleTenthsPct = ACT_PWM_START_DUTY_CYCLE_PERCENT * 10;
                }
                else if( API_ActuatorStatus.presentDutyCycleTenthsPercent <=
                         ( ( API_ActuatorCfg.downDecelerationTenthsPercent ) +
                           ( ACT_PWM_START_DUTY_CYCLE_PERCENT * 10 ) ) )
                {
                    newDutyCycleTenthsPct = ACT_PWM_START_DUTY_CYCLE_PERCENT * 10;
                }
                else
                {
                    // calculate the new duty cycle
                    newDutyCycleTenthsPct = API_ActuatorStatus.presentDutyCycleTenthsPercent -
                                            ( API_ActuatorCfg.downDecelerationTenthsPercent );
                }

                // now convert percent to actual tics
                PDC1 = ( uint16_t )( ( PHASE1 / 100 ) * ( 100 - ( newDutyCycleTenthsPct / 10 ) ) );
                API_ActuatorStatus.presentDutyCycleTenthsPercent = newDutyCycleTenthsPct;

                // if we are moving to idle state, shut down PWM
                if( API_ActuatorStatus.state == API_ACTUATOR_IDLE )
                {
                    CFG_IDLE_BRAKE_MODE;
                }

                break;

            case API_ACTUATOR_IDLE:
                CFG_IDLE_BRAKE_MODE;
                PDC1 = ( uint16_t )( ( PHASE1 / 100 ) * ACT_PWM_START_DUTY_CYCLE_PERCENT );

                break;

            default:
                break;
        }
        _cycleStart = TIMER_Now();
    }
}

/**
 * Configures the speed and acceleration parameters of the actuator output.
 *
 */
void API_ConfigureDeckLift( uint16_t upAcceleration, uint16_t downAcceleration, uint16_t upDeceleration,
                            uint16_t downDeceleration, uint8_t maxUpSpeedPct, uint8_t maxDownSpeedPct )
{
    if( ( upAcceleration > 1000 ) || ( downAcceleration > 1000 ) || ( upDeceleration > 1000 ) ||
        ( downDeceleration > 1000 ) || ( maxUpSpeedPct > PWM_MAX_DUTY_CYCLE_PERCENT ) ||
        ( maxDownSpeedPct > PWM_MAX_DUTY_CYCLE_PERCENT ) )
    {
        assert( false );
    }
    else
    {
        API_ActuatorCfg.upAccelerationTenthsPercent = upAcceleration;
        API_ActuatorCfg.downAccelerationTenthsPercent = downAcceleration;
        API_ActuatorCfg.upDecelerationTenthsPercent = upDeceleration;
        API_ActuatorCfg.downDecelerationTenthsPercent = downDeceleration;
        API_ActuatorCfg.maxUpSpeedPct = maxUpSpeedPct;
        API_ActuatorCfg.maxDownSpeedPct = maxDownSpeedPct;
    }
}

/**
 * Raise the deck using the previously set configuration.
 */
void API_RaiseDeckLift( void )
{
    if( API_ActuatorStatus.state == API_ACTUATOR_IDLE )
    {
        // set PWM to practical slow starting value for duty cycle
        PDC1 = ( uint16_t )( ( PHASE1 / 100 ) * ACT_PWM_START_DUTY_CYCLE_PERCENT );

        CFG_RAISE_MODE;

        // will start moving, continue until stop or current limit in future.
        API_ActuatorStatus.state = API_ACTUATOR_RAISING;
        API_ActuatorStatus.presentDutyCycleTenthsPercent = ACT_PWM_START_DUTY_CYCLE_PERCENT * 10;

        _cycleStart = TIMER_Now();
    }
}

/**
 * Lower the deck using the previously set configuration.
 */
void API_LowerDeckLift( void )
{
    if( API_ActuatorStatus.state == API_ACTUATOR_IDLE )
    {
        // set PWM to practical slow starting value for duty cycle

        PDC1 = ( uint16_t )( ( PHASE1 / 100 ) * ( 100 - ACT_PWM_START_DUTY_CYCLE_PERCENT ) );

        CFG_LOWER_MODE;

        // will start moving, continue until stop or over current.
        API_ActuatorStatus.state = API_ACTUATOR_LOWERING;
        API_ActuatorStatus.presentDutyCycleTenthsPercent = ACT_PWM_START_DUTY_CYCLE_PERCENT * 10;

        _cycleStart = TIMER_Now();
    }
}

/**
 * Immediately stops the deck drive
 */
void API_StopDeckLift( void )
{
    if( API_ActuatorStatus.state != API_ACTUATOR_IDLE )
    {
        // stop pwming the FETs - motor in brake/regen
        CFG_IDLE_BRAKE_MODE;

        // set up PWM duty cycle for next activation
        PDC1 = ( uint16_t )( ( PHASE1 / 100 ) * ACT_PWM_START_DUTY_CYCLE_PERCENT );

        API_ActuatorStatus.state = API_ACTUATOR_IDLE;
        API_ActuatorStatus.presentDutyCycleTenthsPercent = ACT_PWM_START_DUTY_CYCLE_PERCENT * 10;
    }
}

/**
 * Stop deck movement using the previously set configuration.
 */
void API_DecelAndStopDeckLift( void )
{
    if( API_ActuatorStatus.state == API_ACTUATOR_LOWERING )
    {
        API_ActuatorStatus.state = API_ACTUATOR_LOWERING_DECEL;
    }
    else if( API_ActuatorStatus.state == API_ACTUATOR_RAISING )
    {
        API_ActuatorStatus.state = API_ACTUATOR_RAISING_DECEL;
    }
}

/**
 * Return actuator state
 */
ApiActuatorState_e API_getActuatorState( void )
{
    return API_ActuatorStatus.state;
}
