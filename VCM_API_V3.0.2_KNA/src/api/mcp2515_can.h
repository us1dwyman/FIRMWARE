/**
 * Defines messaging interface to the MCP2515 CAN transceiver via SPI
 *
 *
 * Copyright (c) 2018 Control Solutions LLC.  All Rights Reserved.
 * @private
 */
#ifndef _MCP2515_H_
#define _MCP2515_H_

#include <stdbool.h>
#include <stdint.h>

//------------------------------//----------------------------------------------
// Definitions
//
//
//

/**
 * @enum CANCTRL_REQOP_MODE
 * @brief The possible operation modes of the chip
 *
 * Use MCP2515_SetMode() to set the mode of the chip
 *
 * @par
 * + OPMODE_NORMAL - Normal Operation mode
 * + OPMODE_SLEEP - Sleep mode
 * + OPMODE_LOOP - Loopback mode
 * + OPMODE_LISTEN - Listen-Only mode
 * + OPMODE_CONFIG - Configuration mode
 */
typedef enum
{
    CANCTRL_REQOP_NORMAL = 0x00,
    CANCTRL_REQOP_SLEEP = 0x20,
    CANCTRL_REQOP_LOOPBACK = 0x40,
    CANCTRL_REQOP_LISTENONLY = 0x60,
    CANCTRL_REQOP_CONFIG = 0x80,
    CANCTRL_REQOP_POWERUP = 0xE0
} CANCTRL_REQOP_MODE;

typedef enum
{

    OPMODE_NORMAL,
    OPMODE_SLEEP,
    OPMODE_LOOP,
    OPMODE_LISTEN,
    OPMODE_CONFIG

} MCP2515_Opmode_t;

//------------------------------//----------------------------------------------
// on-chip Register definitions
//

// CANSTAT
#define CANSTAT_ADR 0x0E
#define CANSTAT_MASK_REQOP 0xE0

// CANCTRL
#define CANCTRL_ADR 0X0F
#define CANCTRL_MASK_REQOP 0xE0
#define CANCTRL_MASK_ABAT 0x10
#define CANCTRL_MASK_OSM 0x08
#define CANCTRL_MASK_CLKEN 0x04
#define CANCTRL_MASK_CLKPRE 0x03

// CNFx
#define CNF3_ADR 0X28
#define CNF2_ADR 0X29
#define CNF1_ADR 0X2A

// Receive Buffers
// The start of the receive buffers.
// a 'MsgBuffer_t' structure lives there
#define RXB0CTRL_ADR 0x60
#define RXB1CTRL_ADR 0x70

#define RXBxCTRL_MASK_FILHIT0 0x01
#define RXBxCTRL_MASK_BUKT1 0x02
#define RXBxCTRL_MASK_BUKT 0x04
#define RXBxCTRL_MASK_RTR 0x08
#define RXBxCTRL_MASK_RXM 0x60

#define RXB0ID_ADR 0x61
#define RXB1ID_ADR 0x71

// transmit Buffers
// The start of the transmit buffers.
// a 'MsgBuffer_t' structure lives there
#define TXB0CTRL_ADR 0x30
#define TXB1CTRL_ADR 0x40
#define TXB2CTRL_ADR 0x50

#define TXBxCTRL_MASK_TXREQ 0x08
#define TXBxCTRL_MASK_TXP 0x03

#define TXB0ID_ADR 0x31
#define TXB1ID_ADR 0x41
#define TXB2ID_ADR 0x51

// bit masks for DLC registers
#define TXB_EXIDE_MASK 0x08U
#define DLC_MASK 0x0FU
#define RXBxDLC_MASK_RTR 0x40U
#define TXBxDLC_MASK_RTR 0x40U

// CANINTE
#define CANINTE_ADR 0X2BU

// CANINTF
#define CANINTF_ADR 0X2CU
#define CANINTF_MASK_RX0IF 0x01
#define CANINTF_MASK_RX1IF 0x02
#define CANINTF_MASK_TX0IF 0x04
#define CANINTF_MASK_TX1IF 0x08
#define CANINTF_MASK_TX2IF 0x10
#define CANINTF_MASK_ERRIF 0x20
#define CANINTF_MASK_WAKIF 0x40
#define CANINTF_MASK_MERRF 0x80
#define CANINTF_MASK_ALL 0xFF

// EFLG - error flags
#define EFLG_ADR 0X2D
#define EFLG_MASK_EWARN 0x01
#define EFLG_MASK_RXWAR 0x02
#define EFLG_MASK_TXWAR 0x04
#define EFLG_MASK_RXEP 0x08
#define EFLG_MASK_TXEP 0x10
#define EFLG_MASK_TXBO 0x20
#define EFLG_MASK_RX0OVR 0x40
#define EFLG_MASK_RX1OVR 0x80
#define EFLG_MASK_ALL 0xFF

