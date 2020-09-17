/**
 * Bootloader for PIC24 (CANBUS version)
 *
 * Implements a simple bootloader for writing internal program flash or EEPROM as applicable.
 * The bootloader communicates with an external "Loader" program via the CANBUS interface.
 *
 * Reference: DOC0004197A CANBUS Bootloader Protocol Specification
 *
 * The Loader selects the memory space to be programmed, and sends blocks of
 * data to the Bootloader, which programs them into the device.
 *
 * Memory spaces are:
 *      Internal Program FLASH for the Application
 *
 * At powerup, the bootloader waits for a command from the Loader.  If
 * there is a valid Application, and the user activates the keyswitch (one of the
 * hardware inputs), the bootloader terminates and jumps to the Application.
 *
 * The PIC program memory map looks like this (in 16-bit words, not to scale):
 *
 *             00000    +-------------------------------+
 *                      | IVT, AIVT.                    |
 *             01FF     +-------------------------------+
 *             0200     +-------------------------------+
 *                      | Bootloader executable         |
 *             0FFF     +-------------------------------+
 *        BL_C_APP_ADR  +-------------------------------+
 *                      | Application IVT.              |
 *                      +-------------------------------+
 *   BL_APP_ENTRY_ADDR  +-------------------------------+
 *                      | Application executable        |
 *      BL_C_APP_END-1  +-------------------------------+
 *      BL_C_APP_END    +-------------------------------+
 *                      | config words                  |
 *      End of memory   +-------------------------------+
 *
 *
 *
 *
 * Copyright (c) 2019 Control Solutions LLC.  All Rights Reserved.
 */

#include "bootloader.h"
#include "app.h"
#include "can.h"
#include "crc16.h"
#include "flash.h"
#include "j1939.h"
#include "j1939cfg.h"
#include "system.h"
#include "timer.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xc.h>

//------------------------------//----------------------------------------------
// Definitions

// the first block the bootloader should be allowed to write
#define MIN_FLASH_BLOCK ( BL_C_APP_ADR / BL_FLASH_BLK_SIZE )

// the last block the bootloader should be allowed to write
#define MAX_FLASH_BLOCK ( BL_C_APP_END / BL_FLASH_BLK_SIZE )

// Creates a 'function' that jumps to the Application start location
#define BootApplication() ( ( ( int ( * )( void ) )( BL_APP_ENTRY_ADDR ) )() )

// An enumeration that identifies the memory space we can deal with
typedef enum
{
    // internal program flash
    SPACE_APP = 0,
    NUM_SPACES,
} MemorySpace_e;

// Defines a CANBUS message payload we return when we receive the ENQ command
#define ENQ_RESPONSE_LENGTH 6
static uint8_t ENQ_RESPONSE[ ENQ_RESPONSE_LENGTH ] = {
    MODBUS_COMMAND,                // Function code
    BL_OP_ENQUIRE,                 // the command we are responding to
    BL_PLAT_TREX | BL_PROC_PIC24,  // An identifer we report to the enquiring host
    VERSION_MAJOR,                 // Bootloader version
    VERSION_MINOR,                 // Bootloader version
    NUM_SPACES                     // Number of memory spaces supported
};

// Defines a CANBUS message payload we return when we receive the SEL command
#define SEL_RESPONSE_LENGTH 8
static uint8_t SEL_RESPONSE[ NUM_SPACES ][ SEL_RESPONSE_LENGTH ] = {
    {
        MODBUS_COMMAND,                         // Function code
        BL_OP_SELECT,                           // the op code we are responding to
        ( ( BL_FLASH_BLK_SIZE >> 8 ) & 0xFF ),  // data block size MSB
        ( BL_FLASH_BLK_SIZE & 0xFF ),           // data block size LSB

        ( MIN_FLASH_BLOCK / 256 ),   // Application start block high byte
        ( MIN_FLASH_BLOCK & 0xFF ),  // Application start block high byte
        ( MAX_FLASH_BLOCK / 256 ),   // Application end block high byte
        ( MAX_FLASH_BLOCK & 0xFF ),  // Application end block high byte
    },

};

