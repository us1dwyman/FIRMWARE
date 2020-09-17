#ifndef J1939_H
#define J1939_H
#include "j1939cfg.h"
#include <stdint.h>

/* SAE J1939 Source Addresses found in J1939 p. 45. 252 is
   for experimental use */
#define J1939_ADDR_DIAG_TOOL1 249
#define J1939_ADDR_DIAG_TOOL2 250
#define J1939_ADDR_EXPERIMENTAL_USE 252
#define J1939_ADDR_NULL 254
#define J1939_ADDR_GLOBAL 255

// constrain source addresses to valid proprietary range
#define J1939_SALOW_DEFAULT 0x80
#define J1939_SAHIGH_DEFAULT 0xEF

/* SAE J1939 parameter group numbers */
#define J1939_PGN_REQUEST 59904
#define J1939_PGN_ADDRESS_CLAIMED 60928
#define J1939_PGN_PROPRIETARY_A 61184
#define J1939_PGN_TP_CM 60416
#define J1939_PGN_TP_DT 60160

typedef struct
{

    uint32_t pgn;     /* parameter group number. */
    uint8_t *buf;     /* pointer to data. */
    uint16_t buf_len; /* size of data. */
    uint8_t dst;      /* destination of message. */
    uint8_t src;      /* source of message. */
    uint8_t pri;      /* priority of message. */

} j1939_t;

extern uint8_t j1939_sa;

extern uint8_t j1939_name[];

extern void J1939_Initialize( void );

extern uint8_t J1939_bip_tx_rate_max_get( void );

extern void J1939_bip_tx_rate_allowed_set( uint8_t rate );

/*
** This function translates a J1939 message into a CAN frame which
** is then buffered for transmission.
**
** Does not support multi-packet messages
**
** INPUT: msg - pointer to J1939 message
** RETURN: 0 - success
**         1 - failure
*/
uint8_t J1939_Tx( j1939_t *msg );

uint8_t J1939_AppTx( j1939_t *msg, uint8_t *status );

extern void J1939_Update( void );

/*
** This function checks for new incoming CAN packets
*/
extern void J1939_CheckRx( void );

//------------------------------//----------------------------------------------
// Callbacks from the J1939 stack.  Implement these in your application code

/**
 * Receives an incoming PGN
 *
 * Processes received J1939 message. This function is called by the J1939 module with a complete J1939 message and is
 * the intended location for the application layer to handle received J1939 messages. For multipacket messages, this
 * function isn?t called until all packets have been received and assembled into a complete J1939 message.
 */
void J1939_AppProcess( j1939_t *pMsg );

/**
 * Receives an incoming non-J1939 message (11-bit identifier)
 * 
 * @param pMsg
 */
void J1939_AppProcessStd( can_t *pMsg );
            
#if defined( J1939_TP )
/**
 * @brief      Returns the next candidate address for address claiming
 *
 * The J1939 will attempt to reserve a device ID on the CANBUS.
 * It uses this function to ask the application for the next valid address
 * it should try.
 * The application should return 255 if no more addresses are
 * valid.
 *
 * @return     next address to try
 */
uint8_t J1939_AppSaGet( void );

/**
 * @brief      Implement a random delay
 *
 */
void J1939_App_Delay( void );

/**
 * @brief Callback from J1939 stack when we have claimed an address
 *
 */
void J1939_App_Address_Claimed( uint8_t address );

/**
 * @brief      Test if a byte is a valid J1939 address
 *
 * @param      x     value to test
 *
 * @return     true if within the allowed range
 */
#define J1939_IsValidSourceAddress( x ) ( ( x >= J1939_SALOW_DEFAULT ) && ( x <= J1939_SAHIGH_DEFAULT ) )

#endif

#endif
