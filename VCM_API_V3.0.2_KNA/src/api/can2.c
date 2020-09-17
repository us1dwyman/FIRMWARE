/**
 * Driver for CAN2 port using MCP2515 interface via SPI
 *
 * @link http://ww1.microchip.com/downloads/en/DeviceDoc/MCP2515-Stand-Alone-CAN-Controller-with-SPI-20001801J.pdf
 *
 * Copyright (c) 2019 Control Solutions LLC. All Rights Reserved.
 */
#include "app.h"

#if defined( CAN2_ENABLE )

#include "can.h"
#include "cbuf.h"
#include "drv_spi.h"
#include "hardware.h"
#include "mcp2515_can.h"
#include "timer.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <xc.h>

//------------------------------//----------------------------------------------
// SPI interface functions
#define SPI_Put( c ) DRV_SPI_Put( SPI_CHANNEL_CAN2, c )
#define SPI_PutBuffer( pBuf, count ) DRV_SPI_PutBuffer( SPI_CHANNEL_CAN2, pBuf, count )
#define SPI_Get() DRV_SPI_Get( SPI_CHANNEL_CAN2 )
#define SPI_GetBuffer( pBuf, count ) DRV_SPI_GetBuffer( SPI_CHANNEL_CAN2, pBuf, count )
#define SPI_Lock()                                                                                                     \
    while( !DRV_SPI_Lock( SPI_CHANNEL_CAN2 ) )                                                                         \
    {                                                                                                                  \
    }                                                                                                                  \
    DRV_SPI_Initialize( &_spiConfig )
#define SPI_Unlock() DRV_SPI_Unlock( SPI_CHANNEL_CAN2 )

//------------------------------//----------------------------------------------
// Definitions

// Work out the values of configuration registers CNF1, CNF2, CNF3
// based on the #define CAN2_BITRATE (=125, 250, 500, 1000)

// sync is always 1
#define CFG_SYNC 1

#if( CAN2_BITRATE == 125 )

// Note the segment values loaded into cfg registers are 1 less than
// calculated according to the equations in the datasheet
#define CFG_PROP ( 6 - 1 )
#define CFG_SEG1 ( 5 - 1 )
#define CFG_SEG2 ( 4 - 1 )
#define CFG_SJW CNF1_MASK_SJW_1
#define CFG_BRP 4

#elif( CAN2_BITRATE == 250 )

// Note the segment values loaded into cfg registers are 1 less than
// calculated according to the equations in the datasheet
#define CFG_PROP ( 1 - 1 )
#define CFG_SEG1 ( 5 - 1 )
#define CFG_SEG2 ( 3 - 1 )
#define CFG_SJW CNF1_MASK_SJW_1
#define CFG_BRP 3

#elif( CAN2_BITRATE == 500 )

// Note the segment values loaded into cfg registers are 1 less than
// calculated according to the equations in the datasheet
#define CFG_PROP ( 1 - 1 )
#define CFG_SEG1 ( 5 - 1 )
#define CFG_SEG2 ( 3 - 1 )
#define CFG_SJW CNF1_MASK_SJW_1
#define CFG_BRP 1

#elif( CAN2_BITRATE == 1000 )

// Note the segment values loaded into cfg registers are 1 less than
// calculated according to the equations in the datasheet
#define CFG_PROP ( 1 - 1 )
#define CFG_SEG1 ( 5 - 1 )
#define CFG_SEG2 ( 3 - 1 )
#define CFG_SJW CNF1_MASK_SJW_1
#define CFG_BRP 0

#else
#error unknown CAN2_BITRATE
#endif

#if( CFG_BRP < 0 || CFG_BRP > 63 )
#error CAN BRP is out of range
#endif

#if( CFG_SYNC + CFG_PROP + CFG_SEG1 + CFG_SEG2 ) < 5 || ( CFG_SYNC + CFG_PROP + CFG_SEG1 + CFG_SEG2 ) > 25
#error Total CAN TQ is out of range
#endif

// Values to be programmed to the chip - format as required for registers
#define CNF1_VALUE ( CFG_SJW | ( CFG_BRP ) )
#define CNF2_VALUE ( CNF2_MASK_BTLMODE | CNF2_MASK_SAM3 | ( CFG_SEG1 << 3 ) | CFG_PROP )
#define CNF3_VALUE ( CFG_SEG2 )

