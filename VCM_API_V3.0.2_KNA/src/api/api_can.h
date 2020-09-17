/**
 * @file api_can.h
 * @brief CANBUS Controller Module
 *
 * @copyright Copyright (c) 2018 Control Solutions LLC.  All Rights Reserved.
 *
 */

#ifndef _API_CAN_H_
#define _API_CAN_H_

#include "app.h"
#include "can.h"
#include "hardware.h"
#include "mcc_generated_files/can_types.h"

//------------------------------//----------------------------------------------
// Definitions

//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Public functions

/** \defgroup    canbus CANBUS interface
 * Provides an interface to send and receive CAN messages
 *
 * The CANBUS interface is a packet-oriented interface designed for communication
 * between various subsystems of the vehicle.
 *
 * CANBUS messages have an 8-byte payload, as well as either an 11-bit or 29-bit
 * message identifier.
 *
 * This module maintains a queue of outgoing packets, which may use either 11-bit
 * or 29-bit identifiers.  The CAN hardware handles collision avoidance and bus arbitration.
 *
 * Upon initialization, a set of filter masks is defined that determine which messages
 * are received.  This hardware receive filtering reduces the load on the processor since
 * only messages that are directed at us are received.  Received packets are queued
 * for processing by the APICB_CanRx() function, which must be defined by the application.
 *
 * To use this driver, call API_EnableCan() with the desired configuration parameters.
 *
 * The
 * APICB_CanRx() callback will be called for each message in the received queue until the queue is empty.
 *
 * You may call API_SendCanMessage() at any time to send a message.
 *
 */

/** \ingroup    canbus Enable CAN bus
 *
 * Enabling the interface will put it 'on bus', making it possible to send and
 * receive CAN messages.  The
 * masks and filters are used to determine which incoming messages will be
 * accepted and passed to the APICB_CanRx() function.
 *
 */
void API_EnableCan( void );

/** \ingroup canbus
 * Disable CAN bus
 *
 * Disabling takes the interface off bus (in standby)
 */
void API_DisableCan( void );

/** \ingroup canbus
 * Send message on CAN bus
 *
 * The message will be queued if a transmission is already in progress.
 * Queued messages will be sent as soon as the bus is free.
 *
 * @param[in] pMsg pointer to structure containing the CAN message
 * @return false if there is no room in the outgoing queue, or true otherwise
 */
bool API_SendCanMessage( uCAN_MSG *pMsg );

/** \ingroup callback
 * New message received on the CANBUS
 *
 * The API will call this function when a CAN message matching the configured
 * filters is received.
 *
 */
void APICB_CanRx( uCAN_MSG *pMsg );

/** \ingroup    canbus
 * Enable CAN 2 interface
 *
 * Enabling the interface will put it 'on bus', making it possible to send and
 * receive CAN messages.  The
 * masks and filters are used to determine which incoming messages will be
 * accepted and passed to the APICB_CanRx() function.
 *
 */
void API_EnableCan2( void );

/** \ingroup canbus
 * Disable CAN 2 interface
 *
 * Disabling takes the interface off bus (in standby, low power mode)
 */
void API_DisableCan2( void );

/** \ingroup canbus
 * Send message on CAN bus 2
 *
 * The message will be queued if a transmission is already in progress.
 * Queued messages will be sent as soon as the bus is free.
 *
 * @param[in] pMsg pointer to structure containing the CAN message
 * @return false if there is no room in the outgoing queue, or true otherwise
 */
bool API_SendCan2Message( can_t *pMsg );

/** \ingroup callback
 * New message received on CANBUS 2
 *
 * The API will call this function when a CAN message matching the configured
 * filters is received.
 *
 */
void APICB_Can2Rx( can_t *pMsg );


void API_Can2_UpdateTxMailbox( unsigned index, uint8_t *pData, uint8_t dlc, bool bSendNow );

void API_Can2_EnableTxMailbox( unsigned index, uint32_t id, uint16_t interval );

void API_Can2_DisableTxMailbox( unsigned index );

/**
 * Powerup Initialization
 * @private
 */
void API_Can_Initialize( void );

/**
 * Module task loop
 * @private
 */
void API_Can_Tasks( void );

/**
 * Debug dump function for this module
 *
 * @private
 */
void API_Can_Dump( void );

#endif
