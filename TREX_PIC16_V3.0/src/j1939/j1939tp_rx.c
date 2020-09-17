/**
 * Lightweight receive version of j1939tp
 *
 * Supports only a single incoming, point to point, message at a time.
 * This version is useful for implementations that require a small code
 * footprint, like a bootloader, when the big messages are inbound only.
 *
 * It works in a similar way to j1939tp.c.  You would include it in your
 * project INSTEAD OF j1939tp.c
 */

#include "j1939.h"
#include "j1939cfg.h"
#include "j1939tp.h"

#ifdef J1939_TP

/* sanity check */
#if J1939CFG_TP_RX_BUF_SIZE > 1785
#error : J1939CFG_TP_RX_BUF_SIZE is to big
#endif

/* different transport protocol states */
#define J1939TP_STATE_UNUSED 0
#define J1939TP_STATE_SEND_BAM 1
#define J1939TP_STATE_SEND_RTS 2
#define J1939TP_STATE_SEND_CTS 3
#define J1939TP_STATE_SEND_DATA 4
#define J1939TP_STATE_SEND_ACK 5
#define J1939TP_STATE_SEND_ABORT 6
#define J1939TP_STATE_WAIT_CTS 7
#define J1939TP_STATE_WAIT_DATA 8
#define J1939TP_STATE_WAIT_ACK 9

/* type of TP msg (see spec -21) */
#define J1939TP_CTRL_BYTE_RTS 16
#define J1939TP_CTRL_BYTE_CTS 17
#define J1939TP_CTRL_BYTE_ACK 19
#define J1939TP_CTRL_BYTE_BAM 32
#define J1939TP_CTRL_BYTE_ABORT 255

/* reasons to send an abort msg (see spec -21) */
#define J1939TP_RSN_BUSY 1
#define J1939TP_RSN_RSRCS_FREED 2
#define J1939TP_RSN_TIMEOUT 3
#define J1939TP_RSN_ERROR 254

/* timeouts (see spec -21) */
#define J1939TP_TIMEOUT_BM ( 50u / J1939CFG_TICK_PERIOD )
#define J1939TP_TIMEOUT_TR ( 200u / J1939CFG_TICK_PERIOD )
#define J1939TP_TIMEOUT_TH ( 500u / J1939CFG_TICK_PERIOD )
#define J1939TP_TIMEOUT_T1 ( 750u / J1939CFG_TICK_PERIOD )
#define J1939TP_TIMEOUT_T2 ( 1250u / J1939CFG_TICK_PERIOD )
#define J1939TP_TIMEOUT_T3 ( 1250u / J1939CFG_TICK_PERIOD )
#define J1939TP_TIMEOUT_T4 ( 1050u / J1939CFG_TICK_PERIOD )

/* internal buffers used to hold
   large incoming and outgoing msgs */
typedef struct
{

    uint8_t state;
    uint32_t pgn;
    uint16_t msg_size;
    uint16_t timer;
    uint8_t num_packets;
    uint8_t next_seq;
    uint8_t cts_max;
    uint8_t cts_cnt;
    uint8_t rsn;
    uint8_t dst;
    uint8_t src;
    uint8_t buf[ J1939CFG_TP_RX_BUF_SIZE ];

} j1939tp_rx_t;

/* buffers */
j1939tp_rx_t j1939tp_rxbuf;

/*
** Initializes the J1939 transport protocol (called from j1939.c)
*/
void j1939tp_init( void )
{

    j1939tp_rxbuf.state = J1939TP_STATE_UNUSED;
}

/*
** This function receives an outgoing message, with more than 8 bytes of data,
** from the j1939 module and attempts to place it in a TX buffer.  *status
** will be set to J1939TP_INPROCESS during transmission, J1939TP_DONE for
** success, or J1939TP_FAILED for failure.
** RETURNS: 0 - successfully placed in TX buffer
**          1 - failed to be placed in TX buffer
*/
uint8_t j1939tp_tx( j1939_t *msg, uint8_t *status )
{
    return 1;
}

