/**
 * CAN Driver for dsPIC33E and PIC24E
 *
 * Implements the CAN interface defined in can.h
 *
 * Reference: http://ww1.microchip.com/downloads/en/DeviceDoc/70353C.pdf
 *
 * This driver relies on configuration in system_config.h and app.h
 * to define its behavior.
 *
 * Copyright (c) 2019 Control Solutions LLC.  All Rights Reserved.
 */

#include "can.h"
#include "dma.h"
#include "system_config.h"
#include <string.h>

//------------------------------//----------------------------------------------
// Definitions

// Access for the transmit control SFRs
typedef struct __attribute__( ( packed ) )
{
    unsigned priority : 2;
    unsigned remote_transmit_enable : 1;
    unsigned send_request : 1;
    unsigned error : 1;
    unsigned lost_arbitration : 1;
    unsigned message_aborted : 1;
    unsigned transmit_enabled : 1;
} CAN1_TX_CONTROLS;

// Operation modes per chip data sheet
typedef enum
{
    CAN_NORMAL_OPERATION_MODE = 0,
    CAN_DISABLE_MODE = 1,
    CAN_LOOPBACK_MODE = 2,
    CAN_LISTEN_ONLY_MODE = 3,
    CAN_CONFIGURATION_MODE = 4,
    CAN_LISTEN_ALL_MESSAGES_MODE = 7
} CAN_OP_MODES;

typedef enum
{
    CAN_PRIORITY_HIGH = 0b11,
    CAN_PRIORITY_MEDIUM = 0b10,
    CAN_PRIORITY_LOW = 0b01,
    CAN_PRIORITY_NONE = 0b00
} CAN_TX_PRIOIRTY;

//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Local variables

/*
 * Buffer that holds all CAN messages (incoming and outgoing)
 * This alignment is required because of the DMA's peripheral indirect
 * addressing mode. */
static unsigned int can1msgBuf[ CAN1_MESSAGE_BUFFERS ][ 8 ] __attribute__( ( aligned( 32 * 8 * 2 ) ) );

//------------------------------//----------------------------------------------
// Local functions

/**
 * @brief      Copy message from DMA buffer to user RAM
 *
 * @param[in]  buffer_number  The DMA buffer number
 * @param      pMessage        pointer to buffer to receive message
 */
static void _DMACopy( uint8_t buffer_number, can_t *pMessage )
{
    uint16_t ide = 0;
    uint16_t rtr = 0;

    /* read word 0 to see the message type */
    ide = can1msgBuf[ buffer_number ][ 0 ] & 0x0001U;

    /* check to see what type of message it is */
    /* message is standard identifier */
    if( ide == 0U )
    {
        pMessage->id = ( can1msgBuf[ buffer_number ][ 0 ] & 0x1FFCU ) >> 2U;
        rtr = can1msgBuf[ buffer_number ][ 0 ] & 0x0002U;
    }
    /* message is extended identifier */
    else
    {
        uint32_t id;
        id = can1msgBuf[ buffer_number ][ 0 ] & 0x1FFCU;
        pMessage->id = id << 16U;
        pMessage->id += ( ( (uint32_t)can1msgBuf[ buffer_number ][ 1 ] & (uint32_t)0x0FFF ) << 6U );
        pMessage->id += ( ( (uint32_t)can1msgBuf[ buffer_number ][ 2 ] & (uint32_t)0xFC00U ) >> 10U );
        pMessage->id |= CAN_EFF_FLAG;
        rtr = can1msgBuf[ buffer_number ][ 2 ] & 0x0200;
    }
    /* check to see what type of message it is */
    /* RTR message */
    if( rtr != 0U )
    {
        pMessage->id |= CAN_RTR_FLAG;
    }
    /* normal message */
    else
    {

        pMessage->buf_len = (unsigned char)( can1msgBuf[ buffer_number ][ 2 ] & 0x000FU );
        memcpy( pMessage->buf, &can1msgBuf[ buffer_number ][ 3 ], 8 );
    }
}

/**
 * @brief      Copies a CAN message to the DMA memory buffer
 *
 * @param      buffer   pointer to the DMA buffer that will contain the message
 * @param      message  The message to be copied
 */
