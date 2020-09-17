/**
 * 16-bit Cyclic Redundancy check calculations
 *
 * This module uses a 256-byte precalculated table to compute the CRC over a block of data
 *
 */
#include "crc16.h"

//------------------------------//----------------------------------------------
// Definitions

#define CRC_TABLE_SIZE 256

// the polynomial can be specified in the header file.  Otherwise default
#ifndef CRC16_GEN_POLY
// The Generator Polynomial: x^16 + x^15 + x^2 + 1
#define CRC16_GEN_POLY 0xA001
#endif

//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Local variables

// Stores a lookup table computed in _Initialize
static uint16_t _CRCTable[ CRC_TABLE_SIZE ];

//------------------------------//----------------------------------------------
// Local functions

//------------------------------//----------------------------------------------
// Public functions

/**
 * @brief      Updates the CRC as a result of a new data byte
 *
 * @param[in]  crc   The initial value of the CRC
 * @param[in]  data  The data byte
 *
 * @return     Updated CRC value
 */
uint16_t CRC16_Update( uint16_t crc, uint8_t data )
{
    return ( crc >> 8 ) ^ _CRCTable[ ( crc ^ (uint16_t)data ) & 0x00FF ];
}

/**
 * @brief      Initialize the CRC table
 *
 * Builds the local CRC lookup table
 */
void CRC16_Initialize( void )
{
    uint16_t i;
    uint16_t j;
    uint16_t crc;
    uint16_t c;

    for( i = 0; i < CRC_TABLE_SIZE; i++ )
    {
        crc = 0;
        c = i;

        for( j = 0; j < 8; j++ )
        {

            if( ( crc ^ c ) & 0x0001 )
                crc = ( crc >> 1 ) ^ CRC16_GEN_POLY;
            else
                crc = crc >> 1;

            c = c >> 1;
        }

        _CRCTable[ i ] = crc;
    }
}
