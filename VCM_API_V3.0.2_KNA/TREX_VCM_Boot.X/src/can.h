/**
 * Defines types and structures used for processing CANBUS messages
 *
 * These definitions are intended to be relatively generic and portable.
 * The implementation (.c) file should adapt to the specific hardware
 * or lower-level drivers.
 *
 * Typically the driver constraints (number of buffers, etc) are defined
 * in the system_config.h file.
 *
 * Before sending or attempting to receive any CAN messages, call
 * CAN_Initialize().  Depending on the driver, you may need to pass in
 * a pointer to an initialization structure.  If not, pass in NULL.
 *
 * To send messages, use either the CAN_Tx() function, which uses the
 * next available transmit buffer, or the CAN_TxMailbox(), which attempts to put
 * the message in a specific mailbox.  The number of mailboxes and/or number of buffers
 * available is implementation-specific.
 *
 * To receive messages, use either the CAN_Rx() function, which reads the
 * next message from the incoming FIFO, or CAN_RxMailbox(), which checks a
 * specific mailbox.  The number of mailboxes and/or number of buffers available
 * is implementation-specific.
 *
 * The driver may or may not use interrupts.  Because of this, you should
 * call CAN_Tasks() periodically to allow any housekeeping tasks to occur.
 * Also, you will want to check for incoming messages frequently enough that
 * incoming buffers do not overflow (the interval required depends on not only
 * the implementation of the driver, but also the characteristics of the CANBUS
 * message traffic).
 *
 * Copyright (c) 2018 Control Solutions LLC.  All Rights Reserved.
 */
#ifndef CAN_H
#define CAN_H

#include "app.h"
#include <stdint.h>

//------------------------------//----------------------------------------------
// Definitions

/*
 * Controller Area Network Identifier structure
 *
 * bit 0-28	: CAN identifier (11/29 bit)
 * bit 29	: error message frame flag (0 = data frame, 1 = error message)
 * bit 30	: remote transmission request flag (1 = rtr frame)
 * bit 31	: frame format flag (0 = standard 11 bit, 1 = extended 29 bit)
 */
typedef uint32_t canid_t;

/* special address description flags for the CAN_ID */
#define CAN_EFF_FLAG 0x80000000U /* EFF/SFF is set in the MSB */
#define CAN_RTR_FLAG 0x40000000U /* remote transmission request */
#define CAN_ERR_FLAG 0x20000000U /* error message frame */

/* valid bits in CAN ID for frame formats */
#define CAN_SFF_MASK 0x000007FFU /* standard frame format (SFF) */
#define CAN_EFF_MASK 0x1FFFFFFFU /* extended frame format (EFF) */
#define CAN_ERR_MASK 0x1FFFFFFFU /* omit EFF, RTR, ERR flags */

/**
 * can_t is a data type used to store CAN frames. It contains the CAN frame identifier, the CAN frame data, and the size
 * of data. The three MSBs indicate the type of frame (see CAN_xxx_FLAG definitions)
 * indicates a standard CAN frame.
 */
typedef struct
{
    // Id of the message
    canid_t id;

    // extra bytes to make the structure size a multiple of 2 bytes
    // may improve efficiency with certain processors/compilers
    uint8_t pad1;

    // number of bytes in the buf
    uint8_t buf_len;

    uint8_t buf[ 8 ];
} can_t;

//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Public functions

/**
 * @brief      Initialize the CAN driver
 *
 * initializes the CAN peripheral for reception and transmission of CAN frames.
 *
 * @param[in]  pConfig    pointer to driver-specific configuration info.  NULL if none
 */
void CAN_Initialize( const void *pConfig );

/**
 * @brief      Sets a receive filter mask
 *
 * @param[in]  m           index of the mask (limit depends on hardware capability)
 * @param[in]  mask  	   bit mask (CAN_EFF_FLAG) if extended id
 * @param[in]  mide        0 -> Match either standard or extended id message if filters match
 * 						   1 -> Match only message type indicated in the mask
 * @return 0 on success, 1 on failure (eg invalid mask reference)
 */
uint8_t CAN_SetRxMask( int m, canid_t id, unsigned int mide );

/**
 * @brief      Configures an acceptance filter for incoming messages
 *
 * If an incoming message matches the filter/mask pair, it will
 * be stored in the indicated mailbox or fifo.
 *
 * @param[in]  n           Index of the filter (limit depends on hardware capability)
 * @param[in]  identifier  The identifier to match. Set CAN_EFF_FLAG if an extended id
 * @param[in]  bufPnt      The buffer used to store the message
 * @param[in]  maskSel     Index to the mask to use
 *
 * @return 0 on success, 1 on failure (eg invalid buffer or mask reference)
 */
uint8_t CAN_SetRxFilter( int n, canid_t id, unsigned int bufPnt, unsigned int maskSel );

