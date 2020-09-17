/**
 * System (hardware) configuration settings
 *
 * @copyright Copyright (c) 2019 Control Solutions LLC.  All Rights Reserved
 * @private
 */

#ifndef __SYSTEM_CONFIG_H
#define __SYSTEM_CONFIG_H

#include <xc.h>

//------------------------------//----------------------------------------------
// Program Flash
// These define where the bootloader is allowed to program FLASH and how
// it starts the main Application.  These constants need to be compatible with
//      - The bootloader.gld linker file
//      - the application.gld linker file
//      - The settings in the MPLAB project (eg the 'Building->Execute this line after build project setting which
//      embeds the checksum in the Application image')

#if defined( __PIC24FJ256DA206__ ) || defined( __dsPIC33EP128MC506__ )
// the size of a program memory block- depends on the processor family
// This is how many bytes we need to program a block of FLASH
#define BL_FLASH_BLK_SIZE ( 8U * 192U )

// This is how many address locations are covered by the above flash block
// Since in PIC14, 192 bytes of instructions takes up 256 addresses
#define BL_ADDR_BLK_SIZE ( 8 * 256U )
#else
#error unknown processor
#endif

// Defines the start of program memory reserved for the APPLICATION.
// We don't write anything outside this area.
// These constants need to be aligned with the linker config file for
// both the bootloader and the Application.
// This is a WORD address.  Each processor instruction consumes 2 words/4 bytes
// This also needs to be aligned with the start of an ERASE Page ()
#define BL_C_APP_ADR 0X001800UL

// End of application space
// For this device, the program flash includes configuration words at the end (top) of flash.  Erasing the top page of
// program memory
//  erases these config words, which we don't want to do.
// Therefore we consider the application end to be:

#define END_FLASH 0x015800L

#define BL_C_APP_END ( END_FLASH - FLASH_ERASE_PAGE_SIZE_IN_PC_UNITS )

// The address we jump to when exiting the bootloader. The Application
// needs to be linked with this address as its entry point.
#define BL_APP_ENTRY_ADDR 0X001A00UL

//------------------------------//----------------------------------------------
// IO Pins

#define OPIN_CAN_STBY _LATB7

// Keyswitch input
#define KEYSWITCH_ON() !_RA10

//------------------------------//----------------------------------------------
// CANBUS Driver (see can.c)

// Maps CAN TX and RX functions to DMA channels
#define CAN1_TX_DMA_CHANNEL DMA_CHANNEL_1
#define CAN1_RX_DMA_CHANNEL DMA_CHANNEL_2

// Total DMA buffers avaiable for CAN
// Valid options are 4, 6, 8, 12, 16, 24, or 32.
#define CAN1_MESSAGE_BUFFERS 32

// Number of transmit buffers
#define CAN1_TX_BUFFER_COUNT 8

// Buffer index where the receive FIFO starts
// The size of the fifo is (CAN1_MESSAGE_BUFFERS - CAN1_FIFO_STARTING_BUFFER )
#define CAN1_FIFO_STARTING_BUFFER 0x8

// include code to update filters run-time
#undef CAN_ALLOW_FILTER_UPDATE

// whether CAN driver should use interrupts
#undef CAN_USE_INTERRUPTS

// Maximum number of RX filters supported by hardware
//#define CAN_MAX_FILTERS 16

// this is reduced for the bootloader, which just saves a few bytes of flash space
// since we don't need all the filters.
#define CAN_MAX_FILTERS 8

// Maximum number of RX filter masks supported by hardware
//#define CAN_MAX_MASKS 3

// this is reduced for the bootloader, which just saves a few bytes of flash space
// since we don't need all the filters.
#define CAN_MAX_MASKS 1

// Defines the size of the RAM queues used to hold CAN messages
// #define CAN_txMessageQ_SIZE 4U
// #define CAN_rxMessageQ_SIZE 4U

// IO pin controlling transceiver standby
#define CAN_STANDBY( x ) OPIN_CAN_STBY = x

typedef struct
{

    uint16_t CiCFG1;
    uint16_t CiCFG2;
    uint16_t CiCTRL1;
    uint16_t CiCTRL2;

    // Rx filter enables
    uint16_t CiFEN1;
    uint16_t CiFCTRL;

    // Transmit buffer enable and priority setting
    uint16_t CiTR01CON;
    uint16_t CiTR23CON;
    uint16_t CiTR45CON;
    uint16_t CiTR67CON;

    // Filter mask IDs, 32 bits each
    uint16_t CiRXM[ CAN_MAX_MASKS << 2 ];

    // Filter SID, 32 bits each
    uint16_t CiRXFxSID[ CAN_MAX_FILTERS << 2 ];

    // Filter to DMA buffer mapping
    uint16_t CiBUFPNT[ CAN_MAX_FILTERS >> 2 ];

    // Filter mask selection
    uint16_t CiFMSKSEL[ CAN_MAX_FILTERS >> 3 ];

} CAN_Config_t;

//------------------------------//----------------------------------------------
// System timer configuration

// Period = 0.256 s; Frequency = 40000000 Hz; PR1 40000;
#define TIMER_PRELOAD 0x9C40

/**
 * @brief      Restart the timer that decides if we should launch the app
 *
 * @return     { description_of_the_return_value }
 */
#define LaunchTimerReset()                                                                                             \
    TMR3HLD = 0;                                                                                                       \
    TMR2 = 0;                                                                                                          \
    IFS0bits.T3IF = 0;

#define LaunchTimerExpired() IFS0bits.T3IF

//------------------------------//----------------------------------------------

#endif