// Define a circular buffer to hold outgoing messages (compatible
// with the CBUF macros)
typedef struct
{
    uint8_t m_getIdx;
    uint8_t m_putIdx;
    MessageBuffer_t m_entry[ CAN2_txMessageQ_SIZE ];
} CAN2_txMessageQ_t;

// Define a circular buffer to hold incoming messages (compatible
// with the CBUF macros)
typedef struct
{
    uint8_t m_getIdx;
    uint8_t m_putIdx;
    MessageBuffer_t m_entry[ CAN2_rxMessageQ_SIZE ];
} CAN2_rxMessageQ_t;

//------------------------------//----------------------------------------------
// Local variables
//

// Holds the SPI driver configuration used to communicate with the device
static const DRV_SPI_INIT_DATA _spiConfig = {
    SPI_CHANNEL_CAN2,       // channel
    0b10,                   // 4:1 primaryPrescale
    0b110,                  // 2:1 secondaryPrescale
    SPI_CKE_ACT_IDLE,       // cke
    SPI_BUS_MODE_0,         // spi mode
    SPI_TRANSFER_MODE_8BIT  // transfer mode
};

// A circular buffer of pointers to outgoing messages
static CAN2_txMessageQ_t CAN2_txMessageQ;

// A circular buffer of pointers to incoming messages
static CAN2_rxMessageQ_t CAN2_rxMessageQ;

// the operational mode of the chip
static CANCTRL_REQOP_MODE _canMode;

//------------------------------//----------------------------------------------
// Global Variables

// diagnostic counters
MCP2515_Diagnostics_t MCP2515_diag;

//------------------------------//----------------------------------------------
// Local Functions

/**
 * @brief      Formats a chip message buffer to a can_t message
 *
 * @param      p     pointer to buffer holding the message
 * @param      pMsg  pointer to memory that will hold the resulting message
 */
static void _RxBufferToMessage( MessageBuffer_t *p, can_t *pMsg )
{
    uint32_t id = ( (uint32_t)p->sidh << 3 ) + ( p->sidl >> 5 );

    if( ( p->sidl & TXB_EXIDE_MASK ) == TXB_EXIDE_MASK )
    {
        id = ( id << 2 ) + ( p->sidl & 0x03 );
        id = ( id << 8 ) + p->eid8;
        id = ( id << 8 ) + p->eid0;

        id |= CAN_EFF_FLAG;
    }

    uint8_t dlc = p->dlc & DLC_MASK;
    if( dlc > 8 )
    {
        // hard to imagine how this would happen, but protect from buffer overrun
        dlc = 8;
    }

    // Check for RTR frame
    if( p->dlc & RXBxDLC_MASK_RTR )
    {
        id |= CAN_RTR_FLAG;
    }

    pMsg->id = id;
    pMsg->buf_len = dlc;
    memcpy( pMsg->buf, p->data, dlc );
}

/**
 * @brief      Formats a can_t message to fit in the chip's buffer format
 *
 * @param      pFrame  pointer to CAN frame to be put in the buffer
 * @param      pBuf    pointer to buffer which will store the frame
 */
static void _MessageToTxBuffer( can_t *pFrame, MessageBuffer_t *pBuf )
{
    bool ext = ( pFrame->id & CAN_EFF_FLAG ) > 0;
    bool rtr = ( pFrame->id & CAN_RTR_FLAG ) > 0;

    uint32_t id = ( pFrame->id & ( ext ? CAN_EFF_MASK : CAN_SFF_MASK ) );
    uint16_t canid = ( uint16_t )( id & 0x0FFFF );

    if( ext )
    {
        pBuf->eid0 = ( uint8_t )( canid & 0xFF );
        pBuf->eid8 = ( uint8_t )( canid >> 8 );
        canid = ( uint16_t )( id >> 16 );
        pBuf->sidl = ( uint8_t )( canid & 0x03 );
        pBuf->sidl += ( uint8_t )( ( canid & 0x1C ) << 3 );
        pBuf->sidl |= TXB_EXIDE_MASK;
        pBuf->sidh = ( uint8_t )( canid >> 5 );
    }
    else
    {
        pBuf->sidh = ( uint8_t )( canid >> 3 );
        pBuf->sidl = ( uint8_t )( ( canid & 0x07 ) << 5 );
        pBuf->eid0 = 0;
        pBuf->eid8 = 0;
    }

    // pFrame->id = id;
    pBuf->dlc = rtr ? ( pFrame->buf_len | TXBxDLC_MASK_RTR ) : pFrame->buf_len;
    memcpy( pBuf->data, pFrame->buf, pFrame->buf_len );
}