/**
 * @brief      Task loop.
 *
 * Call each time through the main loop to maintain CAN message transfers
 */
void CAN_Tasks( void );

/**
 * @brief      Put CAN in low power mode
 */
void CAN_Sleep( void );

/**
 * @brief      Checks for and returns an incoming CAN frame
 *
 * checks to see if there is a CAN data frame available in the receive buffer. If one is available, it is copied into
 * the can_t structure which is pointed to by frame. If the most significant bit of frame->id (i.e. bit 31) is set, it
 * indicates an extended CAN frame, else it indicates a standard CAN frame.
 *
 * @param      pFrame   Points to memory where the received CAN frame should be stored.
 *
 * @return     0: A CAN frame was successfully read from the receive buffer. 1=No CAN frame
 */
uint8_t CAN_Rx( can_t *pFrame );
uint8_t CAN_RxMailbox( can_t *pFrame, unsigned which );

/**
 * @brief      Determines ability to transmit.
 *
 * If memory is available inside of the transmit buffer, can_tx copies the memory pointed to by frame to the transmit
 * buffer. If transmission of CAN frames is not currently in progress, then it will be initiated. If the most
 * significant bit of frame->id (i.e. bit 31) is set, it indicates an extended CAN frame, else it indicates a standard
 * CAN frame.
 *
 * @param      pFrame   Points to the CAN frame that should be copied to the transmit buffer.
 *
 * @return     0: The CAN frame was successfully written to the transmit buffer. 1: no CAN frame written to buffer
 */
uint8_t CAN_Tx( can_t *pFrame );
uint8_t CAN_TxMailbox( can_t *pFrame, unsigned which );

#if defined( CAN2_ENABLE )

/**
 * @brief      Initialize the CAN driver
 *
 * initializes the CAN peripheral for reception and transmission of CAN frames.
 *
 * @param[in]  pConfig    pointer to driver-specific configuration info.  NULL if none
 */
void CAN2_Initialize( const void *pConfig );

/**
 * @brief      Sets a receive filter mask
 *
 * @param[in]  m           index of the mask (limit depends on hardware capability)
 * @param[in]  mask  	   bit mask (CAN_EFF_FLAG) if extended id
 * @param[in]  mide        0 -> Match either standard or extended id message if filters match
 * 						   1 -> Match only message type indicated in the mask
 * @return 0 on success, 1 on failure (eg invalid mask reference)
 */
uint8_t CAN2_SetRxMask( int m, canid_t id, unsigned int mide );

/**
 * @brief      Configures an acceptance filter for incoming messages
 *
 * If an incoming message matches the filter/mask pair, it will
 * be stored in the indicated mailbox or fifo.
 *
 * @param[in]  n           Index of the filter (limit depends on hardware capability)
 * @param[in]  identifier  The identifier to match. Set CAN_EFF_FLAG if an extended id
 * @param[in]  bufPnt      The buffer used to store the message
 * @param[in]  maskSel     Index to the mask to use
 *
 * @return 0 on success, 1 on failure (eg invalid buffer or mask reference)
 */
uint8_t CAN2_SetRxFilter( int n, canid_t id, unsigned int bufPnt, unsigned int maskSel );

/**
 * @brief      Task loop.
 *
 * Call each time through the main loop to maintain CAN message transfers
 */
void CAN2_Tasks( void );

/**
 * @brief      Put CAN in low power mode
 */
void CAN2_Sleep( void );

/**
 * @brief      Checks for and returns an incoming CAN frame
 *
 * checks to see if there is a CAN data frame available in the receive buffer. If one is available, it is copied into
 * the can_t structure which is pointed to by frame. If the most significant bit of frame->id (i.e. bit 31) is set, it
 * indicates an extended CAN frame, else it indicates a standard CAN frame.
 *
 * @param      pFrame   Points to memory where the received CAN frame should be stored.
 *
 * @return     0: A CAN frame was successfully read from the receive buffer. 1=No CAN frame
 */
uint8_t CAN2_Rx( can_t *pFrame );
uint8_t CAN2_RxMailbox( can_t *pFrame, unsigned which );

/**
 * @brief      Determines ability to transmit.
 *
 * If memory is available inside of the transmit buffer, can_tx copies the memory pointed to by frame to the transmit
 * buffer. If transmission of CAN frames is not currently in progress, then it will be initiated. If the most
 * significant bit of frame->id (i.e. bit 31) is set, it indicates an extended CAN frame, else it indicates a standard
 * CAN frame.
 *
 * @param      pFrame   Points to the CAN frame that should be copied to the transmit buffer.
 *
 * @return     0: The CAN frame was successfully written to the transmit buffer. 1: no CAN frame written to buffer
 */
uint8_t CAN2_Tx( can_t *pFrame );
uint8_t CAN2_TxMailbox( can_t *pFrame, unsigned which );

#endif

#endif
