/**
 * Driver for Microchip 25xx SPI Serial EEPROMs
 *
 * Ref: http://ww1.microchip.com/downloads/en/DeviceDoc/21822G.pdf
 *
 * http://www.microchip.com/stellent/groups/SiteComm_sg/documents/DeviceDoc/en542958.pdf
 *
 * Implements the required SPI interface to support
 *      - Write
 *      - Read
 *      - Block Protect
 *
 * The SPI interface must be initialized prior to calling these functions
 * and SPI access macros must be defined in a header file
 *
 * Copyright (c) 2015 Control Solutions LLC. All Rights Reserved
 *
 */

#include "drv_spi.h"
#include "eeprom.h"
#include "hardware.h"

// These macros glue the hardware interface to this driver
#define EESPIPut( x ) DRV_SPI_Put( SPI_CHANNEL_EEPROM, x )
#define EESPIGet() DRV_SPI_Get( SPI_CHANNEL_EEPROM )

#define EE_SPI_START()                                                                                                 \
    while( !DRV_SPI_Lock( SPI_CHANNEL_EEPROM ) )                                                                       \
        ;                                                                                                              \
    DRV_SPI_Initialize( &_spiConfig )

#define EE_SPI_END()                                                                                                   \
    DRV_SPI_Deinitialize( SPI_CHANNEL_EEPROM );                                                                        \
    DRV_SPI_Unlock( SPI_CHANNEL_EEPROM )

#if !defined( EESPIPut ) || !defined( EESPIGet )
#error "You must provide SPI access functions"
#endif

#if !defined( EE_SPI_START )
#define EE_SPI_START()
#endif

#if !defined( EE_SPI_END )
#define EE_SPI_END()
#endif

// macros for readability
#define EE_SELECT() OPIN_EEPROM_CS_ENABLE()
#define EE_DESELECT() OPIN_EEPROM_CS_DISABLE()

// SPI Command set
#define EE_WRSR 0x01
#define EE_WRITE 0x02
#define EE_READ 0x03
#define EE_DIS_WRT 0x04
#define EE_RD_STATUS 0x05
#define EE_EN_WRT 0x06

// Holds the SPI driver configuration used to communicate with the device
// These settings need to be compatible with the device we are talking to!
static const DRV_SPI_INIT_DATA _spiConfig = {
    SPI_CHANNEL_EEPROM,     // channel
    0b10,                   // primaryPrescale
    0b111,                  // secondaryPrescale
    SPI_CKE_ACT_IDLE,       // cke
    SPI_BUS_MODE_0,         // spi mode
    SPI_TRANSFER_MODE_8BIT  // transfer mode
};

/**
 * Read block of data from EEPROM into destination buffer
 *
 */
void EEPROM_Read( void *pDest, uint16_t addr, uint16_t count )
{
    // cast to byte pointer so we can increment
    uint8_t *pByte = (uint8_t *)pDest;

    uint8_t temp = 0;

    // Arbitrate access to the SPI bus
    EE_SPI_START();

    // wait until any in-progress write cycle is completed
    do
    {
        EE_SELECT();

        // Read the status word and check the WIP bit
        EESPIPut( EE_RD_STATUS );

        temp = EESPIGet();

        EE_DESELECT();

        Nop();

    } while( temp & 0x01 );

    // activate chip select
    EE_SELECT();

// Send the write command and address to start reading
// depending on the chip type
#if MCHP_EEPROM < MCHP_4KBIT
    EESPIPut( EE_READ );
    EESPIPut( addr );
#elif MCHP_EEPROM == MCHP_4KBIT
    if( addr > 0xFF )
    {
        EESPIPut( EE_READ | 0x08 );
    }
    else
    {
        EESPIPut( EE_READ );
    }
    EESPIPut( addr );
#elif MCHP_EEPROM < MCHP_1MBIT
    EESPIPut( EE_READ );  // 0x03
    EESPIPut( addr >> 8 );
    EESPIPut( addr );
#endif

    DRV_SPI_GetBuffer( SPI_CHANNEL_EEPROM, pByte, count );

    // // Now the bytes start coming from the chip
    // while( count > 0 )
    // {
    //     *pByte++ = EESPIGet();
    //     count--;
    // }

    // When we have enough, disable the chip select
    EE_DESELECT();

    // Release our claim to the SPI bus
    EE_SPI_END();
}

/**
 * Write block of data from buffer to EEPROM.
 *
 * If the fill parameter is true, the block is filled with the single byte
 * stored at pSource
 *
 */