/*
** Transmits an abort CM msg.
** INPUTS: dst - msg desination
**         rsn - reason for abort
**         pgn - PGN
** RETURNS: 0 - success
**          1 - failure
*/
uint8_t j1939tp_tx_abort( uint8_t dst, uint8_t rsn, uint32_t pgn )
{

    j1939_t msg_tx;
    uint8_t buf[ 8 ];

    msg_tx.buf = buf;
    msg_tx.pgn = J1939_PGN_TP_CM;
    msg_tx.pri = 7;
    msg_tx.buf[ 0 ] = J1939TP_CTRL_BYTE_ABORT;
    msg_tx.buf[ 1 ] = rsn;
    msg_tx.buf[ 2 ] = 0xff;
    msg_tx.buf[ 3 ] = 0xff;
    msg_tx.buf[ 4 ] = 0xff;
    msg_tx.buf[ 5 ] = (uint8_t)pgn;
    msg_tx.buf[ 6 ] = ( uint8_t )( pgn >> 8 );
    msg_tx.buf[ 7 ] = ( uint8_t )( pgn >> 16 );
    msg_tx.buf_len = 8;
    msg_tx.dst = dst;

    return J1939_AppTx( &msg_tx, 0 );
}

/*
** Transmits a CTS CM msg.
** RETURNS: 0 - success
**          1 - failure
*/
uint8_t j1939tp_tx_cts( void )
{

    j1939_t msg_tx;
    uint8_t buf[ 8 ];

    msg_tx.buf = buf;
    msg_tx.pgn = J1939_PGN_TP_CM;
    msg_tx.pri = 7;
    msg_tx.buf[ 0 ] = J1939TP_CTRL_BYTE_CTS;
    msg_tx.buf[ 1 ] = j1939tp_rxbuf.cts_max;
    msg_tx.buf[ 2 ] = j1939tp_rxbuf.next_seq;
    msg_tx.buf[ 3 ] = 0xff;
    msg_tx.buf[ 4 ] = 0xff;
    msg_tx.buf[ 5 ] = ( uint8_t )( j1939tp_rxbuf.pgn );
    msg_tx.buf[ 6 ] = ( uint8_t )( j1939tp_rxbuf.pgn >> 8 );
    msg_tx.buf[ 7 ] = ( uint8_t )( j1939tp_rxbuf.pgn >> 16 );
    msg_tx.buf_len = 8;
    msg_tx.dst = j1939tp_rxbuf.src;

    return J1939_AppTx( &msg_tx, 0 );
}

/*
** Transmits an ack CM msg.
** INPUTS: index - index into TX buffer
** RETURNS: 0 - success
**          1 - failure
*/
uint8_t j1939tp_tx_ack( void )
{

    j1939_t msg_tx;
    uint8_t buf[ 8 ];

    msg_tx.buf = buf;
    msg_tx.pgn = J1939_PGN_TP_CM;
    msg_tx.pri = 7;

    msg_tx.buf[ 0 ] = J1939TP_CTRL_BYTE_ACK;
    msg_tx.buf[ 1 ] = (uint8_t)j1939tp_rxbuf.msg_size;
    msg_tx.buf[ 2 ] = j1939tp_rxbuf.msg_size >> 8;
    msg_tx.buf[ 3 ] = j1939tp_rxbuf.num_packets;
    msg_tx.buf[ 4 ] = 0xff;
    msg_tx.buf[ 5 ] = (uint8_t)j1939tp_rxbuf.pgn;
    msg_tx.buf[ 6 ] = ( uint8_t )( j1939tp_rxbuf.pgn >> 8 );
    msg_tx.buf[ 7 ] = ( uint8_t )( j1939tp_rxbuf.pgn >> 16 );
    msg_tx.buf_len = 8;
    msg_tx.dst = j1939tp_rxbuf.src;

    return J1939_AppTx( &msg_tx, 0 );
}

