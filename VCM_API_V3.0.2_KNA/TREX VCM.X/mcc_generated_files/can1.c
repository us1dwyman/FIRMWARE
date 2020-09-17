/**
  CAN1 Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    can1.c

  @Summary
    This is the generated driver implementation file for the CAN1 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This source file provides APIs for CAN1.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.95-b-SNAPSHOT
        Device            :  dsPIC33EP128MC506
        Driver Version    :  1.00
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.36
        MPLAB 	          :  MPLAB X v5.10
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
  Section: Included Files
*/

#include "can1.h"
//#include "ecan1.h"
#include "dma.h"

#define CAN1_TX_DMA_CHANNEL DMA_CHANNEL_1
#define CAN1_RX_DMA_CHANNEL DMA_CHANNEL_2

/* Valid options are 4, 6, 8, 12, 16, 24, or 32. */
#define CAN1_MESSAGE_BUFFERS 32

#define CAN1_FIFO_STARTING_BUFFER 0x8

#define CAN1_TX_BUFFER_COUNT 8

/******************************************************************************/

/******************************************************************************/
/* Private type definitions                                               */
/******************************************************************************/
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

/******************************************************************************/
/* Private variable definitions                                               */
/******************************************************************************/
/* This alignment is required because of the DMA's peripheral indirect
 * addressing mode. */
static unsigned int can1msgBuf[ CAN1_MESSAGE_BUFFERS ][ 8 ] __attribute__( ( aligned( 32 * 8 * 2 ) ) );

/******************************************************************************/
/* Private function prototypes                                                */
/******************************************************************************/
static void CAN1_DMACopy( uint8_t buffer_number, uCAN_MSG *message );
static void CAN1_MessageToBuffer( uint16_t *buffer, uCAN_MSG *message );

///* Null weak implementations of callback functions. */
// void __attribute__((weak, deprecate("This callback ECAN1_CallbackBusOff() call will removed later")))
// CAN1_CallbackBusOff(void)
//{
//    ECAN1_CallbackBusOff();
//}
//
// void __attribute__((weak, deprecate("This callback ECAN1_CallbackTxErrorPassive() call will removed later")))
// CAN1_CallbackTxErrorPassive(void)
//{
//    ECAN1_CallbackTxErrorPassive();
//}
//
// void __attribute__((weak, deprecate("This callback ECAN1_CallbackRxErrorPassive() call will removed later")))
// CAN1_CallbackRxErrorPassive(void)
//{
//    ECAN1_CallbackRxErrorPassive();
//}
//
// void __attribute__((weak, deprecate("This callback ECAN1_CallbackMessageReceived() call will removed later")))
// CAN1_CallbackMessageReceived(void)
//{
//    ECAN1_CallbackMessageReceived();
//}
//
// void __attribute__((weak, deprecate("This callback ECAN1_CallbackRxBufferOverflow() call will removed later")))
// CAN1_CallbackRxBufferOverflow()
//{
//    ECAN1_CallbackRxBufferOverflow();
//}

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

