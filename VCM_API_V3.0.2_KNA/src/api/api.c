/**
 * Implements the main task loop for the hardware API
 */
#include "api.h"
#include "adc.h"
#include "api_can.h"
#include "mcc_generated_files/system.h"
#include "timer.h"
#include <string.h>

//------------------------------//----------------------------------------------
// Definitions
#define CFG_BRK_SET __builtin_write_PWMSFR( &IOCON3, 0x8600, &PWMKEY )
#define CFG_BRK_REL __builtin_write_PWMSFR( &IOCON3, 0x8400, &PWMKEY )
#define CFG_BRK_REL_100 __builtin_write_PWMSFR( &IOCON3, 0x8680, &PWMKEY )

#define CFG_HORN_OFF __builtin_write_PWMSFR( &IOCON2, 0x4500, &PWMKEY )
#define CFG_HORN_ON __builtin_write_PWMSFR( &IOCON2, 0x4400, &PWMKEY )
#define CFG_HORN_ON_100 __builtin_write_PWMSFR( &IOCON2, 0x4540, &PWMKEY )

//------------------------------//----------------------------------------------
// Global variables

// This variable provides a way to provide the effect of the system direction
// input
uint16_t API_direction = API_FORWARD;
volatile API_OpPresence_e  _opPresState = OPERATOR_NO_EVENT;

// System status storage
ApiSystemState_e API_SystemState = API_SYSTEM_ACTIVE;

// keep track of fault status
uint32_t API_majorFaults = 0;
uint32_t API_minorFaults = 0;

//------------------------------//----------------------------------------------
// Local variables
static uint8_t _brkDrvPwmCfgDutyCyclePct = 100;

//------------------------------//----------------------------------------------
// Local functions

/**
 * Finish initialization started in SYSTEM_Initialize()
 *
 */
static void _BrakeInitialize( void )
{
    // override of MCC code
    PHASE3 = BRAKE_PWM_PERIOD;
    // insure no PWM out on Brake Drive (override PWM)
    CFG_BRK_SET;
}

void set_operator_status(API_OpPresence_e status){
    _opPresState = status;
}

API_OpPresence_e get_operator_status(void){
    return _opPresState;
}


/**
 * Finish initialization started in SYSTEM_Initialize()
 *
 */
static void _HornInitialize( void )
{
    PHASE2 = HORN_PWM_PERIOD;
    // insure no PWM out on Horn Out (override PWM)
    CFG_HORN_OFF;
}

//------------------------------//----------------------------------------------
// Public functions

/**
 * Power-up initialization of the API
 *
 */
void API_Initialize( void )
{
    // initialize the device
    SYSTEM_Initialize();

    // SYS_Initialize();
    TIMER_Initialize();
    ADC_Initialize();
    _BrakeInitialize();
    _HornInitialize();
    // API_SPI_Initialize();

    API_Actuator_Initialize();

    API_Dinput_Initialize();

    API_Can_Initialize();
}

/**
 * Maintenance of the API
 *
 */
void API_Tasks( void )
{
    API_Actuator_Tasks();

    API_Dinput_Tasks();

    API_Can_Tasks();
}

/**
 * Put the system in a low power, non-responsive state
 *
 */
void API_Sleep( void )
{
    API_DisableCan();
    API_DisableCan2();
}

/**
 *
 */
void API_Wakeup( void )
{
    API_EnableCan();
    API_EnableCan2();
}

/**
 * Reset the controller processor
 */
void API_Reset( void )
{
    __builtin_disable_interrupts();

    API_DisableCan();
    API_DisableCan2();

    RESET();
}

/**
 * Report a MAJOR Fault
 */
void API_SetMajorFault( uint32_t code )
{
    API_majorFaults |= code;

    TRACE( ( "SET MAJ FAULT: %08lX\r\n", API_majorFaults ) );

    if( API_majorFaults != 0 )
    {
        // force the FAULT output to true
        IO_RA7_SetLow();
        IO_RA7_SetDigitalOutput();
    }
}

/**
 * Clears a MAJOR Fault
 */
