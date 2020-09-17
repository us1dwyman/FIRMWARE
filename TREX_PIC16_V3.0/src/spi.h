/**
 * Serial Peripheral Interface driver for PIC16
 *
 * Provides routines for reading and writing using the SPI peripheral.
 *
 * To use this module, the following can be configured in your app.h header
 * file:
 * 		ENABLE_SPI1: Compile in driver for SPI port 1
 * 		ENABLE_SPI2: Compile in driver for SPI port 2
 * 	Note that SPI2 may not be available in all hardware.
 * 		SPI_DUMMY_DATA: The byte to write out when reading from the SPI bus.
 * 		Normally this doesn't matter and can be left at the default
 *
 *  Note that this module does not do anything with chip select pins; this must be
 *  managed by the upper level code.  This module does not re-map the device
 *  pins, nor configure I/O pins.  This must be done prior to initializing and using
 *  the SPI driver.
 *  	SCK - Serial Clock (output)
 *  	MISO - Serial data from slave to master (Input)
 *  	MOSI - Serial data from master to slave (Ouput)
 *  	SS - Slave Select
 *
 *
 *  The driver may be used from Interrupt Service Routines; however the
 *  caller must arbitrate access to the bus so that an interrupt does not clobber
 *  a non-interrupt based transfer.  This is typically done by disabling any interrupt
 *  using the same SPI port while the non-interrupt access is occurring.
 *
 * Copyright (c) 2018 Control Solutions LLC.  All Rights Reserved.
 */
#ifndef _SPI_H
#define _SPI_H

#include "app.h"
#include <stdint.h>

//------------------------------//----------------------------------------------
// Definitions

#if !defined( SPI_DUMMY_DATA )
#define SPI_DUMMY_DATA 0xFF
#endif

//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Public functions

/**
 * @brief      Initializes the SPI port to the specified configuration
 *
 */
void SPI1_Initialize();
void SPI2_Initialize();

/**
 * @brief      Write a byte to the port
 *
 * This function is blocking and returns when the byte has been written.
 * The SPI port must already be Initialize(d).
 *
 * @param[in]  data  The data to write
 */
void SPI1_Put( uint8_t data );
void SPI2_Put( uint8_t data );

/**
 * @brief      Reads a byte from the SPI
 *
 * This is a blocking function that returns when the byte has been read.
 * The SPI port must already be Initialize(d).
 *
 * @return     The byte that was read.
 */
uint8_t SPI1_Get( void );
uint8_t SPI2_Get( void );

/**
 * @brief      Writes a number of bytes to the SPI
 *
 * This is a blocking function that returns when the bytes have been written.
 * The SPI port must already be Initialize(d).
 *
 * @return     N/A
 */
void SPI1_PutBuffer( uint8_t *pData, uint8_t count );
void SPI2_PutBuffer( uint8_t *pData, uint8_t count );

/**
 * @brief      Reads a number of bytes from the SPI
 *
 * This is a blocking function that returns when the bytes have been read.
 * The SPI port must already be Initialize(d).
 *
 * @return     N/A
 */
void SPI1_GetBuffer( uint8_t *pData, uint8_t count );
void SPI2_GetBuffer( uint8_t *pData, uint8_t count );

#endif
