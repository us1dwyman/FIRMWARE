/**
 * Tests the analog inputs by watching for changes
 *
 * Here we watch to see if any of the analog inputs change, and if so
 * we print out a message.  In order to 'pass' we need to see at least one
 * input change state.
 *
 *
 */

#include "adc.h"
#include "api.h"
#include "test.h"

//------------------------------//----------------------------------------------
// Definitions
#define TEST_ADC_FILTER 0xFFF0

//------------------------------//----------------------------------------------
// Global variables

bool TEST_AINPUTS_brkSenseOutputEnabled = false;
bool TEST_AINPUTS_adcOutputEnabled = false;

//------------------------------//----------------------------------------------
// Local variables

// keep our own copy of the last known state
static uint16_t _lastADC_value[ ADC_MAX_CHANNELS ];

// to make sure we only report the test result once
static bool _testComplete = false;

//------------------------------//----------------------------------------------
// Local functions

//------------------------------//----------------------------------------------
// Public functions

/**
 * Initialize the test runner
 */
void ainputs_Initialize( void )
{
    int i;

    // Initialize our copy of the state
    for( i = 0; i < ( sizeof( _lastADC_value ) / sizeof( _lastADC_value[ 0 ] ) ); i++ )
    {
        _lastADC_value[ i ] = ADC_ChannelAverage( i );
    }
}

/**
 * Run the task loop for the test
 */
void TEST_ainputs_Tasks( void )
{
    uint16_t lastFiltered;
    uint16_t presentFiltered;

    if( true == TEST_AINPUTS_adcOutputEnabled )
    {
        lastFiltered = _lastADC_value[ ADC_CHANNEL_ANIN0 ] & TEST_ADC_FILTER;
        presentFiltered = ADC_ChannelAverage( ADC_CHANNEL_ANIN0 ) & TEST_ADC_FILTER;
        if( lastFiltered != presentFiltered )
            TRACE( ( "ADC_ANIN0: %d\r\n", ADC_ChannelAverage( ADC_CHANNEL_ANIN0 ) ) );

        lastFiltered = _lastADC_value[ ADC_CHANNEL_ANIN1 ] & TEST_ADC_FILTER;
        presentFiltered = ADC_ChannelAverage( ADC_CHANNEL_ANIN1 ) & TEST_ADC_FILTER;
        if( lastFiltered != presentFiltered )
            TRACE( ( "ADC_CHANNEL_ANIN1: %d\r\n", ADC_ChannelAverage( ADC_CHANNEL_ANIN1 ) ) );

        lastFiltered = _lastADC_value[ ADC_CHANNEL_THROTTLE ] & TEST_ADC_FILTER;
        presentFiltered = ADC_ChannelAverage( ADC_CHANNEL_THROTTLE ) & TEST_ADC_FILTER;
        if( lastFiltered != presentFiltered )
            TRACE( ( "ADC_CHANNEL_THROTTLE: %d\r\n", ADC_ChannelAverage( ADC_CHANNEL_THROTTLE ) ) );

        lastFiltered = _lastADC_value[ ADC_CHANNEL_BATTLEVEL ] & TEST_ADC_FILTER;
        presentFiltered = ADC_ChannelAverage( ADC_CHANNEL_BATTLEVEL ) & TEST_ADC_FILTER;
        if( lastFiltered != presentFiltered )
            TRACE( ( "ADC_CHANNEL_BATTLEVEL: %d\r\n", ADC_ChannelAverage( ADC_CHANNEL_BATTLEVEL ) ) );

        lastFiltered = _lastADC_value[ ADC_CHANNEL_ACTUATOR_CURRENT ] & TEST_ADC_FILTER;
        presentFiltered = ADC_ChannelAverage( ADC_CHANNEL_ACTUATOR_CURRENT ) & TEST_ADC_FILTER;
        if( lastFiltered != presentFiltered )
            TRACE( ( "ADC_CHANNEL_ACTUATOR_CURRENT: %d\r\n", ADC_ChannelAverage( ADC_CHANNEL_ACTUATOR_CURRENT ) ) );
    }

    if( true == TEST_AINPUTS_brkSenseOutputEnabled )
    {
        lastFiltered = _lastADC_value[ ADC_CHANNEL_BRK_SENSE ] & TEST_ADC_FILTER;
        presentFiltered = ADC_ChannelAverage( ADC_CHANNEL_BRK_SENSE ) & TEST_ADC_FILTER;
        if( lastFiltered != presentFiltered )
            TRACE( ( "ADC_CHANNEL_BRK_SENSE: %d\r\n", ADC_ChannelAverage( ADC_CHANNEL_BRK_SENSE ) ) );
    }

    // update our copy of the most recent state
    ainputs_Initialize();

    if( !_testComplete )
    {
        _testComplete = true;
        TEST_Complete( "ainputs", true );
    }
}