void API_ClearMajorFault( uint32_t code )
{
    API_majorFaults &= ~code;

    TRACE( ( "CLR MAJ FAULT: %08lX\r\n", API_majorFaults ) );

    if( API_majorFaults == 0 )
    {
        // release the hardware fault line
        IO_RA7_SetDigitalInput();
    }
}

/**
 * Report a MINOR fault
 */
void API_SetMinorFault( uint32_t code )
{
    API_minorFaults |= code;

    TRACE( ( "SET MIN FAULT: %08lX\r\n", API_minorFaults ) );
}

/**
 * Clears a MINOR Fault
 */
void API_ClearMinorFault( uint32_t code )
{
    API_minorFaults &= ~code;

    TRACE( ( "CLR MIN FAULT: %08lX\r\n", API_minorFaults ) );
}

/**
 * Returns the bitmapped major fault code value
 */
uint32_t API_GetMajorFaults( void )
{
    return API_majorFaults;
}

/**
 * Clears a MINOR Fault
 */
uint32_t API_GetMinorFaults( void )
{
    return API_minorFaults;
}

/**
 * Read Analog Input 0
 *
 * Returns the most recent value of the ANIN0 input.
 *
 * @return value in the range of 0 to API_ADC_MAX
 */
uint16_t API_ReadAin0( void )
{
    return (uint16_t)ADC_ChannelAverage( ADC_CHANNEL_ANIN0 );
}

/**
 * Read Analog Input 1
 *
 * Returns the most recent value of the ANIN0 input.
 *
 * @return value in the range of 0 to API_ADC_MAX
 */
uint16_t API_ReadAin1( void )
{
    return (uint16_t)ADC_ChannelAverage( ADC_CHANNEL_ANIN1 );
}

/** \ingroup analoginputs
 * Read ACT Isense
 *
 * Returns the most recent value of the input.
 *
 * @return value in the range of 0 to API_ADC_MAX
 */
uint16_t API_ReadActIsense( void )
{
    return (uint16_t)ADC_ChannelAverage( ADC_CHANNEL_ACTUATOR_CURRENT );
}

/**
 * Read BattLevel
 *
 * Returns the most recent value of the input.
 *
 * @return value in the range of 0 to API_ADC_MAX
 */
uint16_t API_ReadBattLevel( void )
{
    return (uint16_t)ADC_ChannelAverage( ADC_CHANNEL_BATTLEVEL );
}

/**
 * Read Throttle
 *
 * Returns the most recent value of the input.
 *
 * @return value in the range of 0 to API_ADC_MAX
 */
uint16_t API_ReadThrottle( void )
{
    return (uint16_t)ADC_ChannelAverage( ADC_CHANNEL_THROTTLE );
}

/**
 * Read BRK Sense
 *
 * Returns the most recent value of the input.
 *
 * @return value in the range of 0 to API_ADC_MAX
 */
uint16_t API_ReadBrkSense( void )
{
    return (uint16_t)ADC_ChannelAverage( ADC_CHANNEL_BRK_SENSE );
}

/**
 * Read VControl Level
 *
 * Returns the most recent value of the input.
 *
 * @return value in the range of 0 to API_ADC_MAX
 */
uint16_t API_ReadVControlLevel( void )
{
    return (uint16_t)ADC_ChannelAverage( ADC_CHANNEL_VCONTROL_LEVEL );
}

/**
 * Output API state to debug console
 *
 */
