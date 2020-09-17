/**
 * IO Pin mapping
 *
 * Contains input/output pin utilities and mapping to hardware pins
 */
#ifndef __IO_MAPPING_H
#define __IO_MAPPING_H

#include "mcc_generated_files/pin_manager.h"
#include <xc.h>

// This is a result of the oscillator configuration
#define FCY ( 39613750 )
#define TCY ( FCY / 2 )

// Put the dsPIC33 processor in low power mode until interrupt (#1 == IDLE_MODE)
#define SLEEP() asm( "PWRSAV #1" );

// prevent the watchdog timer from resetting the processor
#define KICK_WATCHDOG() asm( "CLRWDT" )

// Reset the processor
#define RESET() asm( "RESET" )

//------------------------------//----------------------------------------------
// Macros for controlling pins

#define OUTPUT_OFF( pin ) ( OPIN_##pin##_LAT = 0 )
#define OUTPUT_ON( pin ) ( OPIN_##pin##_LAT = 1 )
#define OUTPUT_STATUS( pin ) ( OPIN_##pin##_LAT )
#define OUTPUT_TOGGLE( pin ) ( OPIN_##pin##_LAT = !OPIN_##pin##_LAT )

#define INPUT_STATUS( pin ) ( IPIN_##pin##_PORT )

#define PIN_SET( pin, value ) ( OPIN_##pin##_LAT = value )

#define PIN_IS_INPUT( pin ) ( IPIN_##pin##_TRIS = 1 )
#define PIN_IS_OUTPUT( pin ) ( OPIN_##pin##_TRIS = 0 )
//#define PIN_IS_INPUT_PULLUP( pin ) ( pin##_TRIS = 1 )

#define LAT( port, bit ) ( LAT##port##bits.LAT##port####bit )
#define TRIS( port, bit ) ( TRIS##port##bits.TRIS##port####bit )
#define PORT( port, bit ) ( PORT##port##bits.R##port####bit )

//------------------------------//----------------------------------------------
// PIN Definitions

//------------------------------//----------------------------------------------
//  LEDs

//------------------------------//----------------------------------------------
// SPI
// SPI definitions.  Needed by drv_spi
//
//

#define DRV_SPI_CONFIG_CHANNEL_1_ENABLE
#define DRV_SPI_CONFIG_CHANNEL_2_ENABLE
//#define DRV_SPI_CONFIG_CHANNEL_3_ENABLE
//#define DRV_SPI_CONFIG_CHANNEL_4_ENABLE

// Which SPI channel is the eeprom wired to?
#define SPI_CHANNEL_EEPROM 2
// Which SPI channel is the expansion port wired to?
#define SPI_CHANNEL_EXPANSION 1
// Which SPI channel is the CAN2 chip wired to?
#define SPI_CHANNEL_CAN2 1

//------------------------------//----------------------------------------------
// CANBUS

// Control of Standby pin on CAN1 driver.  low = CAN is enabled
#define OPIN_CAN_STBY_OFF CAN_STBY_IO_RB7_SetLow
#define OPIN_CAN_STBY_ON CAN_STBY_IO_RB7_SetHigh

// Chip select for CAN2
// The no-ops are a bit arbitrary, just to ensure a minimum space between CS off and on.
// @todo check the data sheet for recommended timings
#define CAN2_CSOFF() CAN2_CS_IO_RD8_SetHigh()
#define CAN2_CSON()                                                                                                    \
    asm( "NOP" );                                                                                                      \
    asm( "NOP" );                                                                                                      \
    asm( "NOP" );                                                                                                      \
    asm( "NOP" );                                                                                                      \
    CAN2_CS_IO_RD8_SetLow()

#define OPIN_CAN2_STBY_OFF CAN2_Stby_IO_RE13_SetLow
#define OPIN_CAN2_STBY_ON CAN2_Stby_IO_RE13_SetHigh

#define IPIN_CAN2_INT() ( !CAN2_Int_IO_RG9_GetValue() )

// crystal frquency for MCP25625 chip
#define CAN2_FOSC 20000000

//------------------------------//----------------------------------------------
// Analog input channel definitions.  These refer to ANx inputs as managed by
// the adc.c
// driver.

#define ADC_CHANNEL_PBATTV 0
#define ADC_CHANNEL_PBATTI 1

#define ADC_CHANNEL_FUEL_LEVEL 3
#define ADC_CHANNEL_GENPWRI 4
#define ADC_CHANNEL_GENPWRV 5
#define ADC_CHANNEL_CO_VIN 6

#define ADC_CHANNEL_SHOREV 8
#define ADC_CHANNEL_SHOREI 9

#define ADC_CHANNEL_ANIN0 0
#define ADC_CHANNEL_ANIN1 1
#define ADC_CHANNEL_THROTTLE 2
#define ADC_CHANNEL_BATTLEVEL 3
#define ADC_CHANNEL_ACTUATOR_CURRENT 10
#define ADC_CHANNEL_VCONTROL_LEVEL 11
#define ADC_CHANNEL_BRK_SENSE 15

//------------------------------//----------------------------------------------
// IMU Definitions

// the device address for the IMU. Depends on how SD0/SA0 pin is connected
#define IMU_I2C_ADDR 0x6B

//------------------------------//----------------------------------------------
// EEPROM definitions.  Needed by eeprom.h

/**
 * Enumerate EEPROM sizes & choose which size EEPROM is present in the system
 */
#define MCHP_1KBIT 1
#define MCHP_2KBIT 2
#define MCHP_4KBIT 3
#define MCHP_8KBIT 4
#define MCHP_16KBIT 5
#define MCHP_32KBIT 6
#define MCHP_64KBIT 7
#define MCHP_128KBIT 8
#define MCHP_256KBIT 9
#define MCHP_512KBIT 10
#define MCHP_1MBIT 11

#define MCHP_EEPROM MCHP_256KBIT

// Macros to control the chip select
#define OPIN_EEPROM_CS_DISABLE EE_CS_IO_RC13_SetHigh
#define OPIN_EEPROM_CS_ENABLE EE_CS_IO_RC13_SetLow

//------------------------------//----------------------------------------------

#endif
