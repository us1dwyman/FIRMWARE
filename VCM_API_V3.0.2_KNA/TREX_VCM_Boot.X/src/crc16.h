/**
 * CRC-16 functions
 *
 * Provides cyclic-redundancy check calculations.
 * This module builds a lookup table in RAM in order to speed up processing of large data blocks.
 *
 * Default characteristics of the CRC:
 * 		- Polynomial: 0XA001 (default)
 * 		- Direction: MSB first
 * 		- Reflected: No
 *
 * Use the _Initialize() function before computing any CRCs.
 *
 * To compute a CRC over a block of data,
 * 		Initialize the CRC value (this can be zero or non-zero, but the same initial value needs to be
 * 		used when generating and verifying the CRC)
 *
 * 		Call the CRC16_Update function once per data byte.  The return value is the CRC.
 *
 * To validate a CRC,
 * 		Compute the CRC over the same block of data with the same initial value and polynomial.
 * 		The resulting CRC should be the same.  Alternately, calculate the CRC over the block of data
 * 		with the CRC appended, and the resulting CRC will be zero if there are no errors detected.
 *
 *
 * 	For more details on the CRC algorithm used, see Listing 4 in
 * 	@url http://www.drdobbs.com/tools/understanding-crcs/184410177
 *
 * 	The following definitions can be put in app.h to customize behavior:
 *
 *  Define an alternate 16-bit polynomial (0x1021 in this case)
 *  #define CRC16_GEN_POLY 0x1021
 *
 *  Use bitwise CRC (less RAM but longer computation)
 *  #define CRC16_BITWISE
 *
 * Copyright (c) 2018 Control Solutions LLC.  All Rights Reserved.
 */

#ifndef _CRC16_H_
#define _CRC16_H_

#include "app.h"
#include <stdint.h>

//------------------------------//----------------------------------------------
// Definitions

// the initial value to use when starting a CRC computation
#ifndef CRC16_INIT
#define CRC16_INIT 0xFFFF
#endif

//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Public Functions

/**
 * @brief      Updates the CRC as a result of a new data byte
 *
 * @param[in]  crc   The initial value of the CRC
 * @param[in]  data  The data byte
 *
 * @return     Updated CRC value
 */
uint16_t CRC16_Update( uint16_t crc, uint8_t data );

/**
 * @brief      Initialize the CRC table
 *
 * Call once on system startup
 */
void CRC16_Initialize( void );

#endif