static void _MessageToBuffer( uint16_t *pBuffer, can_t *pMessage )
{
    if( pMessage->id & CAN_EFF_FLAG )
    {
        pBuffer[ 0 ] = ( ( ( uint16_t )( pMessage->id >> 16 ) & 0x1FFC ) ) | 0b1;
        pBuffer[ 1 ] = ( uint16_t )( pMessage->id >> 6 ) & 0x0FFF;
        pBuffer[ 2 ] = ( pMessage->buf_len & 0x0F ) + ( ( uint16_t )( pMessage->id << 10 ) & 0xFC00 );
    }
    else
    {
        pBuffer[ 0 ] = ( pMessage->id & 0x000007FF ) << 2;
        pBuffer[ 1 ] = 0;
        pBuffer[ 2 ] = pMessage->buf_len & 0x0F;
    }

    memcpy( &pBuffer[ 3 ], pMessage->buf, 8 );
}

/**
 * @brief      Change the CAN module mode of operation
 */
static void _ConfigMode( bool bConfig )
{
    if( bConfig )
    {
        // Disable interrupts before the Initialization
        IEC2bits.C1IE = 0;
        C1INTE = 0;

        // set the CAN{instance}_initialize module to the options selected in the User Interface

        /* put the module in configuration mode */
        C1CTRL1bits.REQOP = CAN_CONFIGURATION_MODE;
        while( C1CTRL1bits.OPMODE != CAN_CONFIGURATION_MODE )
            ;
    }
    else
    {

        // put the module in normal mode
        C1CTRL1bits.REQOP = CAN_NORMAL_OPERATION_MODE;
        while( C1CTRL1bits.OPMODE != CAN_NORMAL_OPERATION_MODE )
            ;
#if defined( CAN_USE_INTERRUPTS )
        /* Enable CAN1 Interrupt */
        IEC2bits.C1IE = 1;
#endif
    }
}

//------------------------------//----------------------------------------------
// Public functions

/**
 * @brief      Configures an acceptance filter for incoming messages
 *
 * n-> Filter number [0-15]
 * identifier-> Bit ordering is given below
 * Filter Identifier (29-bits) : 0b000f ffff ffff ffff ffff ffff ffff ffff
 *                                    |____________|_____________________|
 *                                      SID10:0           EID17:0
 *
 *
 * Filter Identifier (11-bits) : 0b0000 0000 0000 0000 0000 0fff ffff ffff
 *                                                           |___________|
 *                                                               SID10:0
 */
#if defined( CAN_ALLOW_FILTER_UPDATE )
uint8_t CAN_SetRxFilter( int n, canid_t id, unsigned int bufPnt, unsigned int maskSel )
{
    if( n < CAN_MAX_FILTERS && maskSel < CAN_MAX_MASKS )
    {
        unsigned long sid10_0 = 0;
        unsigned int *sidRegAddr, *bufPntRegAddr, *maskSelRegAddr, *fltEnRegAddr;

        _ConfigMode( true );

        C1CTRL1bits.WIN = 1;

        // Obtain the Address of CiRXFnSID, CiBUFPNTn, CiFMSKSELn and CiFEN register for a given filter number "n"
        sidRegAddr = (unsigned int *)( &C1RXF0SID + ( n << 1 ) );
        bufPntRegAddr = (unsigned int *)( &C1BUFPNT1 + ( n >> 2 ) );
        maskSelRegAddr = (unsigned int *)( &C1FMSKSEL1 + ( n >> 3 ) );
        fltEnRegAddr = (unsigned int *)( &C1FEN1 );

        // Bit-filed manupulation to write to Filter identifier register
        if( id & CAN_EFF_FLAG )
        {
            unsigned long eid15_0, eid17_16;

            // Filter Extended Identifier
            eid15_0 = ( id & 0xFFFF );
            eid17_16 = ( id >> 16 ) & 0x3;
            sid10_0 = ( id >> 18 ) & 0x7FF;

            *sidRegAddr = ( ( ( sid10_0 ) << 5 ) + 0x8 ) + eid17_16;  // Write to CiRXFnSID Register
            *( sidRegAddr + 1 ) = eid15_0;                            // Write to CiRXFnEID Register
        }
        else
        {  // Filter Standard Identifier
            sid10_0 = ( id & 0x7FF );
            *sidRegAddr = ( sid10_0 ) << 5;  // Write to CiRXFnSID Register
            *( sidRegAddr + 1 ) = 0;         // Write to CiRXFnEID Register
        }

        *bufPntRegAddr = ( *bufPntRegAddr ) & ( 0xFFFF - ( 0xF << ( 4 * ( n & 3 ) ) ) );  // clear nibble
        *bufPntRegAddr = ( ( bufPnt << ( 4 * ( n & 3 ) ) ) | ( *bufPntRegAddr ) );        // Write to C1BUFPNTn Register

        *maskSelRegAddr = ( *maskSelRegAddr ) & ( 0xFFFF - ( 0x3 << ( ( n & 7 ) * 2 ) ) );  // clear 2 bits
        *maskSelRegAddr = ( ( maskSel << ( 2 * ( n & 7 ) ) ) | ( *maskSelRegAddr ) );  // Write to C1FMSKSELn Register

        *fltEnRegAddr = ( ( 0x1 << n ) | ( *fltEnRegAddr ) );  // Write to C1FEN1 Register

        C1CTRL1bits.WIN = 0;

        _ConfigMode( false );

        // success
        return 0;
    }

    // error return
    return 1;
}
#endif