void EEPROM_Write( void *pSource, uint16_t addr, uint16_t count, bool fill )
{
    // cast to byte pointer so we can increment
    uint8_t *pByte = (uint8_t *)pSource;

    // keep track of where we are on a page
    // (block writes can't cross a page boundary)
    uint8_t PageCounter = 0;

    // Arbitrate access to the SPI bus
    EE_SPI_START();

EEPROM_NEXT_PAGE:

    // wait until any in-progress write cycle is completed
    do
    {
        EE_SELECT();

        // Read the status word and check the WIP bit
        EESPIPut( EE_RD_STATUS );

        // PageCounter is just used as a temp variable
        PageCounter = EESPIGet();

        EE_DESELECT();

        Nop();

    } while( PageCounter & 0x01 );

    // Set the write enable latch by issuing the command inside a chip select toggle
    EE_SELECT();
    EESPIPut( EE_EN_WRT );
    EE_DESELECT();
    Nop();

    // Issue the write command and start address (format depends on chip type)
    EE_SELECT();

#if MCHP_EEPROM < MCHP_4KBIT
    EESPIPut( EE_WRITE );
    EESPIPut( addr );
#elif MCHP_EEPROM == MCHP_4KBIT
    if( addr > 0xFF )
    {
        EESPIPut( EE_WRITE | 0x08 );
    }
    else
    {
        EESPIPut( EE_WRITE );
    }
    EESPIPut( addr );
#elif MCHP_EEPROM < MCHP_1MBIT
    EESPIPut( EE_WRITE );
    EESPIPut( addr >> 8 );
    EESPIPut( addr );
#endif

    // Keep track of how many bytes we have written in this page
    PageCounter = 0;

    // Keep going until we write all the bytes (or hit a page boundary)
    while( count > 0 )
    {
        // Write the next byte to the device
        EESPIPut( *pByte );

        // if we are filling with a single byte, don't bump the pointer
        if( !fill )
            pByte++;

        count--;
        PageCounter++;

        // Check to see if we hit a page boundary
        if( ( ( addr + PageCounter ) & ( NVM_PAGE_SIZE - 1 ) ) == 0 )
        {
            // If so, release the chip enable to start the write cycle
            // and move on to the next page
            EE_DESELECT();

            addr += PageCounter;
            goto EEPROM_NEXT_PAGE;
        }
    }

    // When finished, disable the chip select (starts the write cycle)
    EE_DESELECT();

    // Release our claim to the SPI bus
    EE_SPI_END();
}

/**
 * Sets the device block protection state (read-only blocks)
 *
 */
void EEPROM_SetBlockProtect( uint8_t block )
{
    uint8_t temp;

    // Arbitrate access to the SPI bus
    EE_SPI_START();

    // wait until any in-progress write cycle is completed
    do
    {
        EE_SELECT();

        // Read the status word and check the WIP bit
        EESPIPut( EE_RD_STATUS );

        // PageCounter is just used as a temp variable
        temp = EESPIGet();

        EE_DESELECT();

        Nop();

    } while( temp & 0x01 );

    // Issue the write enable command
    EE_SELECT();
    EESPIPut( EE_EN_WRT );
    EE_DESELECT();
    Nop();

    // Write the command to the chip
    EE_SELECT();
    EESPIPut( EE_WRSR );
    EESPIPut( block << 2 );
    EE_DESELECT();

    // Release our claim to the SPI bus
    EE_SPI_END();
}

/**
 * Reads the current device block protection state
 *
 */
uint8_t EEPROM_GetBlockProtect( void )
{
    uint8_t bp;

    // Arbitrate access to the SPI bus
    EE_SPI_START();

    EE_SELECT();

    // Read the status word
    EESPIPut( EE_RD_STATUS );

    // PageCounter is just used as a temp variable
    bp = EESPIGet();

    EE_DESELECT();

    // Release our claim to the SPI bus
    EE_SPI_END();

    // return the block protect bits
    return ( bp >> 2 ) & 0x3;
}

/**
 * Calculates a simple checksum over a block of bytes
 *
 * Checksum is the ones complement of the sum of all the bytes.
 * The NVRAM is read in blocks for efficiency (the area being
 * read does not need to be aligned to a page boundary, we just
 * use the page size as a convenient data chunk.  With the current
 * hardware, there is no need or benefit to align the blocks with
 * device pages.

 */
uint8_t EEPROM_Checksum( uint16_t addr, uint16_t count )
{
    uint8_t cks = 0;

    // Arbitrate access to the SPI bus
    EE_SPI_START();

    // wait until any in-progress write cycle is completed
    do
    {
        EE_SELECT();

        // Read the status word and check the WIP bit
        EESPIPut( EE_RD_STATUS );

        // cks is just used as a temp variable
        cks = EESPIGet();

        EE_DESELECT();

        Nop();

    } while( cks & 0x01 );

    // activate chip select
    EE_SELECT();

    cks = 0;
// Send the write command and address to start reading
// depending on the chip type
#if MCHP_EEPROM < MCHP_4KBIT
    EESPIPut( EE_READ );
    EESPIPut( addr );
#elif MCHP_EEPROM == MCHP_4KBIT
    if( addr > 0xFF )
    {
        EESPIPut( EE_READ | 0x08 );
    }
    else
    {
        EESPIPut( EE_READ );
    }
    EESPIPut( addr );
#elif MCHP_EEPROM < MCHP_1MBIT
    EESPIPut( EE_READ );
    EESPIPut( addr >> 8 );
    EESPIPut( addr );
#endif

    // Now the bytes start coming from the chip
    while( count > 0 )
    {
        cks += EESPIGet();
        count--;
    }

    // When we have enough, disable the chip select
    EE_DESELECT();

    // Release our claim to the SPI bus
    EE_SPI_END();

    return ( ~cks );
}