//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Local variables

static const CAN_Config_t _canConfig = {

    0x03,    // CiCFG1; BRP TQ = (2 x 4)/FCAN; SJW 1 x TQ;
    0x41A8,  // CiCFG2; WAKFIL enabled; SEG2PHTS Freely programmable; SEG2PH 2 x TQ; SEG1PH 6 x TQ; PRSEG 1 x TQ; SAM
             // Once at the sample point;
    0x00,    // CiCTRL1; CANCKS FOSC/2; CSIDL disabled; ABAT disabled; REQOP Sets Normal Operation Mode; WIN Uses buffer
             // window; CANCAP disabled;

    0x0000,  // CiCTRL2;

    0x01,  // CiFEN1

    0xC008,  // CiFCTRL; FSA Receive Buffer RB8; DMABS 32;

    // Transmit buffer enable and priority setting
    0x8080,  // CiTR01CON;
    0x8080,  // CiTR23CON;
    0x8080,  // CiTR45CON;
    0x8080,  // CiTR67CON;

    // Filter mask IDs
    {
        // Mask EXIDE 0x00F8FF00, MIDE=1
        //
        0x07C8, 0xFF00,  // CiRXM0SID, CiRXM0EID
                         //        0x0000, 0x0000,  // CiRXM1SID, CiRXM1EID
                         //        0x0000, 0x0000,  // CiRXM1SID, CiRXM1EID

    },

    // Receive Filter SIDs CiRXFxSID
    {
        // Filter 0: 0x00EF8000
        0x076B, 0x8000,  // CiRXFxSID, CiRXFxEID
        0x0000, 0x0000,  // CiRXFxSID, CiRXFxEID
        0x0000, 0x0000,  // CiRXFxSID, CiRXFxEID
        0x0000, 0x0000,  // CiRXFxSID, CiRXFxEID
        0x0000, 0x0000,  // CiRXFxSID, CiRXFxEID
        0x0000, 0x0000,  // CiRXFxSID, CiRXFxEID
        0x0000, 0x0000,  // CiRXFxSID, CiRXFxEID
                         //        0x0000, 0x0000,  // CiRXFxSID, CiRXFxEID
                         //        0x0000, 0x0000,  // CiRXFxSID, CiRXFxEID
                         //        0x0000, 0x0000,  // CiRXFxSID, CiRXFxEID
                         //        0x0000, 0x0000,  // CiRXFxSID, CiRXFxEID
                         //        0x0000, 0x0000,  // CiRXFxSID, CiRXFxEID
                         //        0x0000, 0x0000,  // CiRXFxSID, CiRXFxEID
                         //        0x0000, 0x0000,  // CiRXFxSID, CiRXFxEID
                         //        0x0000, 0x0000,  // CiRXFxSID, CiRXFxEID
                         //        0x0000, 0x0000,  // CiRXFxSID, CiRXFxEID
    },

    // CiBUFPNTx
    {
        0x000F,  // CiBUFPNT1, Filter 0 -> FIFO
        0x0000,  // CiBUFPNT2
                 //        0x0000,  // CiBUFPNT3
                 //        0x0000,  // CiBUFPNT4
    },

    // CiFMSKSELx
    {
        0x0000,  // C1FMSKSEL1, Filter 0 -> Mask 0
                 //        0x0000,  // C1FMSKSEL2
    }

};

// The size of the data block we are expecting (depends on memory space selected)
static uint16_t _blockSize;

// Which memory space we are supposed to deal with
static MemorySpace_e _memorySpace = SPACE_APP;

// Filter incoming commands to make sure they are coming in a valid sequence
static uint8_t _commandMask = BL_OP_MASK_LOCKED;

// keep track of if we have a valid application to jump to
static bool _bAppValid;