/**
 * @brief      Read one or more registers from the device
 *
 * @param[in]  reg    The starting register to read
 * @param      pBuf   pointer to buffer to hold the result
 * @param[in]  count  number of bytes to read
 */
static void _Read( uint8_t reg, uint8_t *pBuf, uint8_t count )
{
    CAN2_CSON();

    SPI_Put( CAN_C_SPI_CMD_READ );
    SPI_Put( reg );
    SPI_GetBuffer( pBuf, count );

    CAN2_CSOFF();
}

/**
 * Write to one or more registers in the device
 *
 * @param reg the starting register to write
 * @param pBuf pointer to buffer of bytes to write
 * @param count number of bytes to write
 */
static void _Write( uint8_t reg, uint8_t *pBuf, uint8_t count )
{
    CAN2_CSON();

    SPI_Put( CAN_C_SPI_CMD_WRITE );
    SPI_Put( reg );
    SPI_PutBuffer( pBuf, count );

    CAN2_CSOFF();
}

static void _Modify( uint8_t reg, uint8_t mask, uint8_t value )
{
    CAN2_CSON();

    SPI_Put( CAN_C_SPI_CMD_MODIFY );
    SPI_Put( reg );
    SPI_Put( mask );
    SPI_Put( value );

    CAN2_CSOFF();
}

static void _TxCommand( uint8_t cmd, uint8_t *pBuf, uint8_t count )
{
    CAN2_CSON();

    SPI_Put( cmd );
    SPI_PutBuffer( pBuf, count );

    CAN2_CSOFF();
}

/**
 * @brief      Read values using a command
 *
 * @param[in]  cmd    The command to read
 * @param      pBuf   pointer to buffer to hold the result
 * @param[in]  count  number of bytes to read
 */
static void _RxCommand( uint8_t cmd, void *p, uint8_t count )
{
    uint8_t *pBuf = (uint8_t *)p;

    CAN2_CSON();

    SPI_Put( cmd );
    SPI_GetBuffer( pBuf, count );

    CAN2_CSOFF();
}

/**
 * @brief      Copies the message into the designated transmit buffer and enables send
 *
 *             Note: does not check whether the buffer is available; that
 *             should be done by the caller.
 *
 * @param[in]  addr  The register address of the tx buffer to use
 * @param      pMsg  The message
 */
static void _SendBuffer( uint8_t addr, MessageBuffer_t *pBuf )
{
    _Write( addr + 1, (uint8_t *)pBuf, sizeof( MessageBuffer_t ) );
    _Modify( addr, TXBxCTRL_MASK_TXREQ, TXBxCTRL_MASK_TXREQ );
}

/**
 * @brief      Converts transmit frame and pushes onto queue
 *
 * Note: does not error check anything - caller's responsibility
 *
 * @param      pFrame  The frame
 */
static void _EnqueueTxMessage( can_t *pFrame )
{
    MessageBuffer_t *pBuf = CBUF_GetPushEntryPtr( CAN2_txMessageQ );

    _MessageToTxBuffer( pFrame, pBuf );

    CBUF_AdvancePushIdx( CAN2_txMessageQ );
}

/**
 * @brief      Set mode of the CAN controller
 *
 * @param[in]  mode  The desired mode
 *
 * @return     true if successful
 */
static bool _SetMode( CANCTRL_REQOP_MODE mode )
{
    bool modeMatch = false;
    MS_TICK timer;

    _Modify( CANCTRL_ADR, CANCTRL_MASK_REQOP, mode );
    timer = TIMER_Now();

    // this normally takes <1ms, so give a generous timeout
    while( TIMER_Since( timer ) < 5 )
    {
        _Read( CANSTAT_ADR, (uint8_t *)&_canMode, 1 );
        _canMode &= CANSTAT_MASK_REQOP;

        modeMatch = ( _canMode == mode );

        if( modeMatch )
        {
            break;
        }
    }

    return modeMatch;
}

//------------------------------//----------------------------------------------
// Public Functions

/**
 * @brief      Initialize the CAN hardare
 *
 * @param[in]  pConfig  The configuration
 *
 * @return     True if able to 2 initialize, False otherwise.
 */