/*
This function configures Acceptance Filter "n"

Inputs:
n-> Filter number [0-15]
identifier-> Bit ordering is given below
Filter Identifier (29-bits) : 0b000f ffff ffff ffff ffff ffff ffff ffff
                                   |____________|_____________________|
                                      SID10:0           EID17:0


Filter Identifier (11-bits) : 0b0000 0000 0000 0000 0000 0fff ffff ffff
                                                          |___________|
                                                              SID10:0
exide -> "0" for standard identifier
         "1" for Extended identifier

bufPnt -> Message buffer to store filtered message [0-15]
maskSel -> Optinal Masking of identifier bits [0-3]

*/
void CAN1_SetRxFilter( int n, long identifier, unsigned int exide, unsigned int bufPnt, unsigned int maskSel )
{

    unsigned long sid10_0 = 0;
    unsigned int *sidRegAddr, *bufPntRegAddr, *maskSelRegAddr, *fltEnRegAddr;

    C1CTRL1bits.WIN = 1;

    // Obtain the Address of CiRXFnSID, CiBUFPNTn, CiFMSKSELn and CiFEN register for a given filter number "n"
    sidRegAddr = (unsigned int *)( &C1RXF0SID + ( n << 1 ) );
    bufPntRegAddr = (unsigned int *)( &C1BUFPNT1 + ( n >> 2 ) );
    maskSelRegAddr = (unsigned int *)( &C1FMSKSEL1 + ( n >> 3 ) );
    fltEnRegAddr = (unsigned int *)( &C1FEN1 );

    // Bit-filed manupulation to write to Filter identifier register
    if( exide == 1 )
    {
        unsigned long eid15_0, eid17_16;

        // Filter Extended Identifier
        eid15_0 = ( identifier & 0xFFFF );
        eid17_16 = ( identifier >> 16 ) & 0x3;
        sid10_0 = ( identifier >> 18 ) & 0x7FF;

        *sidRegAddr = ( ( ( sid10_0 ) << 5 ) + 0x8 ) + eid17_16;  // Write to CiRXFnSID Register
        *( sidRegAddr + 1 ) = eid15_0;                            // Write to CiRXFnEID Register
    }
    else
    {  // Filter Standard Identifier
        sid10_0 = ( identifier & 0x7FF );
        *sidRegAddr = ( sid10_0 ) << 5;  // Write to CiRXFnSID Register
        *( sidRegAddr + 1 ) = 0;         // Write to CiRXFnEID Register
    }

    *bufPntRegAddr = ( *bufPntRegAddr ) & ( 0xFFFF - ( 0xF << ( 4 * ( n & 3 ) ) ) );  // clear nibble
    *bufPntRegAddr = ( ( bufPnt << ( 4 * ( n & 3 ) ) ) | ( *bufPntRegAddr ) );        // Write to C1BUFPNTn Register

    *maskSelRegAddr = ( *maskSelRegAddr ) & ( 0xFFFF - ( 0x3 << ( ( n & 7 ) * 2 ) ) );  // clear 2 bits
    *maskSelRegAddr = ( ( maskSel << ( 2 * ( n & 7 ) ) ) | ( *maskSelRegAddr ) );       // Write to C1FMSKSELn Register

    *fltEnRegAddr = ( ( 0x1 << n ) | ( *fltEnRegAddr ) );  // Write to C1FEN1 Register

    C1CTRL1bits.WIN = 0;
}

/*
This function configures Acceptance Filter Mask "m"

Inputs:
m-> Mask number [0-2]
identifier-> Bit ordering is given below
Filter Mask Identifier (29-bits) : 0b000f ffff ffff ffff ffff ffff ffff ffff
                                        |____________|_____________________|
                                            SID10:0           EID17:0


Filter Mask Identifier (11-bits) : 0b0000 0000 0000 0000 0000 0fff ffff ffff
                                                               |___________|
                                                                  SID10:0

mide ->  "0"  Match either standard or extended address message if filters match
         "1"  Match only message types that correpond to 'exide' bit in filter

*/

void CAN1_SetRxMask( int m, long identifier, unsigned int mide, unsigned int exide )
{

    unsigned long sid10_0 = 0;
    unsigned int *maskRegAddr;

    C1CTRL1bits.WIN = 1;

    // Obtain the Address of CiRXMmSID register for given Mask number "m"
    maskRegAddr = (unsigned int *)( &C1RXM0SID + ( m << 1 ) );

    // Bit-filed manupulation to write to Filter Mask register
    if( exide == 1 )
    {
        unsigned long eid15_0, eid17_16;

        // Filter Extended Identifier
        eid15_0 = ( identifier & 0xFFFF );
        eid17_16 = ( identifier >> 16 ) & 0x3;
        sid10_0 = ( identifier >> 18 ) & 0x7FF;

        if( mide == 1 )
            *maskRegAddr = ( ( sid10_0 ) << 5 ) + 0x0008 + eid17_16;  // Write to CiRXMnSID Register
        else
            *maskRegAddr = ( ( sid10_0 ) << 5 ) + eid17_16;  // Write to CiRXMnSID Register
        *( maskRegAddr + 1 ) = eid15_0;                      // Write to CiRXMnEID Register
    }
    else
    {  // Filter Standard Identifier
        sid10_0 = ( identifier & 0x7FF );
        if( mide == 1 )
            *maskRegAddr = ( ( sid10_0 ) << 5 ) + 0x0008;  // Write to CiRXMnSID Register
        else
            *maskRegAddr = ( sid10_0 ) << 5;  // Write to CiRXMnSID Register

        *( maskRegAddr + 1 ) = 0;  // Write to CiRXMnEID Register
    }

    C1CTRL1bits.WIN = 0;
}

