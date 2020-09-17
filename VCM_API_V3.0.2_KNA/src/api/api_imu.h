/**
 * @file api_imu.h
 * @brief Defines the interface to the Inertial Measurement Unit
 *
 * @copyright Copyright (c) 2018 Control Solutions LLC.  All Rights Reserved.
 *
 */

#ifndef _API_IMU_H_
#define _API_IMU_H_

#include "api.h"
#include <stdint.h>

/** \defgroup    imu Inertial Measurement Unit
 * Provides an interface to the onboard IMU
 *
 * The IMU monitors the motion of the controller by combining the
 * output of an accelerometer and a gyroscope.  It outputs an estimate
 * of the pitch, roll, and yaw of the vehicle.  This position is relative to the
 * position of the vehicle at startup, as there is no absolute reference to
 * orient the device to, for example, north.
 * Also, which dimension (X,Y,Z) corresponds to pitch, roll, and yaw will
 * be dependent on the orientation of the controller mounted in the vehicle.
 *
 */

/** \ingroup imu
 * Structure to contain a single sensor sample.
 */
typedef struct
{
    /**
     * Accelerometer X-axis reading. This is a floating point number in g's
     */
    float aX;

    /**
     * Accelerometer Y-axis reading. This is a floating point number in g's
     */
    float aY;

    /**
     * Accelerometer Z-axis reading. This is a floating point number in g's
     */
    float aZ;

    /**
     * Gyroscope X-axis reading. This is a floating point number in degrees per second
     */
    float gX;

    /**
     * Gyroscope Y-axis reading. This is a floating point number in degrees per second
     */
    float gY;

    /**
     * Gyroscope Y-axis reading. This is a floating point number in degrees per second
     */
    float gZ;

    /**
     * IMU Temperature reading in degrees F
     */
    float temp;

} ImuRawSample_t;

/** \ingroup imu
 * @brief      Reads the accelerometer, gyro, and temperature
 *
 * @param      p     pointer to structure to receive the sample
 * @return boolean true if success
 */
bool API_ReadImu( ImuRawSample_t *p );

/** \ingroup imu
 * Enable the IMU and start taking measurements.
 *
 *
 */
void API_StartImu( void );

/** \ingroup imu
 * Disable the IMU and stop taking measurements.
 *
 *
 */
void API_StopImu( void );

/**
 * @brief      Debug dump
 *
 * @private
 */
void API_DumpImu( void );

#endif