bool CAN2_Initialize( const void *pConfig )
{
    bool bResult = true;
    uint8_t temp8;
    MCP2515_Config_t *p = (MCP2515_Config_t *)pConfig;

    CBUF_Init( CAN2_rxMessageQ );
    CBUF_Init( CAN2_txMessageQ );

    // clear diagnostic counters
    memset( &MCP2515_diag, 0, sizeof( MCP2515_Diagnostics_t ) );

    // Standby pin disabled
    CAN2_CSOFF();
    OPIN_CAN2_STBY_OFF();

    // Set up SPI
    while( !DRV_SPI_Lock( SPI_CHANNEL_CAN2 ) )
    {
    }
    DRV_SPI_Initialize( &_spiConfig );

    // RESET
    _TxCommand( CAN_C_SPI_CMD_RESET, NULL, 0 );
    // need 128 clock cycles (at 20Mhz) minimum after reset
    // = ~6.4us.  Here we wait 1-2ms, which is plenty long
    MS_TICK wait = TIMER_Now();
    while( TIMER_Since( wait ) < 2 )
        ;

    // was this (hw rev A):
    //     clear all canctrl bits except mode (CLKEN, CLKPRE, OSM, ABAT)
    //     This would turn off CLKOUT (reset value was CLKOUT enabled) 
    //     and change the freq from power on of SYSCLK/8 (reset value) to SYSCLK/1.
    // So change to (hw rev B):
    //     clear all canctrl bits excepting mode, CLKEN and CLKPRE - so clear 
    //     (OSM, ABAT)
    // Reason:
    //     this allows the CAN device to keep outputting the power on default of 
    //     20MHz/8 on its CLKOUT pin, without changing frequency, which inturn allows the
    //     dspic clock subsystem to be configured once and done.
    _Modify( CANCTRL_ADR, (uint8_t)~( CANCTRL_MASK_REQOP | CANCTRL_MASK_CLKEN | CANCTRL_MASK_CLKPRE ), 0 );

    // Make sure we are in config mode
    bResult = bResult & _SetMode( CANCTRL_REQOP_CONFIG );

    uint8_t buf[ 5 ];
    buf[ 0 ] = CNF3_VALUE;
    buf[ 1 ] = CNF2_VALUE;
    buf[ 2 ] = CNF1_VALUE;

    _Write( CNF3_ADR, buf, 3 );

    // enable rx filtering
    temp8 = CAN_M_RXBXCTRL_FILTER_ON;
    _Write( CAN_C_RXB1CTRL_ADR, &temp8, 1 );

    // also enable rollover for RB0
    temp8 |= CAN_M_RXR0CTRL_ROLLOVER;
    _Write( CAN_C_RXB0CTRL_ADR, &temp8, 1 );

    // Set the filter masks
    _Write( CAN_C_RXB0M_ADR, (uint8_t *)p->masks, MCP2515_NUM_MASKS * sizeof( uint32_t ) );

    // Set the filters
    _Write( CAN_C_RXB0F_ADR, (uint8_t *)p->filters, 12 );
    _Write( CAN_C_RXB3F_ADR, (uint8_t *)p->filters + 12, 12 );

    // Clear interrupt flags
    temp8 = 0;
    _Modify( CANINTF_ADR, 0xFF, 0 );

    // Set interrupt enables
    temp8 = CANINTF_MASK_RX0IF | CANINTF_MASK_RX1IF | CANINTF_MASK_TX0IF;  //| CANINTF_MASK_TX1IF | CANINTF_MASK_TX2IF;
    temp8 |= CANINTF_MASK_ERRIF;

    _Write( CANINTE_ADR, &temp8, 1 );

    // Set mode to normal
    bResult = bResult & _SetMode( CANCTRL_REQOP_NORMAL );

    // release SPI hardware
    DRV_SPI_Unlock( SPI_CHANNEL_CAN2 );

    return bResult;
};

/**
 * @brief      Put CAN in low power mode
 */
void CAN2_Sleep( void )
{
    // set STBY pin to active
    OPIN_CAN2_STBY_ON();

    // send sleep command (removed for rev B hardware - sleep shuts down
    // CLKOUT to dsPIC, impeding loading which starts with a reset command
    // over CAN)
    //_SetMode( CANCTRL_REQOP_SLEEP );
}

