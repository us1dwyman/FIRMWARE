/**
 * Driver for LSM6DS3 Inertial Measurement Unit
 *
 * Provides functions for interfacing to the device
 *
 * Copyright (c) 2018 Control Solutions LLC.  All Rights Reserved.
 */

#include "lsm6ds3.h"
#include "app.h"
#include "hardware.h"
#include "mcc_generated_files/i2c1.h"
#include "timer.h"
#include <string.h>

//------------------------------//----------------------------------------------
// Definitions

// read a single register is really just a region read of 1 byte
#define _ReadRegister( p, o ) _ReadRegisterRegion( p, o, 1 )

//------------------------------//----------------------------------------------
// Global variables

unsigned int LSM6DS3_failedReadCounter = 0;

//------------------------------//----------------------------------------------
// Local variables

//------------------------------//----------------------------------------------
// Local functions

/**
 * @brief      Reads one or more registers from the device
 *
 * Reads the requested bytes, and returns IMU_SUCCESS if no errors occur.
 * Implements a gross timeout to avoid getting stuck if the hardware fails to
 * complete the transaction.
 * Note with the current I2C driver there does not seem to be a way to cancel
 * a transaction, and we are giving it pointers to variables on the stack.
 * If we time out, and the driver later accesses those variables, we can
 * crash the system.
 *
 * @param      outputPointer  pointer to buffer to receive bytes
 * @param[in]  offset         register address at which to start reading
 * @param[in]  length         number of bytes to read
 *
 * @return     error code
 */
static LSM6DS3_Status_e _ReadRegisterRegion( uint8_t *outputPointer, uint8_t offset, uint8_t length )
{
    volatile I2C1_MESSAGE_STATUS status = I2C1_MESSAGE_PENDING;
    I2C1_TRANSACTION_REQUEST_BLOCK readTRB[ 2 ];

    // write address to chip
    I2C1_MasterWriteTRBBuild( &readTRB[ 0 ], &offset, 1, IMU_I2C_ADDR );

    // request a read of that register
    I2C1_MasterReadTRBBuild( &readTRB[ 1 ], outputPointer, length, IMU_I2C_ADDR );

    // now send the transactions
    I2C1_MasterTRBInsert( 2, readTRB, (I2C1_MESSAGE_STATUS *) &status );

    MS_TICK timer = TIMER_Now();

    // wait for the message to be sent or status has changed. the timeout
    // is kinda arbitrary and really depends on the number of bytes read
    // 1 millisecond per byte seems generous
    while( ( status == I2C1_MESSAGE_PENDING ) && ( TIMER_Since( timer ) < length + 1 ) )
    {
    }

    // successful if status is complete
    return ( status == I2C1_MESSAGE_COMPLETE ) ? IMU_SUCCESS : IMU_HW_ERROR;
}

/**
 * @brief      Writes registers
 *
 * Writes to the device, and returns an error code indicating the success of
 * the transaction.  Implements a gross timeout in case the hardware does
 * not complete the transaction for some reason.
 * Note with the current I2C driver there does not seem to be a way to cancel
 * a transaction, and we are giving it pointers to variables on the stack.
 * If we time out, and the driver later accesses those variables, we can
 * crash the system.
 *
 * @param[in]  offset       Register address to write
 * @param[in]  dataToWrite  The data to write
 *
 * @return     error code
 */
static LSM6DS3_Status_e _WriteRegisters( uint8_t *pData, uint8_t length )
{

    volatile I2C1_MESSAGE_STATUS status = I2C1_MESSAGE_PENDING;

    // start write transaction
    I2C1_MasterWrite( pData, length, IMU_I2C_ADDR, (I2C1_MESSAGE_STATUS *) &status );

    MS_TICK timer = TIMER_Now();

    // wait for the message to be sent or status has changed. the timeout
    // is kinda arbitrary and really depends on the number of bytes written
    // 1 millisecond per byte seems generous
    while( ( status == I2C1_MESSAGE_PENDING ) && ( TIMER_Since( timer ) < length + 1 ) )
    {
    }

    return ( status == I2C1_MESSAGE_COMPLETE ) ? IMU_SUCCESS : IMU_HW_ERROR;
}

//------------------------------//----------------------------------------------
// Public functions

/**
 * @brief      Initializes the sensor hardware
 *
 * @return     true on success
 */
bool LSM6DS3_Initialize( void )
{

#define NUM_CONFIG_BYTES 11
    uint8_t buf[ NUM_CONFIG_BYTES ] = {
        // Address to start writing
        LSM6DS3_ACC_GYRO_CTRL1_XL,

        // CTRL1_XL
        LSM6DS3_ACC_GYRO_FS_XL_2g | LSM6DS3_ACC_GYRO_BW_XL_100Hz | LSM6DS3_ACC_GYRO_ODR_XL_26Hz,

        // CTRL2_G
        LSM6DS3_ACC_GYRO_FS_G_245dps | LSM6DS3_ACC_GYRO_ODR_G_26Hz,

        // CTRL3_C
        LSM6DS3_ACC_GYRO_BLE_LSB | LSM6DS3_ACC_GYRO_IF_INC_ENABLED,

        // CTRL4_C
        0b00000000,

        // CTRL5_C
        0b00000000,

        // CTRL6_G
        0b00000000,

        // CTRL7_G
        LSM6DS3_ACC_GYRO_HP_EN_ENABLED,

        // CTRL8_XL
        0b00000000,

        // CTRL9_XL
        0b00111000,

        // CTRL10_C
        0b00111000,

    };

    // Write the configuration registers
    if( IMU_SUCCESS != _WriteRegisters( buf, NUM_CONFIG_BYTES ) )
    {
        return false;
    }

#ifdef DEBUG
    // read back to verify
    uint8_t checkbuf[ NUM_CONFIG_BYTES ];
    if( IMU_SUCCESS != _ReadRegisterRegion( checkbuf, LSM6DS3_ACC_GYRO_CTRL1_XL, NUM_CONFIG_BYTES ) )
    {
        return false;
    }

    // are the registers set correctly?
    if( memcmp( checkbuf, &buf[ 1 ], NUM_CONFIG_BYTES - 1 ) )
    {
        return false;
    }
#endif

    uint8_t readCheck;
    _ReadRegister( &readCheck, LSM6DS3_ACC_GYRO_WHO_AM_I_REG );
    if( readCheck != 0x69 )
    {
        return false;
    }

    return true;
}

/**
 * @brief      Turns off the sensors
 *
 */
void LSM6DS3_Sleep( void )
{
    uint8_t buf[ 3 ] = {// address to start writing
                        LSM6DS3_ACC_GYRO_CTRL1_XL,

                        // CTRL1_XL (off)
                        0,

                        // CTRL2_G (off)
                        0};

    _WriteRegisters( buf, 3 );
}

/**
 * @brief      Read the fixed-point value of all 7 sensors
 *
 * @param      pData  pointer to receive the data (7*2=14 bytes)
 *
 * @return     status enumeration
 */
LSM6DS3_Status_e LSM6DS3_ReadRawSensors( int16_t *pData )
{
    LSM6DS3_Status_e errorLevel = _ReadRegisterRegion( (uint8_t *)pData, LSM6DS3_ACC_GYRO_OUT_TEMP_L, 14 );
    if( errorLevel != IMU_SUCCESS )
    {
        LSM6DS3_failedReadCounter++;
    }
    return errorLevel;
}