/*
** RX RTS/CTS state machine for processing received DT messages.  It checks the
** DT message for validity and copies the data into the matching RX buffer.
** INPUTS: index - index into RX buffer
**         msg - pointer to received message
*/
void j1939tp_state_rx_rtscts( j1939_t *msg )
{

    j1939_t n_msg;
    uint16_t cnt, b_rem, b_rcv, min;

    /* how many bytes have been received and are remaining? */
    b_rcv = ( j1939tp_rxbuf.next_seq - 1 ) * 7;
    b_rem = j1939tp_rxbuf.msg_size - b_rcv;

    /* the entire CAN frame might not be data, so we need to know
       how many bytes are suppose to be left */
    min = 7;
    if( b_rem < 7 )
        min = b_rem;

    if( j1939tp_rxbuf.state == J1939TP_STATE_WAIT_DATA )
    {

        if( ( j1939tp_rxbuf.next_seq == msg->buf[ 0 ] ) && ( ( b_rcv + min ) <= J1939CFG_TP_RX_BUF_SIZE ) &&
            ( ( b_rcv + b_rem ) == j1939tp_rxbuf.msg_size ) )
        {

            j1939tp_rxbuf.timer = J1939TP_TIMEOUT_T1;
            for( cnt = 0; cnt < min; cnt++ )
                j1939tp_rxbuf.buf[ b_rcv + cnt ] = msg->buf[ 1 + cnt ];

            /* was that the last packet? */
            if( j1939tp_rxbuf.next_seq++ >= j1939tp_rxbuf.num_packets )
            {

                n_msg.pgn = j1939tp_rxbuf.pgn;
                n_msg.buf = j1939tp_rxbuf.buf;
                n_msg.buf_len = j1939tp_rxbuf.msg_size;
                n_msg.dst = j1939tp_rxbuf.dst;
                n_msg.src = j1939tp_rxbuf.src;
#if 0
                J1939_AppProcess( &n_msg );
                j1939tp_rxbuf.timer = J1939TP_TIMEOUT_TR;
                j1939tp_rxbuf.state = J1939TP_STATE_SEND_ACK;

#else
                // try to send the ack right away, before we process the message.
                // otherwise our response to the message might be sent before the
                // ack.  This can be confusing to somebody on the other end of the transaction.
                // If we can't send it now, go to the SEND_ACK so we will send it later.
                // In that case we just have to hope that the sending node can properly handle
                // the sequence of packets.
                if( j1939tp_tx_ack() == 0 )
                {
                    j1939tp_rxbuf.state = J1939TP_STATE_UNUSED;
                }
                else
                {
                    j1939tp_rxbuf.timer = J1939TP_TIMEOUT_TR;
                    j1939tp_rxbuf.state = J1939TP_STATE_SEND_ACK;
                }

                J1939_AppProcess( &n_msg );
#endif
            }
            else if( ++j1939tp_rxbuf.cts_cnt >= j1939tp_rxbuf.cts_max )
            {

                j1939tp_rxbuf.timer = J1939TP_TIMEOUT_TR;
                j1939tp_rxbuf.state = J1939TP_STATE_SEND_CTS;
            }
        }
        else
        {

            j1939tp_rxbuf.rsn = J1939TP_RSN_RSRCS_FREED;
            j1939tp_rxbuf.state = J1939TP_STATE_SEND_ABORT;
        }
    }

    return;
}

/*
** RX RTS/CTS state machine for processing update events.  Once a message has been
** placed into a RX buffer this handles sending out the CTS/ACK/ABORT messages.
** INPUTS: index - index into RX buffer
*/
void j1939tp_update_rx_rtscts( void )
{

    /* see if any stage has timed out */
    if( j1939tp_rxbuf.timer )
        if( --j1939tp_rxbuf.timer == 0 )
        {
            j1939tp_rxbuf.rsn = J1939TP_RSN_TIMEOUT;
            j1939tp_rxbuf.state = J1939TP_STATE_SEND_ABORT;
        }

    switch( j1939tp_rxbuf.state )
    {

        case J1939TP_STATE_SEND_CTS:
        {
            if( j1939tp_tx_cts() == 0 )
            {

                j1939tp_rxbuf.cts_cnt = 0;
                j1939tp_rxbuf.timer = J1939TP_TIMEOUT_T3;
                j1939tp_rxbuf.state = J1939TP_STATE_WAIT_DATA;
            }
            break;
        }

        case J1939TP_STATE_SEND_ACK:
        {
            if( j1939tp_tx_ack() == 0 )
                j1939tp_rxbuf.state = J1939TP_STATE_UNUSED;
            break;
        }

        case J1939TP_STATE_SEND_ABORT:
        {
            if( j1939tp_tx_abort( j1939tp_rxbuf.src, j1939tp_rxbuf.rsn, j1939tp_rxbuf.pgn ) == 0 )
                j1939tp_rxbuf.state = J1939TP_STATE_UNUSED;
            break;
        }
    }
}

