/**
 * @file api_spi.h
 * @brief Defines the interface to the SPI bus
 *
 * @copyright Copyright (c) 2018 Control Solutions LLC.  All Rights Reserved.
 *
 */

#ifndef _API_SPI_H_
#define _API_SPI_H_

#include "api.h"
#include "drv_spi.h"
#include <stdint.h>

/** \defgroup    spi Serial Peripheral Interface
 * The Serial Peripheral Interface implements a synchronous serial protocol can be used to communicate with a variety of
 * slave devices via the J5 Expansion connector.
 *
 * Each device typically has a Configuration (which defines the communication format) and has a Chip Select (SS)
 * assigned to it which enables the device.
 *
 * The Expansion connector has MISO (Master In, Slave Out), MOSI (Master Out, Slave In), and SCK (Serial
 * Clock) lines.  It also has two Chip Selects (CS1, CS2).  CS1 also gates MOSI and SCK signals so an off-board
 * device will not see any signals on those lines unles CS1 is enabled.
 *
 * Typical use of a device is as follows:
 *    1. Call API_SPI_BeginTransaction() with the desired device configuration
 *
 *    2. Enable chip select API_SPI_CS1_ENABLE() or API_SPI_CS2_ENABLE()
 *    3. Use API_SPI_Get() or API_SPI_Put() to send and receive bytes as necessary for the device
 *    4. Disable chip select API_SPI_CS1_DISABLE() or API_SPI_CS2_DISABLE()
 *.   5. Repeat steps 2-4 as necessary
 *
 *    6. Call API_SPI_EndTransaction()
 *
 * NOTE: SPI transfers and transactions are synchronous, which means other code is blocked while you are
 * communicating with the off-board device.  Blocking the rest of the application for a long time can
 * cause adverse side effects.
 *
 *
 */

//------------------------------//----------------------------------------------
// Definitions

/** \ingroup spi
 * Enable (pull low) the CS1 signal on J5 Expansion Connector
 *
 * This signal also enables the SPI (SCK, SDO) signals on J5
 *
 */
#define API_SPI_CS1_ENABLE() CS1_IO_RB12_SetHigh()

/** \ingroup spi
 * Disable (pull high) the CS1 signal on J5 Expansion Connector
 *
 * This signal also disables (gates) the SPI (SCK, SDO) signals on J5
 *
 */
#define API_SPI_CS1_DISABLE() CS1_IO_RB12_SetLow()

/** \ingroup spi
 * Enable (pull low) the CS2 signal on J5 Expansion Connector
 *
 */
#define API_SPI_CS2_ENABLE()                                                                                           \
    CS1_IO_RB12_SetHigh();                                                                                             \
    CS2_IO_RB11_SetLow()

/** \ingroup spi
 * Disable (pull high) the CS2 signal on J5 Expansion Connector
 *
 */
#define API_SPI_CS2_DISABLE()                                                                                          \
    CS1_IO_RB12_SetLow();                                                                                              \
    CS2_IO_RB11_SetHigh()

// Primary prescale definitions, see chip datasheet
#define API_SPI_PPRE_1_1 0b11
#define API_SPI_PPRE_4_1 0b10
#define API_SPI_PPRE_16_1 0b01
#define API_SPI_PPRE_64_1 0b00

// Secondary prescale definitions, see chip datasheet
#define API_SPI_SPRE_1_1 0b111
#define API_SPI_SPRE_2_1 0b110
#define API_SPI_SPRE_3_1 0b101
#define API_SPI_SPRE_4_1 0b100
#define API_SPI_SPRE_5_1 0b011
#define API_SPI_SPRE_6_1 0b010
#define API_SPI_SPRE_7_1 0b001
#define API_SPI_SPRE_8_1 0b000

//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Public Functions

/** \ingroup spi
 * Reserves the SPI bus for a specific device
 *
 * Configures the SPI interface for the referenced device, and reserves
 * it for use.
 * The configuration array contains the following members:
 *
 *    channel;    // Must always be SPI_CHANNEL_EXPANSION
 *
 *    primaryPrescale;  // primary clock divider, one of the following:
 *    \code
 *      API_SPI_PPRE_1_1
 *      API_SPI_PPRE_4_1
 *      API_SPI_PPRE_16_1
 *      API_SPI_PPRE_64_1
 *    \endcode
 *
 *    secondaryPrescale;  // secondary clock divider, one of the following
 *    \code
 *      API_SPI_SPRE_1_1
 *      API_SPI_SPRE_2_1
 *      API_SPI_SPRE_3_1
 *      API_SPI_SPRE_4_1
 *      API_SPI_SPRE_5_1
 *      API_SPI_SPRE_6_1
 *      API_SPI_SPRE_7_1
 *      API_SPI_SPRE_8_1
 *    \endcode
 *
 *    cke; // one of the following:
 *    \code
 *      SPI_CKE_IDLE_ACT   // data change is on active clock to idle clock state
 *      SPI_CKE_ACT_IDLE   // data change is on idle clock to active clock state
 *    \endcode
 *
 *    spibus_mode; // one of the following:
 *    \code
 *        SPI_BUS_MODE_0 // smp = 0, ckp = 0
 *        SPI_BUS_MODE_1, // smp = 1, ckp = 0
 *        SPI_BUS_MODE_2, // smp = 0, ckp = 1
 *        SPI_BUS_MODE_3, // smp = 1, ckp = 1
 *    \endcode
 *
 *    mode;  // one of the following:
 *    \code
 *        SPI_TRANSFER_MODE_16BIT
 *        SPI_TRANSFER_MODE_8BIT
 *    \endcode
 *
 *
 * @param[in] pConfig pointer to configuration array
 */
#define API_SPI_BeginTransaction( pConfig )                                                                            \
    while( !DRV_SPI_Lock( SPI_CHANNEL_EXPANSION ) )                                                                    \
    {                                                                                                                  \
    }                                                                                                                  \
    DRV_SPI_Initialize( pConfig )

/** \ingroup spi
 * Releases the SPI bus when a transaction is finished.
 *
 *
 */
#define API_SPI_EndTransaction() DRV_SPI_Unlock( SPI_CHANNEL_EXPANSION )

/** \ingroup spi
 * Send a byte to the SPI device.
 *
 * @param[in] byteToSend the value to be sent.
 */
#define API_SPI_Put( byteToSend ) DRV_SPI_Put( SPI_CHANNEL_EXPANSION, byteToSend )

/** \ingroup spi
 * Retrieve a byte from the SPI device.
 *
 * @return byte that was read
 */
#define API_SPI_Get() DRV_SPI_Get( SPI_CHANNEL_EXPANSION )

#endif