/**
 * @brief      Sets a receive filter mask
 *
 * m-> Mask number [0-2]
 * identifier-> Bit ordering is given below
 * Filter Mask Identifier (29-bits) : 0b000f ffff ffff ffff ffff ffff ffff ffff
 *                                         |____________|_____________________|
 *                                             SID10:0           EID17:0
 *
 *
 * Filter Mask Identifier (11-bits) : 0b0000 0000 0000 0000 0000 0fff ffff ffff
 *                                                                |___________|
 *                                                                   SID10:0
 */
#if defined( CAN_ALLOW_FILTER_UPDATE )
uint8_t CAN_SetRxMask( int m, canid_t id, unsigned int mide )
{
    if( m < CAN_MAX_MASKS )
    {
        unsigned long sid10_0 = 0;
        unsigned int *maskRegAddr;

        _ConfigMode( true );

        C1CTRL1bits.WIN = 1;

        // Obtain the Address of CiRXMmSID register for given Mask number "m"
        maskRegAddr = (unsigned int *)( &C1RXM0SID + ( m << 1 ) );

        // Bit-field manupulation to write to Filter Mask register
        if( id & CAN_EFF_FLAG )
        {
            unsigned long eid15_0, eid17_16;

            // Filter Extended Identifier
            eid15_0 = ( id & 0xFFFF );
            eid17_16 = ( id >> 16 ) & 0x3;
            sid10_0 = ( id >> 18 ) & 0x7FF;

            if( mide == 1 )
                *maskRegAddr = ( ( sid10_0 ) << 5 ) + 0x0008 + eid17_16;  // Write to CiRXMnSID Register
            else
                *maskRegAddr = ( ( sid10_0 ) << 5 ) + eid17_16;  // Write to CiRXMnSID Register
            *( maskRegAddr + 1 ) = eid15_0;                      // Write to CiRXMnEID Register
        }
        else
        {  // Filter Standard Identifier
            sid10_0 = ( id & 0x7FF );
            if( mide == 1 )
                *maskRegAddr = ( ( sid10_0 ) << 5 ) + 0x0008;  // Write to CiRXMnSID Register
            else
                *maskRegAddr = ( sid10_0 ) << 5;  // Write to CiRXMnSID Register

            *( maskRegAddr + 1 ) = 0;  // Write to CiRXMnEID Register
        }

        C1CTRL1bits.WIN = 0;

        _ConfigMode( false );

        // success
        return 0;
    }

    // error return
    return 1;
}
#endif

/******************************************************************************
 *
 *    Function:     CAN1_TransmitEnable
 *    Description:       Setup the DMA for Transmit from the CAN module.  The
 *                       relevant DMA module APIs are grouped in this function
 *                       and this API needs to be called after DMA_Initialize
 *                       and CAN_Initialize
 *
 ******************************************************************************/