/**
 * @brief      Send a CAN message
 *
 * If memory is available inside of the transmit buffer, can_tx copies the memory pointed to by frame to the
 * transmit buffer. If transmission of CAN frames is not currently in progress, then it will be initiated. If the
 * most significant bit of frame->id (i.e. bit 31) is set, it indicates an extended CAN frame, else it indicates a
 * standard CAN frame.
 *
 * @param      ptr   Points to the CAN frame that should be copied to the transmit buffer.
 *
 * @return     0: The CAN frame was successfully written to the transmit buffer. 1: no CAN frame written to buffer
 */
uint8_t CAN2_Tx( can_t *pFrame )
{
    // if the queue is empty, try to send the message immediately.
    if( CBUF_IsEmpty( CAN2_txMessageQ ) )
    {
        if( CAN2_TxMailbox( pFrame, CAN_QUEUE_BUFFER ) )
        {
            // unable to send, but we can buffer it
            _EnqueueTxMessage( pFrame );
        }
    }
    else if( !CBUF_IsFull( CAN2_txMessageQ ) )
    {
        // there are already messages waiting but the queue is not full
        _EnqueueTxMessage( pFrame );
    }
    else
    {
        // error - can't send and no room in queue
        MCP2515_diag.txQueueOverflows++;
        return 1;
    }

    // success! (either sent or queued)
    return 0;
}

uint8_t CAN2_TxMailbox( can_t *pFrame, unsigned which )
{
    uint8_t txBufCtrlAddress;

    switch( which )
    {
        case 0:
            txBufCtrlAddress = TXB0CTRL_ADR;
            break;

        case 1:
            txBufCtrlAddress = TXB1CTRL_ADR;
            break;

        case 2:
            txBufCtrlAddress = TXB2CTRL_ADR;
            break;

        default:
            // invalid
            return 1;
    }

    SPI_Lock();

    uint8_t bufStatus;
    uint8_t result = 1;

    // read the status of the selected buffer
    _Read( txBufCtrlAddress, &bufStatus, 1 );

    if( 0 == ( bufStatus & TXBxCTRL_MASK_TXREQ ) )
    {
        // buffer is available; send
        MessageBuffer_t buf;
        _MessageToTxBuffer( pFrame, &buf );
        _SendBuffer( txBufCtrlAddress, &buf );

        result = 0;
    }

    SPI_Unlock();

    return result;
}

/**
 *
 * @brief      Checks for and returns an incoming CAN frame
 *
 * checks to see if there is a CAN data frame available in the receive buffer. If one is available, it is copied
 * into the can_t structure which is pointed to by frame. If the most significant bit of frame->id (i.e. bit 31) is
 * set, it indicates an extended CAN frame, else it indicates a standard CAN frame.
 *
 * @param      ptr   Points to memory where the received CAN frame should be stored.
 *
 * @return     0: A CAN frame was successfully read from the receive buffer. 1=No CAN frame
 */
uint8_t CAN2_Rx( can_t *ptr )
{
    if( !CBUF_IsEmpty( CAN2_rxMessageQ ) )
    {
        MessageBuffer_t *pBuf = CBUF_GetPopEntryPtr( CAN2_rxMessageQ );

        _RxBufferToMessage( pBuf, ptr );

        CBUF_AdvancePopIdx( CAN2_rxMessageQ );
        return 0;
    }

    return 1;
}

/**
 * @brief      Checks the chip's status and handles incoming and outgoing messages
 *
 * This should be called when the chip signals an 'interrupt' condition.
 * Depending on processor setup, this may be a processor interrupt as a result of
 * a input pin, or detected by polling an input pin.
 */
