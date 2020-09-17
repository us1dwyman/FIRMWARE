/**
 * Hardware driver for the EEPROM
 *
 * Implements basic read/write functions for EEPROM
 *
 * You need to define the EEPROM size in the app header file
 * (e.g. #define MCHP_EEPROM  MCHP_16KBIT)
 *
 * Copyright (c) 2015 Control Solutions LLC. All Rights Reserved
 */

#ifndef _EEPROM_H
#define _EEPROM_H

#include <stdbool.h>
#include <stdint.h>

#include "hardware.h"

/**
 * Figure out the page size
 */
#if MCHP_EEPROM == 0
#error MCHP_EEPROM is not defined
#elif MCHP_EEPROM < MCHP_32KBIT
#define NVM_PAGE_SIZE 16
#elif MCHP_EEPROM < MCHP_128KBIT
#define NVM_PAGE_SIZE 32
#elif MCHP_EEPROM < MCHP_512KBIT
#define NVM_PAGE_SIZE 64
#elif MCHP_EEPROM < MCHP_1MBIT
#define NVM_PAGE_SIZE 128
#elif MCHP_EEPROM == MCHP_1MBIT
#error Microchip 1MBit EEPROM is not supported at this time. User need to modify the EEPROM access function to make it work. The address must be 3 uint8_ts.
#else
#error Invalid MCHP EEPROM part
#endif

/**
 * Figure out the total size in bytes
 */
#if MCHP_EEPROM == MCHP_1KBIT
#define TOTAL_NVM_BYTES 128
#elif MCHP_EEPROM == MCHP_2KBIT
#define TOTAL_NVM_BYTES 256
#elif MCHP_EEPROM == MCHP_4KBIT
#define TOTAL_NVM_BYTES 512
#elif MCHP_EEPROM == MCHP_8KBIT
#define TOTAL_NVM_BYTES 1024
#elif MCHP_EEPROM == MCHP_16KBIT
#define TOTAL_NVM_BYTES 2048
#elif MCHP_EEPROM == MCHP_32KBIT
#define TOTAL_NVM_BYTES 4096
#elif MCHP_EEPROM == MCHP_64KBIT
#define TOTAL_NVM_BYTES 8192
#elif MCHP_EEPROM == MCHP_128KBIT
#define TOTAL_NVM_BYTES 16384
#elif MCHP_EEPROM == MCHP_256KBIT
#define TOTAL_NVM_BYTES 32768
#elif MCHP_EEPROM == MCHP_512KBIT
#define TOTAL_NVM_BYTES 65535
#elif MCHP_EEPROM == MCHP_1MBIT
#error Microchip 1MBit EEPROM is not supported at this time. User need to modify the EEPROM access function to make it work. The address must be 3 uint8_ts.
#else
#error MCHP_EEPROM is not defined
#endif

/**
 * Read block of data from EEPROM into destination buffer
 *
 */
void EEPROM_Read( void *pDest, uint16_t addr, uint16_t count );

/**
 * Write block of data from buffer to EEPROM
 *
 * @param pSource pointer to byte(s) to be written
 * @param address byte offset from the start of the device (0-based)
 * @param count number of bytes to be written
 * @param fill if true, the block is filled with the byte value stored in pSource
 *
 */
void EEPROM_Write( void *pSource, uint16_t addr, uint16_t count, bool fill );

// Block protection choices
#define EEPROM_BLOCK_NONE 0
#define EEPROM_BLOCK_TOP_QUARTER 1
#define EEPROM_BLOCK_TOP_HALF 2
#define EEPROM_BLOCK_ALL 3

/**
 * Sets the device block protection state (read-only blocks)
 *
 */
void EEPROM_SetBlockProtect( uint8_t block );

/**
 * Reads the current device block protection state
 *
 */
uint8_t EEPROM_GetBlockProtect( void );

/**
 * Calculates a simple checksum over a block of bytes
 *
 */
uint8_t EEPROM_Checksum( uint16_t addr, uint16_t count );

#endif
