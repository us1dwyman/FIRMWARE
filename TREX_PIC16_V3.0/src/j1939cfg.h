/**
 * Defines application-specific setup of the J1939/CAN module
 *
 *
 * Copyright (c) 2015 Control Solutions LLC. All Rights Reserved
 */

#ifndef J1939CFG_H
#define J1939CFG_H

// CAN hardware driver interface
#include "can.h"

//------------------------------//----------------------------------------------
// Optional features
// Undefine these to save some code space if you don't need them

// If defined, we will perform the address claim procedure at startup
// Otherwise we will just the  J1939CFG_SA address defined below
#undef J1939_ADDRESS_CLAIM

// Define this to enable 'babbling idiot protection'
#undef J1939_BIP

// define this to enable transport protocol (messages > 8 bytes)
#define J1939_TP

#define DEBUG_DISABLE_RX_TIMER


//------------------------------//----------------------------------------------
// Application specific PGNs

// point-to-point command message
#define PGN_DIRECT 0xEF00U

//------------------------------//----------------------------------------------
// Definitions

// tick period in milliseconds (how often we call j1939_update)

#define J1939CFG_TICK_PERIOD 5

/* multi-packet TX buffer size */
#define J1939CFG_TP_TX_BUF_NUM 1
#define J1939CFG_TP_TX_BUF_SIZE 16

/* multi-packet RX buffer size */
#define J1939CFG_TP_RX_BUF_NUM 1
#define J1939CFG_TP_RX_BUF_SIZE 1785

/* NAME field found in J1939-81 */
#define J1939CFG_N_AAC ( 0U ) /* 1-bit Arbitrary Address Capable */
#define J1939CFG_N_IG ( 0U )  /* 3-bit Industry Group */
#define J1939CFG_N_VSI ( 0U ) /* 4-bit Vehicle System Instance */
#define J1939CFG_N_VS ( 0U )  /* 7-bit Vehicle System */
#define J1939CFG_N_F ( 0U )   /* 8-bit Function */
#define J1939CFG_N_FI ( 0U )  /* 5-bit Function Instance */
#define J1939CFG_N_EI ( 0U )  /* 3-bit ECU Instance */
#define J1939CFG_N_MC ( 0U )  /* 11-bit Manufacturer Code */
#define J1939CFG_N_IN ( 0UL ) /* 21-bit Indentity Number */

#endif