/*
** This function receives CM (BAM/RTS/CTS/ACK/ABORT) messages directly
** from j1939.c and decides what to do with them.
** INPUT: msg - pointer to received CM message
*/
void j1939tp_post_cm( j1939_t *msg )
{
    uint32_t pgn;
    uint16_t msg_size;

    /* all cm messages have the pgn in the same location */
    pgn = msg->buf[ 5 ] | ( (uint32_t)msg->buf[ 6 ] << 8 ) | ( (uint32_t)msg->buf[ 7 ] << 16 );

    /* msg_size is in RTS, ACK, and BAM so make sure it's only used there */
    msg_size = msg->buf[ 1 ] | ( (uint16_t)msg->buf[ 2 ] << 8 );

    switch( msg->buf[ 0 ] )
    {

        case J1939TP_CTRL_BYTE_RTS:
        {
            /* someone wants to establish a rts/cts connection.
               it must not have a global dst, which means it has our dst */
            if( msg->dst != J1939_ADDR_GLOBAL )
            {

                /* will the msg fit and does it have a correct size? */
                if( ( msg_size <= J1939CFG_TP_RX_BUF_SIZE ) && ( msg_size <= (uint16_t)msg->buf[ 3 ] * 7 ) )
                {

                    j1939tp_rxbuf.state = J1939TP_STATE_SEND_CTS;
                    j1939tp_rxbuf.pgn = pgn;
                    j1939tp_rxbuf.msg_size = msg_size;
                    j1939tp_rxbuf.num_packets = msg->buf[ 3 ];
                    j1939tp_rxbuf.next_seq = 1;
                    j1939tp_rxbuf.timer = J1939TP_TIMEOUT_TR;
                    j1939tp_rxbuf.cts_max = msg->buf[ 4 ];
                    j1939tp_rxbuf.dst = msg->dst;
                    j1939tp_rxbuf.src = msg->src;
                }
                else
                {
                    /* since we can't, we should send an abort */
                    (void)j1939tp_tx_abort( msg->src, J1939TP_RSN_BUSY, pgn );
                }
            }
            break;
        }

        case J1939TP_CTRL_BYTE_ABORT:
        {

            if( ( j1939tp_rxbuf.state != J1939TP_STATE_UNUSED ) && ( j1939tp_rxbuf.dst == msg->dst ) &&
                ( j1939tp_rxbuf.src == msg->src ) )
            {

                j1939tp_rxbuf.state = J1939TP_STATE_UNUSED;
            }

            break;
        }
    }

    return;
}

/*
** This function receives DT messages directly from j1939.c
** and decides what to do with them.
** INPUT: msg - pointer to received DT message
*/
void j1939tp_post_dt( j1939_t *msg )
{

    if( ( j1939tp_rxbuf.state != J1939TP_STATE_UNUSED ) && ( j1939tp_rxbuf.dst == msg->dst ) &&
        ( j1939tp_rxbuf.src == msg->src ) )
    {

        j1939tp_state_rx_rtscts( msg );
    }
}

/*
** This function is called from j1939.c at the system tick rate and is
** responsible for updating the state of any in-progress transfer
*/
void j1939tp_update( void )
{
    if( j1939tp_rxbuf.state != J1939TP_STATE_UNUSED )
    {
        j1939tp_update_rx_rtscts();
    }

    return;
}

#endif