/**
  Section: CAN1 APIs
*****************************************************************************************/

void CAN1_Initialize( void )
{
    // Disable interrupts before the Initialization
    IEC2bits.C1IE = 0;
    C1INTE = 0;

    // set the CAN{instance}_initialize module to the options selected in the User Interface

    /* put the module in configuration mode */
    C1CTRL1bits.REQOP = CAN_CONFIGURATION_MODE;
    while( C1CTRL1bits.OPMODE != CAN_CONFIGURATION_MODE )
        ;

    /* Set up the baud rate*/
    C1CFG1 = 0x03;     // BRP TQ = (2 x 4)/FCAN; SJW 1 x TQ;
    C1CFG2 = 0x41A8;   // WAKFIL enabled; SEG2PHTS Freely programmable; SEG2PH 2 x TQ; SEG1PH 6 x TQ; PRSEG 1 x TQ; SAM
                       // Once at the sample point;
    C1FCTRL = 0xC008;  // FSA Receive Buffer RB8; DMABS 32;
    C1FEN1 = 0x07;     // FLTEN8 disabled; FLTEN7 disabled; FLTEN9 disabled; FLTEN0 enabled; FLTEN2 enabled; FLTEN10
                       // disabled; FLTEN1 enabled; FLTEN11 disabled; FLTEN4 disabled; FLTEN3 disabled; FLTEN6 disabled;
                       // FLTEN5 disabled; FLTEN12 disabled; FLTEN13 disabled; FLTEN14 disabled; FLTEN15 disabled;
    C1CTRL1 = 0x00;  // CANCKS FOSC/2; CSIDL disabled; ABAT disabled; REQOP Sets Normal Operation Mode; WIN Uses buffer
                     // window; CANCAP disabled;

    /* Mask configuration */

    // Mask for an 11-bit identifier
    CAN1_SetRxMask( 0, 0xFFF, 1, 0 );

    // Mask for an extended identifier with a specific J1939 destination address
    CAN1_SetRxMask( 1, 0x0000FF00, 1, 1 );

    // Mask for extended identifier with a specific PGN
    CAN1_SetRxMask( 2, 0x00FFFF00, 1, 1 );

    /* Filter configuration */

    // Filter any incoming J1939-style message addressed to us
    CAN1_SetRxFilter( 0, 0x00008000, 1, 0x0F, 1 );

    // Ping messages (debug)
    CAN1_SetRxFilter( 1, 0x111, 0, 0x0F, 0 );

    // Braincorp messages
    CAN1_SetRxFilter( 2, 0x549, 0, 0x0F, 0 );
    CAN1_SetRxFilter( 3, 0x4C9, 0, 0x0F, 0 );

    // Broadcast messages from HMI
    CAN1_SetRxFilter( 4, 0x00FF1000, 1, 0x0F, 2 );
 

    // Phoenix Messages
    CAN1_SetRxFilter( 5, 0x00FF0000, 1, 0x0F, 2 );
    CAN1_SetRxFilter( 6, 0x00FF0100, 1, 0x0F, 2 );
    CAN1_SetRxFilter( 7, 0x00FF0200, 1, 0x0F, 2 );
    CAN1_SetRxFilter( 8, 0x00FF0300, 1, 0x0F, 2 );
    CAN1_SetRxFilter( 9, 0x00FF0400, 1, 0x0F, 2 );
    CAN1_SetRxFilter( 10, 0x00FF0500, 1, 0x0F, 2 );
    CAN1_SetRxFilter( 11, 0x00FF0600, 1, 0x0F, 2 );
    

    /* enable window to access the filter configuration registers */
    /* use filter window*/
    C1CTRL1bits.WIN = 1;

#if 0
    /* select acceptance masks for filters */
    C1FMSKSEL1bits.F0MSK = 0x0;  // Select Mask 0 for Filter 0
    C1FMSKSEL1bits.F1MSK = 0x1;  // Select Mask 1 for Filter 1
    C1FMSKSEL1bits.F2MSK = 0x2;  // Select Mask 2 for Filter 2
    C1FMSKSEL1bits.F3MSK = 0x0;  // Select Mask 0 for Filter 3

    /* Configure the masks */
    C1RXM0SIDbits.SID = 0x67f;
    C1RXM1SIDbits.SID = 0x7ff;
    C1RXM2SIDbits.SID = 0x7ff;

    C1RXM0SIDbits.EID = 0x0;
    C1RXM1SIDbits.EID = 0x3;
    C1RXM2SIDbits.EID = 0x3;

    C1RXM0EID = 0x00;
    C1RXM1EID = 0xFFFF;
    C1RXM2EID = 0xFFFF;

    C1RXM0SIDbits.MIDE = 0x0;
    C1RXM1SIDbits.MIDE = 0x0;
    C1RXM2SIDbits.MIDE = 0x0;

    /* Configure the filters */
    C1RXF0SIDbits.SID = 0x111;
    C1RXF1SIDbits.SID = 0x43f;
    C1RXF2SIDbits.SID = 0x43f;
    C1RXF3SIDbits.SID = 0x549;

    C1RXF0SIDbits.EID = 0x0;
    C1RXF1SIDbits.EID = 0x3;
    C1RXF2SIDbits.EID = 0x3;
    C1RXF3SIDbits.EID = 0x0;

    C1RXF0EID = 0x00;
    C1RXF1EID = 0x1081;
    C1RXF2EID = 0x82;
    C1RXF3EID = 0x00;

    C1RXF0SIDbits.EXIDE = 0x0;
    C1RXF1SIDbits.EXIDE = 0x0;
    C1RXF2SIDbits.EXIDE = 0x0;
    C1RXF3SIDbits.EXIDE = 0x0;
#endif

    /* FIFO Mode */
    C1BUFPNT1bits.F0BP = 0xf;  // Filter 0 uses FIFO
    C1BUFPNT1bits.F1BP = 0xf;  // Filter 1 uses FIFO
    C1BUFPNT1bits.F2BP = 0xf;  // Filter 2 uses FIFO
    C1BUFPNT1bits.F3BP = 0xf;  // Filter 3 uses FIFO

    /* clear window bit to access ECAN control registers */
    C1CTRL1bits.WIN = 0;

    /* CAN1, Buffer 0 is a Transmit Buffer */
    C1TR01CONbits.TXEN0 = 0x1;  // Buffer 0 is a Transmit Buffer
    C1TR01CONbits.TXEN1 = 0x1;  // Buffer 1 is a Transmit Buffer
    C1TR23CONbits.TXEN2 = 0x1;  // Buffer 2 is a Transmit Buffer
    C1TR23CONbits.TXEN3 = 0x1;  // Buffer 3 is a Transmit Buffer
    C1TR45CONbits.TXEN4 = 0x1;  // Buffer 4 is a Transmit Buffer
    C1TR45CONbits.TXEN5 = 0x1;  // Buffer 5 is a Transmit Buffer
    C1TR67CONbits.TXEN6 = 0x1;  // Buffer 6 is a Transmit Buffer
    C1TR67CONbits.TXEN7 = 0x1;  // Buffer 7 is a Transmit Buffer

    C1TR01CONbits.TX0PRI = 0x0;  // Message Buffer 0 Priority Level
    C1TR01CONbits.TX1PRI = 0x0;  // Message Buffer 1 Priority Level
    C1TR23CONbits.TX2PRI = 0x0;  // Message Buffer 2 Priority Level
    C1TR23CONbits.TX3PRI = 0x0;  // Message Buffer 3 Priority Level
    C1TR45CONbits.TX4PRI = 0x0;  // Message Buffer 4 Priority Level
    C1TR45CONbits.TX5PRI = 0x0;  // Message Buffer 5 Priority Level
    C1TR67CONbits.TX6PRI = 0x0;  // Message Buffer 6 Priority Level
    C1TR67CONbits.TX7PRI = 0x0;  // Message Buffer 7 Priority Level

    /* clear the buffer and overflow flags */
    C1RXFUL1 = 0x0000;
    C1RXFUL2 = 0x0000;
    C1RXOVF1 = 0x0000;
    C1RXOVF2 = 0x0000;

    /* configure the device to interrupt on the receive buffer full flag */
    /* clear the buffer full flags */
    C1INTFbits.RBIF = 0;

    /* put the module in normal mode */
    C1CTRL1bits.REQOP = CAN_NORMAL_OPERATION_MODE;
    while( C1CTRL1bits.OPMODE != CAN_NORMAL_OPERATION_MODE )
        ;

    /* Enable CAN1 Interrupt */
 //   IEC2bits.C1IE = 1;

    /* Enable Receive interrupt */
//    C1INTEbits.RBIE = 1;

    /* Enable Error interrupt*/
 //   C1INTEbits.ERRIE = 1;

    /* Enable Receive buffer Overflow interrupt */
 //   C1INTEbits.RBOVIE = 1;
}