void CAN_TransmitEnable()
{
    /* setup channel 0 for peripheral indirect addressing mode
    normal operation, word operation and select as Tx to peripheral */

    /* DMA_PeripheralIrqNumberSet and DMA_TransferCountSet would be done in the
    DMA */

    /* setup the address of the peripheral CAN1 (C1TXD) */
    DMA_PeripheralAddressSet( CAN1_TX_DMA_CHANNEL, &C1TXD );

    /* DPSRAM start address offset value */
    DMA_StartAddressASet( CAN1_TX_DMA_CHANNEL, __builtin_dmaoffset( &can1msgBuf ) );

    /* enable the channel */
    DMA_ChannelEnable( CAN1_TX_DMA_CHANNEL );
}

/******************************************************************************
 *
 *    Function:     CAN_ReceiveEnable
 *    Description:       Setup the DMA for Receive on the CAN module.  The
 *                       relevant DMA module APIs are grouped in this function
 *                       and this API needs to be called after DMA_Initialize
 *                       and CAN_Initialize
 *
 ******************************************************************************/
void CAN_ReceiveEnable()
{
    /* setup DMA channel for peripheral indirect addressing mode
    normal operation, word operation and select as Rx to peripheral */

    /* setup the address of the peripheral CAN1 (C1RXD) */
    /* DMA_TransferCountSet and DMA_PeripheralIrqNumberSet would be set in
    the DMA_Initialize function */

    DMA_PeripheralAddressSet( CAN1_RX_DMA_CHANNEL, &C1RXD );

    /* DPSRAM start address offset value */
    DMA_StartAddressASet( CAN1_RX_DMA_CHANNEL, __builtin_dmaoffset( &can1msgBuf ) );

    /* enable the channel */
    DMA_ChannelEnable( CAN1_RX_DMA_CHANNEL );
}

/******************************************************************************
*
*    Function:      CAN1_isBusOff
*    Description:       Checks whether the transmitter in Bus off state
*

*    Return Value:      true - Transmitter in Bus Off state
*                       false - Transmitter not in Bus Off state
******************************************************************************/
bool CAN1_isBusOff()
{
    return C1INTFbits.TXBO;
}

/******************************************************************************
 *
 *    Function:     CAN1_isRXErrorPassive
 *    Description:       Checks whether the receive in error passive state
 *
 *    Return Value:      true - Receiver in Error Passive state
 *                       false - Receiver not in Error Passive state
 ******************************************************************************/
bool CAN1_isRXErrorPassive()
{
    return C1INTFbits.RXBP;
}

/******************************************************************************
 *
 *    Function:     CAN1_isTXErrorPassive
 *    Description:       Checks whether the transmitter in error passive state
 *
 *    Return Value:      true - Transmitter in Error Passive state
 *                       false - Transmitter not in Error Passive state
 ******************************************************************************/
bool CAN1_isTXErrorPassive()
{
    return ( C1INTFbits.TXBP );
}

/******************************************************************************
 *
 *    Function:     CAN1_messagesInBuffer
 *    Description:       returns the number of messages that are received
 *
 *    Return Value:      Number of message received
 ******************************************************************************/
uint8_t CAN1_messagesInBuffer()
{
    uint_fast8_t messageCount;
    uint_fast8_t currentBuffer;
    uint16_t receptionFlags;

    messageCount = 0;

#if( CAN1_FIFO_STARTING_BUFFER < 16 )
    /* Check any message in buffer 0 to buffer 15*/
    receptionFlags = C1RXFUL1;
    if( receptionFlags != 0 )
    {
        /* check whether a message is received */
        for( currentBuffer = 0; currentBuffer < 16; currentBuffer++ )
        {
            if( ( ( receptionFlags >> currentBuffer ) & 0x1 ) == 0x1 )
            {
                messageCount++;
            }
        }
    }
#endif

    /* Check any message in buffer 16 to buffer 32*/
    receptionFlags = C1RXFUL2;
    if( receptionFlags != 0 )
    {
        /* check whether a message is received */
        for( currentBuffer = 0; currentBuffer < 16; currentBuffer++ )
        {
            if( ( ( receptionFlags >> currentBuffer ) & 0x1 ) == 0x1 )
            {
                messageCount++;
            }
        }
    }

    return ( messageCount );
}