// Define a message buffer for outgoing CAN messages (responses).
// Since differ in payload only, we keep this copy and re-use it many times
static j1939_t _responseMsg = {
    PGN_DIRECT,   // parameter group number.
    NULL,         // pointer to data.
    0,            // size of data.
    255,          // destination of message.
    J1939CFG_SA,  // source of message.
    7,            // priority of message.
};

// keep track of time for polling J199 module
static TIMER_TICK _canTimer;

//------------------------------//----------------------------------------------
// Local functions

/**
 * @brief      Send an ACK or NACK message for the specified op code
 *
 * @param[in]  op     The operation code
 * @param[in]  value  The value (BL_OP_ACK or BL_OP_NACK )
 */
static void _SendResult( uint8_t op, uint8_t value )
{
    uint8_t responseBuf[ 3 ];

    responseBuf[ 0 ] = MODBUS_COMMAND;
    responseBuf[ 1 ] = op;
    responseBuf[ 2 ] = value;
    _responseMsg.buf = responseBuf;

    _responseMsg.buf_len = 3;
    (void)J1939_AppTx( &_responseMsg, NULL );
}

/**
 * Calculates a 16-bit checksum for the APPLICATION and sends it via UART
 *
 * The CRC is computed over the BL_C_APP_ADR to BL_C_APP_END, excluding the
 * last WORD.  The last WORD is where the checksum is stored permanently so
 * we can validate the application on powerup.
 * Note that because of the screwy PIC24 addressing, we can't just CRC over
 * the whole space including stored CRC and expect the result to be 0.
 *
 * @return void
 */
static uint16_t _GetAppChecksum( void )
{
    uint16_t crc = CRC16_INIT;
    uint32_t address = BL_C_APP_ADR;
    uint16_t offset, lo, hi;

    while( address < BL_C_APP_END - 2 )
    {

        TBLPAG = address >> 16;            // Initialize PM Page Boundary SFR
        offset = ( uint16_t )( address );  // Initialize lower word of address
        hi = __builtin_tblrdh( offset );
        lo = __builtin_tblrdl( offset );

        crc = CRC16_Update( crc, lo & 0xFF );
        crc = CRC16_Update( crc, lo >> 8 );
        crc = CRC16_Update( crc, hi & 0xFF );
        crc = CRC16_Update( crc, 0 );

        address += 2;
    };

    //  Now we read the stored checksum ( TBLPAG is still set)
    offset = ( uint16_t )( BL_C_APP_END - 2 );  // Initialize lower word of address
    hi = __builtin_tblrdh( offset );
    lo = __builtin_tblrdl( offset );

    // swap endian
    hi = ( lo >> 8 ) + ( lo << 8 );

    // compare (result = 0 for valid CRC))
    return ( crc - hi );
}

/**
 * @brief Updates a flag that tells us if we have a valid application in flash
 *
 * Computes the program space checksum and compares with the
 * stored value.  Sets the global flag if we have a valid
 * application in memory
 */
static inline void _CheckApp( void )
{
    _bAppValid = ( 0 == _GetAppChecksum() );

    LaunchTimerReset();
    // SYSTEM_LedSet( LED_ALL_OFF );
}

/**
 * @brief      Process a block of data
 *
 * The block should contain a 4-byte address plus one _blockSize worth of bytes.
 * Write it to memory and return an ACK or NAK
 *
 * @param      p      pointer to data block
 * @param[in]  bytes  The bytes
 */