/******************************************************************************
 *
 *    Function:		CAN1_TransmitEnable
 *    Description:       Setup the DMA for Transmit from the CAN module.  The
 *                       relevant DMA module APIs are grouped in this function
 *                       and this API needs to be called after DMA_Initialize
 *                       and CAN_Initialize
 *
 ******************************************************************************/
void CAN1_TransmitEnable()
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
 *    Function:		CAN1_ReceiveEnable
 *    Description:       Setup the DMA for Receive on the CAN module.  The
 *                       relevant DMA module APIs are grouped in this function
 *                       and this API needs to be called after DMA_Initialize
 *                       and CAN_Initialize
 *
 ******************************************************************************/
void CAN1_ReceiveEnable()
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
 *    Function:		CAN1_transmit
 *    Description:       Transmits the message from user buffer to CAN buffer
 *                       as per the buffer number allocated.
 *                       Allocation of the buffer number is done by user
 *
 *    Arguments:		priority : priority of the message to be transmitted
 *                       sendCanMsg: pointer to the message object
 *
 *    Return Value:      true - Transmit successful
 *                       false - Transmit failure
 ******************************************************************************/
bool CAN1_transmit( CAN_TX_PRIOIRTY priority, uCAN_MSG *sendCanMsg )
{
    CAN1_TX_CONTROLS *pTxControls = (CAN1_TX_CONTROLS *)&C1TR01CON;
    uint_fast8_t i;
    bool messageSent = false;

    if( CAN1_TX_BUFFER_COUNT > 0 )
    {
        for( i = 0; i < CAN1_TX_BUFFER_COUNT; i++ )
        {
            if( pTxControls->transmit_enabled == 1 )
            {
                if( pTxControls->send_request == 0 )
                {
                    CAN1_MessageToBuffer( &can1msgBuf[ i ][ 0 ], sendCanMsg );

                    pTxControls->priority = priority;

                    /* set the message for transmission */
                    pTxControls->send_request = 1;

                    messageSent = true;
                    break;
                }
            }

            pTxControls++;
        }
    }
    return messageSent;
}