void API_Dump( void )
{
    double voltage;

    // TRACE( ( "API_Dump To be implemented\r\n" ) );
    voltage = ( 3.3 / ADC_VAL_MAX ) * API_ReadAin0();
    printf( "AN%d: ADC_CHANNEL_ANIN0 raw ct: 0x%04x, voltage: %05.3fv\n", ADC_CHANNEL_ANIN0, API_ReadAin0(), voltage );
    voltage = ( 3.3 / ADC_VAL_MAX ) * API_ReadAin1();
    printf( "AN%d: ADC_CHANNEL_ANIN1 raw ct: 0x%04x, voltage: %05.3fv\n", ADC_CHANNEL_ANIN1, API_ReadAin1(), voltage );
    voltage = ( 3.3 / ADC_VAL_MAX ) * API_ReadThrottle();
    printf( "AN%d: ADC_CHANNEL_THROTTLE raw ct: 0x%04x, voltage: %05.3fv\n", ADC_CHANNEL_THROTTLE, API_ReadThrottle(),
            voltage );
    voltage = ( 3.3 / ADC_VAL_MAX ) * API_ReadBattLevel();
    printf( "AN%d: ADC_CHANNEL_BATTLEVEL raw ct: 0x%04x, voltage: %05.3fv\n", ADC_CHANNEL_BATTLEVEL,
            API_ReadBattLevel(), voltage );
    voltage = ( 3.3 / ADC_VAL_MAX ) * API_ReadActIsense();
    printf( "AN%d: ADC_CHANNEL_ACTUATOR_CURRENT raw ct: 0x%04x, voltage: %05.3fv\n", ADC_CHANNEL_ACTUATOR_CURRENT,
            API_ReadActIsense(), voltage );
    voltage = ( 3.3 / ADC_VAL_MAX ) * API_ReadBrkSense();
    printf( "AN%d: ADC_CHANNEL_BRK_SENSE raw ct: 0x%04x, voltage: %05.3fv\n", ADC_CHANNEL_BRK_SENSE, API_ReadBrkSense(),
            voltage );
    voltage = ( 3.3 / ADC_VAL_MAX ) * API_ReadVControlLevel();
    printf( "AN%d: ADC_CHANNEL_VCONTROL_LEVEL raw ct: 0x%04x, voltage: %05.3fv\n", ADC_CHANNEL_VCONTROL_LEVEL, API_ReadVControlLevel(),
            voltage );
}

/**
 * Debug command parsing function
 *
 * This function is called by (for example) a serial terminal
 * implementation when the user enters a DEBUG API command.
 * The pString points to the first character of the argument
 * AFTER the DEBUG API.
 *
 * @param      pToken The string to be parsed
 */
void API_Debug( char *pToken )
{
#if defined( ENABLE_DEBUG )

#endif
}

/**
 * Configures the ACC_OUT1 output on connector J4 to forward
 *
 */
void API_AccOut1Fwd( void )
{
    AUX_OUT1_FWD_IO_RC0_SetLow();
    AUX_OUT1_REV_IO_RG6_SetHigh();
}

/**
 * Configures the ACC_OUT1 output on connector J4 to reverse
 *
 */
void API_AccOut1Rev( void )
{
    AUX_OUT1_FWD_IO_RC0_SetHigh();
    AUX_OUT1_REV_IO_RG6_SetLow();
}

/**
 * Configures the ACC_OUT1 output on connector J4 to standby
 *
 */
void API_AccOut1Standby( void )
{
    AUX_OUT1_FWD_IO_RC0_SetLow();
    AUX_OUT1_REV_IO_RG6_SetLow();
}

/**
 * Configures the ACC_OUT1 output on connector J4 to brake
 *
 */
void API_AccOut1Brake( void )
{
    AUX_OUT1_FWD_IO_RC0_SetHigh();
    AUX_OUT1_REV_IO_RG6_SetHigh();
}

/**
 * Controls the ACC_OUT2 output on connector J4
 *
 * @param[in]  bEnabled   boolean true if the output should be enabled
 */
void API_AccOut2Enable( bool bEnabled )
{
    if( true == bEnabled )
    {
        AUX_OUT2_IO_RC1_SetHigh();
    }
    else
    {
        AUX_OUT2_IO_RC1_SetLow();
    }
}

/**
 * Controls the ACC_OUT3 output on connector J4
 *
 * @param[in]  bEnabled   boolean true if the output should be enabled
 */
void API_AccOut3Enable( bool bEnabled )
{
    if( true == bEnabled )
    {
        AUX_OUT3_IO_RC2_SetHigh();
    }
    else
    {
        AUX_OUT3_IO_RC2_SetLow();
    }
}

/**
 * Controls the ACC_En_Out output on connector J4
 *
 * @param[in]  bEnabled   boolean true if the output should be enabled
 */