// Commands we can send to the CAN chip
#define CAN_C_SPI_CMD_WRITE 2
#define CAN_C_SPI_CMD_READ 3
#define CAN_C_SPI_CMD_STATUS 0XA0
#define CAN_C_SPI_CMD_RXSTATUS 0XB0
#define CAN_C_SPI_CMD_RESET 0XC0
#define CAN_C_SPI_CMD_MODIFY 0X05

#define CAN_C_SPI_CMD_READ_RX0ID 0X90
#define CAN_C_SPI_CMD_READ_RX1ID 0X94

#define CAN_C_SPI_CMD_LOADTX_TX0 0X40
#define CAN_C_SPI_CMD_LOADTX_TX1 0X42
#define CAN_C_SPI_CMD_LOADTX_TX2 0X44
#define CAN_C_SPI_CMD_RTS_TX0 0X81
#define CAN_C_SPI_CMD_RTS_TX1 0X82
#define CAN_C_SPI_CMD_RTS_TX2 0X84

// Masks for the response to the RXSTATUS command
#define CAN_M_SPI_STATUS_TX2_IF 0B10000000
#define CAN_M_SPI_STATUS_TX2_BUSY 0B01000000
#define CAN_M_SPI_STATUS_TX1_IF 0B00100000
#define CAN_M_SPI_STATUS_TX1_BUSY 0B00010000
#define CAN_M_SPI_STATUS_TX0_IF 0B00001000
#define CAN_M_SPI_STATUS_TX0_BUSY 0B00000100
#define CAN_M_SPI_STATUS_RX1_READY 0B00000010
#define CAN_M_SPI_STATUS_RX0_READY 0B00000001

#define CAN_C_TXB0CTRL_ADR 0X30
#define CAN_C_TXB1CTRL_ADR 0X40
#define CAN_C_TXB2CTRL_ADR 0X50
#define CAN_C_RXB0CTRL_ADR 0X60
#define CAN_C_RXB1CTRL_ADR 0X70
#define CAN_M_RXBXCTRL_FILTER_OFF 0B01100000
#define CAN_M_RXBXCTRL_FILTER_ON 0
#define CAN_M_RXR0CTRL_ROLLOVER 0B00000100
#define CAN_C_RXB0M_ADR 0X20
#define CAN_C_RXB1M_ADR 0X24
#define CAN_C_RXB0F_ADR 0X00
#define CAN_C_RXB3F_ADR 0X10

#define CAN_C_CANINTE_ADR 0X2B
#define CAN_C_CANINTF_ADR 0X2C
#define CAN_M_CANINT_RX0 0B00000001
#define CAN_M_CANINT_RX1 0B00000010

// Structure that holds a message buffer as defined by the MC2515 spec
// These are the bytes that are read and written directly to the chip.
typedef struct
{
    uint8_t sidh;
    uint8_t sidl;
    uint8_t eid8;
    uint8_t eid0;
    uint8_t dlc;
    uint8_t data[ 8 ];
} MessageBuffer_t;

// bit masks for setting configuration registers
#define CNF1_MASK_SJW_1 0X00
#define CNF1_MASK_SJW_2 0X40
#define CNF1_MASK_SJW_3 0X80
#define CNF1_MASK_SJW_4 0XC0
#define CNF2_MASK_BTLMODE 0x80
#define CNF2_MASK_SAM3 0X40

// Structure that holds config info
#define MCP2515_NUM_MASKS 2
#define MCP2515_NUM_FILTERS 6

// which tx buffer will be used for the transmit queue
#define CAN_QUEUE_BUFFER 0

typedef struct
{
    uint32_t masks[ MCP2515_NUM_MASKS ];

    uint32_t filters[ MCP2515_NUM_FILTERS ];

} MCP2515_Config_t;

typedef struct
{
    // received incoming packet but unable to save it to the queue
    // usually an indication that the application is not processing
    // messages fast enough
    unsigned rxQueueOverflows;

    // outgoing RAM queue overflowed.  Application is sending messages
    // faster than they can be sent on the bus.  If this happens
    // occasionally it may be an indication that the queue size should
    // be increased to handle peak outgoing message counts
    unsigned txQueueOverflows;

    // number of on-chip rx buffer overflows
    // this is usually an indication that we are not reading
    // the incoming data fast enough
    unsigned rxBufferOverflow;

} MCP2515_Diagnostics_t;

// diagnostic counters
extern MCP2515_Diagnostics_t MCP2515_diag;

#endif