/******************************************************************************
 *
 *    Function:		CAN1_receive
 *    Description:       Receives the message from CAN buffer to user buffer
 *
 *    Arguments:		recCanMsg: pointer to the message object
 *
 *    Return Value:      true - Receive successful
 *                       false - Receive failure
 ******************************************************************************/
bool CAN1_receive( uCAN_MSG *recCanMsg )
{
    uint_fast8_t currentBuffer;
    uint_fast8_t shiftAmount;
    bool messageReceived = false;
    uint16_t receptionFlags;

    if( C1INTFbits.RBOVIF == 1 )
    {
        C1INTFbits.RBOVIF = 0;
        /* Receive buffer overflow occured, call the notification function */
        //        CAN1_CallbackRxBufferOverflow();
        return messageReceived;
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
        CAN1_DMACopy( currentBuffer, recCanMsg );

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

        messageReceived = true;
    }

    return ( messageReceived );
}

/******************************************************************************
*
*    Function:		CAN1_isBusOff
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
 *    Function:		CAN1_isRXErrorPassive
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
 *    Function:		CAN1_isTXErrorPassive
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
 *    Function:		CAN1_messagesInBuffer
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
 *    Function:		CAN1_sleep
 *    Description:       Puts CAN1 module in disable mode.
 *
 ******************************************************************************/
void CAN1_sleep( void )
{
    C1INTFbits.WAKIF = 0;
    C1INTEbits.WAKIE = 1;

    /* put the module in disable mode */
    C1CTRL1bits.REQOP = CAN_DISABLE_MODE;
    while( C1CTRL1bits.OPMODE != CAN_DISABLE_MODE )
        ;

    // Wake up from sleep should set the CAN module straight into Normal mode
}

/*******************************************************************************
 * PRIVATE FUNCTIONS
 ******************************************************************************/

/******************************************************************************
 *
 *    Function:		CAN1_DMACopy
 *    Description:       moves the message from the DMA memory to RAM
 *
 *    Arguments:		*message: a pointer to the message structure in RAM
 *			that will store the message.
 *
 *
 ******************************************************************************/
