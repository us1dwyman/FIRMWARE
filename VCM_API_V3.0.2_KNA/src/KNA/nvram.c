/**
 * Tests the Non-volatile RAM interface
 *
 * Write and verify (destructive) test for the EEPROM
 *
 *
 */

#include "api.h"
#include "test.h"
#include <string.h>

//------------------------------//----------------------------------------------
// Definitions

#define BUF_LEN 16

//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Local variables

//------------------------------//----------------------------------------------
// Local functions

//------------------------------//----------------------------------------------
// Public functions

/**
 * Initialize the test runner
 */
void NVRAM_Initialize( void )
{
    bool bResult = true;

    uint8_t wbuf[ BUF_LEN ];
    uint8_t rbuf[ BUF_LEN ];

    memset( wbuf, 0x01, BUF_LEN );
    memset( rbuf, 0xFF, BUF_LEN );

    API_WriteNvram( 0, wbuf, BUF_LEN );
    API_ReadNvram( 0, rbuf, BUF_LEN );

    bResult = bResult & ( 0 == memcmp( wbuf, rbuf, BUF_LEN ) );

    // put it back to original state
    memset( wbuf, 0xFF, BUF_LEN );
    API_WriteNvram( 0, wbuf, BUF_LEN );

    // this takes a bit of time; enable it if you want
#if 0
    // write blocks of data to the entire NVRAM
    uint16_t address = 0;
    while( address < TOTAL_NVM_BYTES )
    {
        memset( wbuf, address & 0xff, BUF_LEN );
        API_WriteNvram( wbuf, address, BUF_LEN );

        address += BUF_LEN;
    }

    // Read them back
    address = 0;
    while( address < TOTAL_NVM_BYTES )
    {
        API_ReadNvram( rbuf, address, BUF_LEN );

        int i;
        for( i = 0; i < BUF_LEN; i++ )
        {
            // check if we got the right byte
            bResult = bResult & ( rbuf[ i ] == ( address & 0xFF ) );
        }

        address += BUF_LEN;
    }

    // Now put the EEPROM back to an erased state, just to be polite
    wbuf[ 0 ] = 0xFF;
    API_WriteNvram( wbuf, 0, TOTAL_NVM_BYTES );
#endif
    
    TEST_Complete( "NVRAM", bResult );
}