static void _HandleDataBlock( uint8_t *p, uint16_t bytes )
{
    // data block plus 3 address bytes (block number)
    uint16_t expectedBytes = _blockSize + 4;

    // If we received all the bytes, we are successful.
    if( bytes == expectedBytes )
    {
        uint32_t address = ( (uint32_t)p[ 0 ] << 24 ) + ( (uint32_t)p[ 1 ] << 16 ) + ( (uint32_t)p[ 2 ] << 8 ) + p[ 3 ];

        // skip address words
        p += 4;

        unsigned int i = _blockSize;
        bool result = true;

        FLASH_Unlock( FLASH_UNLOCK_KEY );

        // while( 0 )

        while( i >= 6 )
        {
            uint32_t data0 = ( (uint32_t)p[ 2 ] << 16 ) + ( (uint32_t)p[ 1 ] << 8 ) + p[ 0 ];
            uint32_t data1 = ( (uint32_t)p[ 5 ] << 16 ) + ( (uint32_t)p[ 4 ] << 8 ) + p[ 3 ];

            result &= FLASH_WriteDoubleWord24( address, data0, data1 );

            uint32_t read0 = FLASH_ReadWord24( address );
            uint32_t read1 = FLASH_ReadWord24( address + 2 );

            if( read0 != data0 || read1 != data1 )
            {
                while( 1 )
                    ;
            }
            // we consume 6 bytes of the incoming data and advance the address by 4 words
            i -= 6;
            p += 6;
            address += 4;
        }

        FLASH_Lock();

        _SendResult( BL_OP_DATA, ( result ) ? BL_OP_ACK : BL_OP_NACK );
    }
    else
    {
        _SendResult( BL_OP_DATA, BL_OP_NACK );
    }
}

/**
 * @brief      Erases the application memory space
 *
 * @return     boolean true if no error
 */
static bool _EraseApp( void )
{
    uint32_t address = BL_C_APP_ADR;
    bool bResult = true;

    FLASH_Unlock( FLASH_UNLOCK_KEY );

    while( address < BL_C_APP_END )
    {
        bResult &= FLASH_ErasePage( address );
        address += FLASH_ERASE_PAGE_SIZE_IN_PC_UNITS;
    }

    FLASH_Lock();

    // address -= FLASH_ERASE_PAGE_SIZE_IN_PC_UNITS;

    return bResult;
}

/**
 * Erase the selected memory space
 *
 * @return void
 */
static void _Erase( void )
{
    uint8_t result = false;

    switch( _memorySpace )
    {
        case SPACE_APP:
            _bAppValid = false;
            result = _EraseApp();
            break;

        default:
            break;
    }

    _SendResult( BL_OP_ERASE, ( result ) ? BL_OP_ACK : BL_OP_NACK );
}

/**
 * @brief      Verify the selected memory space
 *
 * Uses the file-scope _memorySpace to know which memory
 * to validate.  This function calculates a checksum over the
 * entire memory space, and sends it as a response to the host.
 */
static void _Verify( void )
{
    uint16_t checksum;

    switch( _memorySpace )
    {
        case SPACE_APP:
            checksum = _GetAppChecksum();
            break;

        default:
            // this can't really happen, so ignore the error case
            return;
    }

    // form and send the response
    uint8_t responseBuf[ 4 ];
    responseBuf[ 0 ] = MODBUS_COMMAND;
    responseBuf[ 1 ] = BL_OP_VERIFY;
    responseBuf[ 2 ] = ( uint8_t )( checksum >> 8 );
    responseBuf[ 3 ] = ( uint8_t )( checksum );
    _responseMsg.buf = responseBuf;
    _responseMsg.buf_len = 4;
    (void)J1939_AppTx( &_responseMsg, NULL );
}

/**
 * @brief      Complete programming memory space
 *
 * Uses the file-scope _memorySpace to know which memory
 * to finish.
 */
static void _Finish( void )
{
    switch( _memorySpace )
    {
        case SPACE_APP:
            _SendResult( BL_OP_FINISH, BL_OP_ACK );
            break;

        default:
            _SendResult( BL_OP_FINISH, BL_OP_NACK );
            break;
    }

    // next command has to be an enq
    _commandMask = BL_OP_MASK_LOCKED;

    // see if we now have a valid application to launch
    _CheckApp();
}

/**
 * @brief      Callback from J1939 CANBUS when a new message is received
 *
 * @param      pMsg  The message
 */