static void CAN1_DMACopy( uint8_t buffer_number, uCAN_MSG *message )
{
    uint16_t ide = 0;
    uint16_t rtr = 0;
    uint32_t id = 0;

    /* read word 0 to see the message type */
    ide = can1msgBuf[ buffer_number ][ 0 ] & 0x0001U;

    /* check to see what type of message it is */
    /* message is standard identifier */
    if( ide == 0U )
    {
        message->frame.id = ( can1msgBuf[ buffer_number ][ 0 ] & 0x1FFCU ) >> 2U;
        message->frame.idType = CAN_FRAME_STD;
        rtr = can1msgBuf[ buffer_number ][ 0 ] & 0x0002U;
    }
    /* message is extended identifier */
    else
    {
        id = can1msgBuf[ buffer_number ][ 0 ] & 0x1FFCU;
        message->frame.id = id << 16U;
        message->frame.id += ( ( (uint32_t)can1msgBuf[ buffer_number ][ 1 ] & (uint32_t)0x0FFF ) << 6U );
        message->frame.id += ( ( (uint32_t)can1msgBuf[ buffer_number ][ 2 ] & (uint32_t)0xFC00U ) >> 10U );
        message->frame.idType = CAN_FRAME_EXT;
        rtr = can1msgBuf[ buffer_number ][ 2 ] & 0x0200;
    }
    /* check to see what type of message it is */
    /* RTR message */
    if( rtr != 0U )
    {
        /* to be defined ?*/
        message->frame.msgtype = CAN_MSG_RTR;
    }
    /* normal message */
    else
    {
        message->frame.msgtype = CAN_MSG_DATA;
        message->frame.data0 = (unsigned char)can1msgBuf[ buffer_number ][ 3 ];
        message->frame.data1 = (unsigned char)( ( can1msgBuf[ buffer_number ][ 3 ] & 0xFF00U ) >> 8U );
        message->frame.data2 = (unsigned char)can1msgBuf[ buffer_number ][ 4 ];
        message->frame.data3 = (unsigned char)( ( can1msgBuf[ buffer_number ][ 4 ] & 0xFF00U ) >> 8U );
        message->frame.data4 = (unsigned char)can1msgBuf[ buffer_number ][ 5 ];
        message->frame.data5 = (unsigned char)( ( can1msgBuf[ buffer_number ][ 5 ] & 0xFF00U ) >> 8U );
        message->frame.data6 = (unsigned char)can1msgBuf[ buffer_number ][ 6 ];
        message->frame.data7 = (unsigned char)( ( can1msgBuf[ buffer_number ][ 6 ] & 0xFF00U ) >> 8U );
        message->frame.dlc = (unsigned char)( can1msgBuf[ buffer_number ][ 2 ] & 0x000FU );
    }
}

/******************************************************************************
 *
 *    Function:		CAN1_MessageToBuffer
 *    Description:       This function takes the input message, reformats it,
 *                       and copies it to the specified CAN module buffer
 *
 *    Arguments:		*buffer: a pointer to the buffer where the message
 *                       would be stored
 *                       *message: pointer to the input message that is received
 *                       by the CAN module
 *
 ******************************************************************************/
static void CAN1_MessageToBuffer( uint16_t *buffer, uCAN_MSG *message )
{
    if( message->frame.idType == CAN_FRAME_STD )
    {
        buffer[ 0 ] = ( message->frame.id & 0x000007FF ) << 2;
        buffer[ 1 ] = 0;
        buffer[ 2 ] = message->frame.dlc & 0x0F;
    }
    else
    {
        buffer[ 0 ] = ( ( ( uint16_t )( message->frame.id >> 16 ) & 0x1FFC ) ) | 0b1;
        buffer[ 1 ] = ( uint16_t )( message->frame.id >> 6 ) & 0x0FFF;
        buffer[ 2 ] = ( message->frame.dlc & 0x0F ) + ( ( uint16_t )( message->frame.id << 10 ) & 0xFC00 );
    }

    buffer[ 3 ] = ( ( message->frame.data1 ) << 8 ) + message->frame.data0;
    buffer[ 4 ] = ( ( message->frame.data3 ) << 8 ) + message->frame.data2;
    buffer[ 5 ] = ( ( message->frame.data5 ) << 8 ) + message->frame.data4;
    buffer[ 6 ] = ( ( message->frame.data7 ) << 8 ) + message->frame.data6;
}

/**
 End of File
*/