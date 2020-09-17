/**
 * Serial Peripheral Interface driver for PIC16
 *
 * Refer to header file for usage
 *
 * Copyright (c) 2018 Control Solutions LLC.  All Rights Reserved.
 */

#include "spi.h"
#include "system_config.h"
#include <xc.h>

//------------------------------//----------------------------------------------
// Definitions

//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Local variables

//------------------------------//----------------------------------------------
// Local functions

//------------------------------//----------------------------------------------
// Public functions

/**
 * @brief      Initializes the SPI port to the specified configuration
 *
 * @param[in]  pInit  Pointer to configuration structure
 */
#if defined( SPI1_ENABLE )
void SPI1_Initialize( void )
{
    SSP1CON1 = 0;           // DISABLE SPI1
    SSP1STAT = 0B11000000;  // TX-RISING/RX-DECENDING
    SSP1CON1 = 0B00100000;  // IDLE LOW/SPI MASTER SS DISABLED/SPEED 8MHZ(32MHZ/4)
}
#endif

#if defined( SPI1_ENABLE )
/**
 * @brief      Write a byte to the port
 *
 */
void SPI1_Put( uint8_t c )
{
    SSP1BUF = c;
    while( SSP1STATbits.BF == 0 )
        ;
}
#endif

#if defined( SPI2_ENABLE )
/**
 * @brief      Write a byte to the port
 *
 */
void SPI2_Put( uint8_t c )
{
    SSP2BUF = c;
    while( SSP2STATbits.BF == 0 )
        ;
}
#endif

#if defined( SPI1_ENABLE )

void SPI1_PutBuffer( uint8_t *pBuf, uint8_t count )
{
    while( count > 0 )
    {
        count--;
        SSP1BUF = *pBuf++;
        while( SSP1STATbits.BF == 0 )
            ;
    }
    return;
}

#endif

#if defined( SPI1_ENABLE )

uint8_t SPI1_Get( void )
{
    SSP1BUF = SPI_DUMMY_DATA;
    while( SSP1STATbits.BF == 0 )
        ;
    return SSP1BUF;
}
#endif

#if defined( SPI1_ENABLE )

void SPI1_GetBuffer( uint8_t *pBuf, uint8_t count )
{
    while( count > 0 )
    {
        count--;
        SSP1BUF = SPI_DUMMY_DATA;
        while( SSP1STATbits.BF == 0 )
            ;
        *pBuf++ = SSP1BUF;
    }
}
#endif