void API_AccEnOutEnable( bool bEnabled )
{
    if( true == bEnabled )
    {
        ACC_En_IO_RE14_SetHigh();
    }
    else
    {
        ACC_En_IO_RE14_SetLow();
    }
}

/**
 * Controls the U5_Contactor_Enable_uC output of the micro.
 *
 * @param[in]  bEnabled   boolean true if the output should be enabled
 */
void API_U5_Contactor_Enable_uC( bool bEnabled )
{
    // note could not get mcc to give me access to OSC2 as GPIO
    // I did not want to edit pin_manager to provide the functionality there
    if( true == bEnabled )
    {
        _LATC15 = 1;
    }
    else
    {
        _LATC15 = 0;
    }
}

/**
 *
 * Releases the EM brake (activates brake release output to allow vehicle motion)
 */
void API_ReleaseBrake( void )
{
    // activate pwm at configured percentage to allow brake to release
    if( 100 == _brkDrvPwmCfgDutyCyclePct )
    {
        // if 100% desired, we use override to accomplish (could not quite get to 100% pwm)
        CFG_BRK_REL_100;
    }
    else
    {
        PDC3 = ( uint16_t )( ( PHASE3 / 100 ) * _brkDrvPwmCfgDutyCyclePct );
        // allow PWM out
        CFG_BRK_REL;
    }
}

/**
 *
 * Sets the EM brake (deactivates brake release output to prevent vehicle motion)
 */
void API_SetBrake( void )
{
    // deactivate pwm so the brake is engaged - PWM3H to IOCTL
    CFG_BRK_SET;
}

/**
 *
 * Sets up the behavior of the brake (not supported by hardware)
 *
 * @param[in]  pullPower  Percentage of PWM to apply to the brake when releasing
 */
void API_ConfigureBrake( uint8_t pullPower )
{
    if( pullPower > PWM_BRAKE_MAX_DUTY_CYCLE_PERCENT )
    {
        _brkDrvPwmCfgDutyCyclePct = (uint8_t)PWM_BRAKE_MAX_DUTY_CYCLE_PERCENT;
    }
    else
    {
        // set the pwm percentage for the brake
        _brkDrvPwmCfgDutyCyclePct = pullPower;
    }
}

/**
 * Controls the Horn Drive Output
 *
 * @param[in]  bOn true if horn output should be enabled.
 * @param[in] dutyCycle sets the PWM duty cycle in percent (0..100)
 */
void API_HornEnable( bool bOn, uint8_t dutyCycle )
{
    static uint8_t _hornDrvPwmCfgDutyCyclePct = 50;

    if( dutyCycle > 100 )
    {
        _hornDrvPwmCfgDutyCyclePct = 100;
    }
    else
    {
        _hornDrvPwmCfgDutyCyclePct = dutyCycle;
    }

    // activate pwm at configured percentage - preserve precision by multiplying first
    PDC2 = ( uint16_t )( ( ( uint32_t ) PHASE2 * _hornDrvPwmCfgDutyCyclePct ) / 100 );

    if( true == bOn )
    {
        // at 100% (PHASE2 = PDC2 = 3636) we see ~20nS low going spikes on PWM rollover
        // so for 100% let's override PWM
        if( 100 == _hornDrvPwmCfgDutyCyclePct )
        {
            // no pwm - override == on
            CFG_HORN_ON_100;
        }
        else
        {
            // allow PWM out
            CFG_HORN_ON;
        }
    }
    else
    {
        // shut down pwm
        CFG_HORN_OFF;
    }
}

/**
 * Sets the U5_ENABLE Output
 *
 */
void API_U5EnableSet( void )
{
    U5_ENABLE_N_IO_RF0_SetHigh();
}

/**
 * Clears the U5_ENABLE Output
 *
 */
void API_U5EnableClr( void )
{
    U5_ENABLE_N_IO_RF0_SetLow();
}

/**
 * Sets the OE_n_uC Output
 *
 */
void API_OeNuCSet( void )
{
    OE_n_uC_IO_RG7_SetHigh();
}

/**
 * Clears the OE_n_uC Output
 *
 */
void API_OeNuCClr( void )
{
    OE_n_uC_IO_RG7_SetLow();
}
