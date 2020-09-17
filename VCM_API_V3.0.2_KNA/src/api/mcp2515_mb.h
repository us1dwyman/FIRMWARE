/**
 * Mailbox manager for CAN
 *
 * Implements RX and TX mailboxes for CANBUS.
 *
 * Copyright (c) 2019 Control Solutions LLC.  All Rights Reserved.
 */

#ifndef __MCP2515_MB
#define __MCP2515_MB

#include "can.h"
#include <stdbool.h>
#include <stdint.h>

//------------------------------//----------------------------------------------
// Definitions

typedef struct
{
    volatile bool Status;
    volatile uint16_t interval;
    volatile uint16_t tickCount;
    can_t msg;
} CAN_Mailbox_t;

//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Public functions

void MCP2515MB_Initialize( void );

void MCP2515MB_Tasks( void );

void MCP2515MB_UpdateTxMailbox( unsigned index, uint8_t *pData, uint8_t dlc, bool bSendNow );

void MCP2515MB_EnableTxMailbox( unsigned index, uint32_t id, uint16_t interval );

void MCP2515MB_DisableTxMailbox( unsigned index );

#endif