/******************************************************************************
 *
 *    Function:     CAN1_sleep
 *    Description:       Puts CAN1 module in disable mode.
 *
 ******************************************************************************/
void CAN_Sleep( void )
{
    C1INTFbits.WAKIF = 0;
    C1INTEbits.WAKIE = 1;

    /* put the module in disable mode */
    C1CTRL1bits.REQOP = CAN_DISABLE_MODE;
    while( C1CTRL1bits.OPMODE != CAN_DISABLE_MODE )
        ;

    // Wake up from sleep should set the CAN module straight into Normal mode
}

/**
 * Transmit a single CAN frame using a specified Tx buffer
 *
 * Will only transmit if the buffer_number is valid, the buffer
 * is enabled, and is not busy.
 *
 * RETURN: 0 - success
 *         1 - failure
 */
uint8_t CAN_TxMailbox( can_t *frame, unsigned which )
{
    uint8_t result = 1;

    if( CAN1_TX_BUFFER_COUNT > which )
    {
        CAN1_TX_CONTROLS *pTxControls = (CAN1_TX_CONTROLS *)&C1TR01CON;

        // point at the buffer of interest
        pTxControls += which;

        if( pTxControls->transmit_enabled == 1 )
        {
            if( pTxControls->send_request == 0 )
            {
                _MessageToBuffer( &can1msgBuf[ which ][ 0 ], frame );

                // @todo make this configurable
                pTxControls->priority = CAN_PRIORITY_MEDIUM;

                // set the message for transmission
                pTxControls->send_request = 1;

                // success
                result = 0;
            }
        }
    }

    return result;
}

/**
 * Transmit a single CAN frame
 *
 * Finds the first open transmit buffer and uses it.
 * If none are available, a failure indication is returned
 *
 * @return 0 - success
 *         1 - failure
 */
uint8_t CAN_Tx( can_t *frame )
{
#if CAN1_TX_BUFFER_COUNT > 0

    uint_fast8_t i;

    // look for the first open buffer
    for( i = 0; i < CAN1_TX_BUFFER_COUNT; i++ )
    {
        if( 0 == CAN_TxMailbox( frame, i ) )
        {
            return 0;
        }
    }
#endif

    return 1;
}

/** Read a single CAN frame
 *
 * Reads a CAN data frame from the receive buffer.
 * @return 0 - success
 *         1 - failure
 */
uint8_t CAN_Rx( can_t *frame )
{

    uint_fast8_t currentBuffer;
    uint_fast8_t shiftAmount;
    uint8_t result = 1;
    uint16_t receptionFlags;

    if( C1INTFbits.RBOVIF == 1 )
    {
        // could provide a callback here rather than just ignoring
        C1INTFbits.RBOVIF = 0;
    }

    currentBuffer = C1FIFObits.FNRB;
    if( currentBuffer < 16 )
    {
        receptionFlags = C1RXFUL1;
        shiftAmount = currentBuffer;
    }
    else
    {
        receptionFlags = C1RXFUL2;
        shiftAmount = currentBuffer - 16;
    }

    if( ( ( receptionFlags >> shiftAmount ) & 0x1 ) == 0x1 )
    {
        _DMACopy( currentBuffer, frame );

        if( currentBuffer < 16 )
        {
            // see errata issue #41
            // http://ww1.microchip.com/downloads/en/DeviceDoc/dsPIC33EPXXXGP50X-dsPIC33EPXXXMC20X-50X-PIC24EPXXXGP-MC20X-Family-Silicon-Errata-and-Data-Sheet-Clarification.pdf
            C1RXFUL1 = ~( 0x001 << shiftAmount );
        }
        else
        {
            // see errata issue #41
            // http://ww1.microchip.com/downloads/en/DeviceDoc/dsPIC33EPXXXGP50X-dsPIC33EPXXXMC20X-50X-PIC24EPXXXGP-MC20X-Family-Silicon-Errata-and-Data-Sheet-Clarification.pdf
            C1RXFUL2 = ~( 0x001 << shiftAmount );
        }

        result = 0;
    }

    return result;
}

/**
 * @brief      Checks for and returns an incoming CAN frame
 */
