/**
 * Tests the Interial Measurement Unit API
 *
 * Initializes and reads the IMU
 *
 *
 */

#include "api.h"
#include "test.h"
#include "timer.h"
#include <stdlib.h>
#include <string.h>

//------------------------------//----------------------------------------------
// Definitions

// if angular rate higher than this threshold is detected, output
// it to debug console if TEST_IMU_gyroOutputOn is set.
// Note per the data sheet a typical gyro drift rate is about 10dps, and
// about 0.05 dps per degree C temperature sensitivity.
// By estimating the (relatively fixed) gyro offset at startup, we should be
// able to reduce that 10dps.
#define GYRO_THRESHOLD 5.0f

// if the acceleration vector magnitude^2 deviates from nominal 1.0 more than
// this amount, output
// it to debug console if TEST_IMU_gyroOutputOn is set.
#define ACCEL_THRESHOLD 0.2f

// if the temperature  is higher than
// this amount, output
// it to debug console if TEST_IMU_gyroOutputOn is set.
#define HIGH_TEMP_THRESHOLD 77.0f

//------------------------------//----------------------------------------------
// Global variables

// whether we should monitor and report gyro values
bool TEST_IMU_gyroOutputOn = false;

//------------------------------//----------------------------------------------
// Local variables

// keep track of the bias (offset) of the gyro
static float _gXoffset, _gYoffset, _gZoffset;

//------------------------------//----------------------------------------------
// Local functions

void _SendSample( ImuRawSample_t *pSample )
{

    // Send the sample out on the CANBUS
    uCAN_MSG msg;

    // calculate degrees per second / 2 (recipient has to multiply
    // by 2 to get actual degrees per second)
    int8_t gX = ( int8_t )( ( pSample->gX - _gXoffset + 0.5 ) / 2 );
    int8_t gY = ( int8_t )( ( pSample->gY - _gYoffset + 0.5 ) / 2 );
    int8_t gZ = ( int8_t )( ( pSample->gZ - _gZoffset + 0.5 ) / 2 );

    // calculate acceleration g's * 50 (recipient has to divide by
    // 50 to get g's)
    int8_t aX = ( int8_t )( ( pSample->aX + 0.5 ) * 50 );
    int8_t aY = ( int8_t )( ( pSample->aY + 0.5 ) * 50 );
    int8_t aZ = ( int8_t )( ( pSample->aZ + 0.5 ) * 50 );

    msg.frame.id = ( CAN_PGN_VCM_IMU << 8 | CANADDR_VCM );
    msg.frame.idType = CAN_FRAME_EXT;
    msg.frame.dlc = 7;
    msg.frame.data0 = ( gX );
    msg.frame.data1 = ( gY );
    msg.frame.data2 = ( gZ );
    msg.frame.data3 = ( aX );
    msg.frame.data4 = ( aY );
    msg.frame.data5 = ( aZ );

    API_SendCanMessage( &msg );
}

//------------------------------//----------------------------------------------
// Public functions

/**
 * Initialize the test runner
 */
void IMU_Initialize( void )
{
    ImuRawSample_t sample;

    // Initialize the device and start taking measurements
    API_StartImu();

    if( API_ReadImu( &sample ) )
    {
        bool isValid = true;

        // magnitude(squared) of 3D acceleration vector should be close to 1g
        // (pointing downward). This assumes the platform is stationary
        // (or at least not accelerating in any direction).

        float mag2 = sample.aX * sample.aX + sample.aY * sample.aY + sample.aZ * sample.aZ;

        isValid &= ( mag2 > 0.8 && mag2 < 1.2 );

        // the gyro will have bias (drift) but it should not be exactly zero or large
        isValid &= ( sample.gX > -5 && sample.gX < 5 && sample.gX != 0 );
        isValid &= ( sample.gY > -5 && sample.gY < 5 && sample.gY != 0 );
        isValid &= ( sample.gZ > -5 && sample.gZ < 5 && sample.gZ != 0 );

        // Temperature should be in a reasonable range
        isValid &= ( sample.temp > 1 && sample.temp < 100.0 );

        if( !isValid )
        {
            printf( "IMU: %f %f %f %f %f %f %f\r\n", (double)sample.aX, (double)sample.aY, (double)sample.aZ,
                    (double)sample.gX, (double)sample.gY, (double)sample.gZ, (double)sample.temp );
        }
        TEST_Complete( "IMU", isValid );

        // keep track of the initial value - assuming it represents the (relatively constant) gyro bias.
        // this could be improved by averaging a few samples, or implementing high pass filtering
        // to remove the DC offset.
        _gXoffset = sample.gX;
        _gYoffset = sample.gY;
        _gZoffset = sample.gZ;

        API_StopImu();

        return;
    }

    TEST_Complete( "IMU", false );

    API_StopImu();
}

/**
 * Run the task loop for the test
 *
 * For this to do anything useful, API_StartImu() has to be called (once) and
 * TEST_IMU_gyroOutputOn has to be set.
 */
void TEST_IMU_Tasks( void )
{
    static MS_TICK timer = 0;

    if( TEST_IMU_gyroOutputOn && TIMER_Since( timer ) >= 100 )
    {
        ImuRawSample_t sample;

        if( API_ReadImu( &sample ) )
        {
            _SendSample( &sample );

            // round gyro reading to nearest dps
            int gX = (int)( sample.gX - _gXoffset + 0.5 );
            int gY = (int)( sample.gY - _gYoffset + 0.5 );
            int gZ = (int)( sample.gZ - _gZoffset + 0.5 );

            if( abs( gX ) > GYRO_THRESHOLD || abs( gY ) > GYRO_THRESHOLD || abs( gZ ) > GYRO_THRESHOLD )
            {
                // gyro reporting a turn
                printf( "GYRO: %3.0f, %3.0f, %3.0f\r\n", (double)gX, (double)gY, (double)gZ );
            }

            float mag2 = sample.aX * sample.aX + sample.aY * sample.aY + sample.aZ * sample.aZ;

            if( mag2 < ( 1.0 - ACCEL_THRESHOLD ) || mag2 > ( 1.0 + ACCEL_THRESHOLD ) )
            {
                printf( "ACCEL: %3.1f %3.1f, %3.1f, %3.1f\r\n", (double)mag2, (double)sample.aX, (double)sample.aY,
                        (double)sample.aZ );
            }

            if( sample.temp > HIGH_TEMP_THRESHOLD )
            {
                printf( "TEMP: %f\r\n", (double)sample.temp );
            }
        }
    }
}