void J1939_AppProcess( j1939_t *pMsg )
{
    // printf( "RX: %04lX %d\r\n", pMsg->pgn, pMsg->buf_len );

    // only thing we care about are direct, command messages that are long enough
    // to contain an op code
    if( PGN_DIRECT == pMsg->pgn && MODBUS_COMMAND == pMsg->buf[ 0 ] && pMsg->buf_len > 1 )
    {
        // response goes to the sender
        _responseMsg.dst = pMsg->src;

        // op code is the first byte of the packet
        uint8_t op = pMsg->buf[ 1 ];

        // If the byte is in the valid range of what we can accept
        if( ( op & _commandMask ) == op )
        {
            // remember when we received the most recent valid command
            LaunchTimerReset();

            switch( op )
            {
                case BL_OP_ENQUIRE:
                    _responseMsg.buf = ENQ_RESPONSE;
                    _responseMsg.buf_len = ENQ_RESPONSE_LENGTH;
                    (void)J1939_AppTx( &_responseMsg, NULL );
                    break;

                case BL_OP_SELECT:
                    if( pMsg->buf[ 2 ] < NUM_SPACES )
                    {
                        _memorySpace = pMsg->buf[ 2 ];

                        _responseMsg.buf = SEL_RESPONSE[ _memorySpace ];
                        _responseMsg.buf_len = SEL_RESPONSE_LENGTH;
                        (void)J1939_AppTx( &_responseMsg, NULL );

                        // calculate size of block we expect to receive
                        _blockSize =
                            ( (uint16_t)SEL_RESPONSE[ _memorySpace ][ 2 ] << 8 ) | SEL_RESPONSE[ _memorySpace ][ 3 ];

                        // now we allow all commands
                        _commandMask = BL_OP_MASK_ALL;
                    }
                    else
                    {
                        _SendResult( op, BL_OP_NACK );
                    }
                    break;

                case BL_OP_RESET:
                    asm( "RESET" );
                    break;

                case BL_OP_ERASE:
                case BL_OP_ERASE_ALL:
                    _Erase();
                    break;

                case BL_OP_DATA:
                    // process the block (adjust length to ignore function and op codes)
                    _HandleDataBlock( &pMsg->buf[ 2 ], pMsg->buf_len - 2 );
                    break;

                case BL_OP_VERIFY:
                    _Verify();
                    break;

                case BL_OP_FINISH:
                    _Finish();
                    break;

                default:
                    // unknown received command.  Lock down communication again.
                    // Host will need to start over.
                    _commandMask = BL_OP_MASK_LOCKED;
                    _SendResult( op, BL_OP_NACK );
                    break;
            }
        }
    }
}

//------------------------------//----------------------------------------------
// Public functions

/**
 * @brief      Entry point for bootloader.
 *
 * @return     Never returns
 */
int main( void )
{
    // hardware setup
    SYSTEM_Initialize();

    // SYS_Initialize();
    TIMER_Initialize();

    // Canbus init
    CAN_Initialize( &_canConfig );

    J1939_Initialize();

    // build a CRC helper table in memory
    CRC16_Initialize();

    // _Check Valid Application
    _CheckApp();

    LaunchTimerReset();

    _canTimer = TIMER_Now();

    //_TRISA7 = 0;

    while( 1 )
    {

        // Don't let watchdog timer reset us
        ClrWdt();

        CAN_Tasks();

        if( TIMER_Since( _canTimer ) >= J1939CFG_TICK_PERIOD )
        {
            //_LATA7 = !_LATA7;
            _canTimer += J1939CFG_TICK_PERIOD;
            J1939_Update();
        }
        else
        {
            // check for new incoming CAN messages
            // We do this frequently because we can get 3-4 packets
            // per millisecond with CAN rate of 500kbps
            J1939_CheckRx();
        }

        // If app is valid and keyswitch is on,
        if( _bAppValid && LaunchTimerExpired() && KEYSWITCH_ON() )
        {
            SYSTEM_Deinitialize();

            BootApplication();
        }

        SYSTEM_Sleep();
    }

    return 0;
}