uint8_t CAN_RxMailbox( can_t *pFrame, unsigned which )
{
    // not supported currently
    // to implement this, you'd reserve some CAN buffers between the
    // transmit buffers and the start of the FIFO.
    // Then, I belive here you would check the corresponding C1RXFUL1
    // bit, and if it were set, copy the message and clear the bit.
    return 1;
}

/**
 * Module task loop
 */
void CAN_Tasks( void )
{
#if defined( CAN_USE_INTERRUPTS )
// Nothing to do here since we use DMA to transfer buffers automatically
#else

    // not sure if it is important to clear the flags, but it seems harmless
    C1INTF = 0;

#endif
}

/**
 * Powerup Initialization
 */
void CAN_Initialize( const void *pConfig )
{
    CAN_Config_t *p = (CAN_Config_t *)pConfig;

    // take transceiver out of standby
    CAN_STANDBY( 0 );

    // put module in config mode
    _ConfigMode( true );

    /* Set up the baud rate*/
    C1CFG1 = p->CiCFG1;
    C1CFG2 = p->CiCFG2;
    C1FCTRL = p->CiFCTRL;
    C1FEN1 = p->CiFEN1;
    C1CTRL1 = p->CiCTRL1;
    C1CTRL2 = p->CiCTRL2;

    C1CTRL1bits.WIN = 1;

    // mask IDs
    memcpy( (void *)&C1RXM0SID, p->CiRXM, CAN_MAX_MASKS << 2 );

    // Filter SID, 32 bits each
    memcpy( (void *)&C1RXF0SID, p->CiRXFxSID, CAN_MAX_FILTERS << 3 );

    // Filter to DMA buffer mapping
    memcpy( (void *)&C1BUFPNT1, p->CiBUFPNT, CAN_MAX_FILTERS >> 1 );

    // Filter mask selection
    memcpy( (void *)&C1FMSKSEL1, p->CiFMSKSEL, CAN_MAX_FILTERS >> 2 );

    /* clear window bit to access ECAN control registers */
    C1CTRL1bits.WIN = 0;

    // Set up transmit buffer priority and enable
    C1TR01CON = p->CiTR01CON;
    C1TR23CON = p->CiTR23CON;
    C1TR45CON = p->CiTR45CON;
    C1TR67CON = p->CiTR67CON;

    /* clear the buffer and overflow flags */
    C1RXFUL1 = 0x0000;
    C1RXFUL2 = 0x0000;
    C1RXOVF1 = 0x0000;
    C1RXOVF2 = 0x0000;

    /* configure the device to interrupt on the receive buffer full flag */
    /* clear the buffer full flags */
    C1INTFbits.RBIF = 0;

    // go to normal mode
    _ConfigMode( false );

#if defined( CAN_USE_INTERRUPTS )
    /* Enable Receive interrupt */
    C1INTEbits.RBIE = 1;

    /* Enable Error interrupt*/
    C1INTEbits.ERRIE = 1;

    /* Enable Receive buffer Overflow interrupt */
    C1INTEbits.RBOVIE = 1;
#endif

    CAN_TransmitEnable();
    CAN_ReceiveEnable();
}

/**
 * @brief      Interrupt service routine for the CAN1 interrupt
 */
#if defined( CAN_USE_INTERRUPTS )
void __attribute__( ( __interrupt__, no_auto_psv ) ) _C1Interrupt( void )
{

    if( C1INTFbits.ERRIF )
    {

        if( C1INTFbits.TXBO == 1 )
        {
            // CAN1_CallbackBusOff();
            C1INTFbits.TXBO = 0;
        }

        if( C1INTFbits.TXBP == 1 )
        {
            // CAN1_CallbackTxErrorPassive();
            C1INTFbits.TXBP = 0;
        }

        if( C1INTFbits.RXBP == 1 )
        {
            // CAN1_CallbackRxErrorPassive();
            C1INTFbits.RXBP = 0;
        }

        /* Call error notification function */
        C1INTFbits.ERRIF = 0;
    }

    if( C1INTFbits.RBIF )
    {
        C1INTFbits.RBIF = 0;

        /* Notification function */
        // CAN1_CallbackMessageReceived();
    }

    if( C1INTFbits.WAKIF )
    {
        C1INTFbits.WAKIF = 0;
    }

    IFS2bits.C1IF = 0;
}
#endif

/**
 End of File
*/