void CAN2_ISR( void )
{
    if( CANCTRL_REQOP_NORMAL == _canMode )
    {
        SPI_Lock();

        uint8_t _intFlags, _intFlags2;

        _RxCommand( CAN_C_SPI_CMD_STATUS, &_intFlags, 1 );

        // get interrupt flags, since the ERRIF flag is stored there
        _Read( CANINTF_ADR, &_intFlags2, 1 );

        if( _intFlags2 & CANINTF_MASK_ERRIF )
        {
            uint8_t _errFlags;
            _Read( EFLG_ADR, &_errFlags, 1 );

            if( _errFlags & EFLG_MASK_RX0OVR )
            {
                MCP2515_diag.rxBufferOverflow++;
            }

            if( _errFlags & EFLG_MASK_RX1OVR )
            {
                MCP2515_diag.rxBufferOverflow++;
            }

            // clear all error flags
            _Modify( EFLG_ADR, EFLG_MASK_RX0OVR | EFLG_MASK_RX1OVR, 0 );
        }

        if( _intFlags & CAN_M_SPI_STATUS_RX0_READY )
        {
            if( !CBUF_IsFull( CAN2_rxMessageQ ) )
            {
                MessageBuffer_t *pBuf = CBUF_GetPushEntryPtr( CAN2_rxMessageQ );
                _Read( RXB0ID_ADR, (uint8_t *)pBuf, sizeof( MessageBuffer_t ) );
                CBUF_AdvancePushIdx( CAN2_rxMessageQ );
            }
            else
                MCP2515_diag.rxQueueOverflows++;
        }

        if( _intFlags & CAN_M_SPI_STATUS_RX1_READY )
        {
            if( !CBUF_IsFull( CAN2_rxMessageQ ) )
            {
                MessageBuffer_t *pBuf = CBUF_GetPushEntryPtr( CAN2_rxMessageQ );
                _Read( RXB1ID_ADR, (uint8_t *)pBuf, sizeof( MessageBuffer_t ) );
                CBUF_AdvancePushIdx( CAN2_rxMessageQ );
            }
            else
                MCP2515_diag.rxQueueOverflows++;
        }

        if( 0 == ( _intFlags & CAN_M_SPI_STATUS_TX0_BUSY ) )
        {
            if( !CBUF_IsEmpty( CAN2_txMessageQ ) )
            {
                MessageBuffer_t *pBuf = CBUF_GetPopEntryPtr( CAN2_txMessageQ );
                _SendBuffer( TXB0CTRL_ADR, pBuf );
                CBUF_AdvancePopIdx( CAN2_txMessageQ );
            }
        }

        //  We don't make use of TX1 and TX2 currently.  This is because
        //  we can't necessarily enforce the order of packets that are transmitted
        //  and we want to ensure FIFO handling of the tx queue.

        //
        //    if( 0 == ( _intFlags & CAN_M_SPI_STATUS_TX1_BUSY ) )
        //    {
        //        if( !CBUF_IsEmpty( CAN2_txMessageQ ) )
        //        {
        //            MessageBuffer_t *pBuf = CBUF_GetPopEntryPtr( CAN2_txMessageQ );
        //            _Write( TXB1ID_ADR, (uint8_t *)pBuf, sizeof( MessageBuffer_t ) );
        //            _Modify( TXB1CTRL_ADR, TXBxCTRL_MASK_TXREQ, TXBxCTRL_MASK_TXREQ );
        //            CBUF_AdvancePopIdx( CAN2_txMessageQ );
        //        }
        //    }
        //
        //    if( 0 == ( _intFlags & CAN_M_SPI_STATUS_TX2_BUSY ) )
        //    {
        //        if( !CBUF_IsEmpty( CAN2_txMessageQ ) )
        //        {
        //            MessageBuffer_t *pBuf = CBUF_GetPopEntryPtr( CAN2_txMessageQ );
        //            _Write( TXB2ID_ADR, (uint8_t *)pBuf, sizeof( MessageBuffer_t ) );
        //            _Modify( TXB2CTRL_ADR, TXBxCTRL_MASK_TXREQ, TXBxCTRL_MASK_TXREQ );
        //            CBUF_AdvancePopIdx( CAN2_txMessageQ );
        //        }
        //    }

        // reset the interrupt flags on the chip
        // this should make the interrupt line de-assert
        _Modify( CANINTF_ADR, 0xFF, 0 );

        SPI_Unlock();
    }
}

/**
 * @brief      Returns true if the bus is in an error state
 *
 * @return     True if transceiver is in bus off state
 */
bool CAN2_isBusOff( void )
{
    uint8_t _errFlags;

    SPI_Lock();

    _Read( EFLG_ADR, &_errFlags, 1 );

    SPI_Unlock();

    return ( _errFlags & EFLG_MASK_TXBO ) ? true : false;
}

/**
 * @brief      Task loop
 *
 * Checks for incoming or outgoing messages that need to be processed
 */
void CAN2_Tasks( void )
{
    if( IPIN_CAN2_INT() )
    {
        CAN2_ISR();
    }
}

#endif
