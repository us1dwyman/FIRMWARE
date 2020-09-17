/**
 * One line module summary
 *
 *  Detailed module description
 *
 * Copyright (c) 2018 Control Solutions LLC.  All Rights Reserved.
 */

#include "LSM6DS3.h"
#include "app.h"
#include "hardware.h"
#include "mcc_generated_files/i2c1.h"
#include "timer.h"

//------------------------------//----------------------------------------------
// Definitions

// Max deg/s.  Can be: 125, 245, 500, 1000, 2000
// This needs to match the hardcoded config value in lsm6ds3.c
#define IMU_GYRO_RANGE 245

// Max G force readable.  Can be: 2, 4, 8, 16
// This needs to match the hardcoded config value in lsm6ds3.c
#define IMU_ACCEL_RANGE 2

#if( IMU_GYRO_RANGE == 245 )
#define GYRO_RANGE_DIVISOR 2
#else
#define GYRO_RANGE_DIVISOR ( IMU_GYRO_RANGE / 125 )
#endif

//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Local variables

//------------------------------//----------------------------------------------
// Local functions

static float _CalcGyro( int16_t input )
{
    float output = (float)input * 4.375 * ( GYRO_RANGE_DIVISOR ) / 1000;
    return output;
}

static float _CalcAccel( int16_t input )
{
    float output = (float)input * 0.061 * ( IMU_ACCEL_RANGE >> 1 ) / 1000;
    return output;
}

static float _CalcTempF( int16_t input )
{
    float output = (float)input / 16;  // divide by 16 to scale
    output += 25;                      // Add 25 degrees to remove offset
    output = ( output * 9 ) / 5 + 32;  // to farenheit

    return output;
}

//------------------------------//----------------------------------------------
// Public functions

/**
 * Powerup Initialization
 */

/**
 * Enable the IMU and start taking measurements.
 *
 *
 */
void API_StartImu( void )
{
    if( !LSM6DS3_Initialize() )
    {
        API_SetMinorFault( FAULT_IMU_HW );
    }

    return;
}

/**
 * Disable the IMU and stop taking measurements.
 *
 *
 */
void API_StopImu( void )
{
    LSM6DS3_Sleep();
}

/**
 * @brief      Reads the accelerometer, gyro, and temperature
 *
 * @param      p     pointer to structure to receive the sample
 * @return boolean true if success
 */
bool API_ReadImu( ImuRawSample_t *p )
{
    int16_t rawData[ 7 ];

    if( IMU_SUCCESS == LSM6DS3_ReadRawSensors( rawData ) )
    {
        p->temp = _CalcTempF( ( rawData[ 0 ] ) );
        p->gX = _CalcGyro( ( rawData[ 1 ] ) );
        p->gY = _CalcGyro( ( rawData[ 2 ] ) );
        p->gZ = _CalcGyro( ( rawData[ 3 ] ) );
        p->aX = _CalcAccel( ( rawData[ 4 ] ) );
        p->aY = _CalcAccel( ( rawData[ 5 ] ) );
        p->aZ = _CalcAccel( ( rawData[ 6 ] ) );

        return true;
    }

    // fail...
    return false;
}

/**
 * @brief      Debug dump
 *
 * @private
 */
void API_DumpImu( void )
{
    ImuRawSample_t sample;

    TRACE( ( "IMU_ERR: %u\r\n", LSM6DS3_failedReadCounter ) );
    if( API_ReadImu( &sample ) )
    {
        TRACE( ( "IMU: ax:%f aY:%f aZ:%f gX:%f gY:%f gZ:%f T:%f\r\n", (double)sample.aX, (double)sample.aY, (double)sample.aZ,
                 (double)sample.gX, (double)sample.gY, (double)sample.gZ, (double)sample.temp ) );
    }
    else
    {
        TRACE( ( "Failed to read IMU\r\n" ) );
    }
}
