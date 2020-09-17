/**
 * Application global configuration
 *
 *
 */

#ifndef _APP_H
#define _APP_H

#include <stdbool.h>
#include <stdint.h>

//------------------------------//----------------------------------------------
// Version.  See semver.org for guidelines
#define VERSION_MAJOR 2
#define VERSION_MINOR 0
#define VERSION_PATCH 1

//------------------------------//----------------------------------------------
// CANBUS

// CANBUS baud rate in kbps
#define CAN_BITRATE 500

// CAN Message identifiers (PGNs)
#define CAN_PGN_HMI_BUTTONS 0xFF10UL
#define CAN_PGN_VCM_STATUS0 0xFF20UL

// Device addresses used on the CANBUS
#define CANADDR_VCM 0x80U
#define CANADDR_VCM_0 0x8000U
#define CANADDR_VCM_1 0x8001U
#define CANADDR_VCM_2 0x8002U
#define CANADDR_HMI 0x81U

// my device address
#define CANADDR_MINE CANADDR_HMI

// The number of outgoing pending messages we can queue
// power of 2, max size restricted, see cbuf.h
#define CAN_txMessageQ_SIZE 16U
#define CAN_rxMessageQ_SIZE 16U

#endif
