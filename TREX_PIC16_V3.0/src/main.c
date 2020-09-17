/**
 * Application entry point
 *
 * Contains the initialization and main loop for the application.
 *      - Monitors CANBUS for incoming messages
 *      - Sends periodic status messages
 *      - Updates the LED matrix
 *      - Scans and debounces switch inputs
 *      - Sends and receives serial messages from PIC24
 *
 * The board specification document contains a description of how the
 * switches and LEDs work.
 *
 * Copyright (c) 2019 Control Solutions LLC.  All Rights Reserved.
 */

#pragma warning disable 1510
// advisory: (1510) non-reentrant function "__Read" appears in multiple call graphs and has been duplicated by the compiler

#pragma warning disable 520
//../src/j1939/j1939tp_rx2.c:382: warning: (520) function "_j1939tp_post_dt" is never called

#include "app.h"
#include "can.h"
#include "mcp2515_can.h"
#include "system.h"
#include "EUSART.h"
#include "LED.h"
#include "buttons.h"
#include "hal.h"
#include <stdint.h>
#include <string.h>
#include <xc.h>
#include "sandbox.h"

//------------------------------//----------------------------------------------
// Definitions

// structure to hold state information used for debouncing switches
typedef struct
{
    // the current (debounced) state of the switches
    uint8_t debounced;

    // state history
    uint8_t sample0;
    uint8_t sample1;

} SwitchState_t;

// The CANID for the incoming LED message
#define CANID_SWITCHES ( 0x10000000 ) + ( CAN_PGN_HMI_BUTTONS << 8 ) + CANADDR_MINE + CAN_EFF_FLAG


//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Local variables

#define LED_MATRIX_ROWS 5
#define SWITCH_MATRIX_ROWS 5
#define SWITCH_MATRIX_COLUMNS 4

// These are bit masks that are used to activate a specific row of switches or leds while scanning
static uint8_t SW_ROW_PORTD[ 5 ] = {SW_ROW1_PORTD, SW_ROW2_PORTD, SW_ROW3_PORTD, SW_ROW4_PORTD, SW_ROW5_PORTD};
static uint8_t LED_ROW_PORTA[ 5 ] = {LED_ROW1_PORTA, LED_ROW2_PORTA, LED_ROW3_PORTA, LED_ROW4_PORTA, LED_ROW5_PORTA};

// Stores the desired state of the LED outputs.
// Each byte represents one 'row' of LEDs
static uint8_t LED_COL[ LED_MATRIX_ROWS ] = {0, 0, 0, 0, 0};

// keep track of how many LED rows we are supposed to scan
// this can be modified via CAN message.  If not all rows are needed,
// this effectively increases the duty cycle (brightness) of the LEDs
static uint8_t _numLedRows = LED_MATRIX_ROWS;

// Keep track of debounced state of all switches
static SwitchState_t _switchStates[ SWITCH_MATRIX_ROWS ];
static SwitchState_t _extraSwitchState;
static SwitchState_t _faultInputState;
static SwitchState_t _enableInputState;

/**
 * Configuration for the SPI (CAN2) driver
 */
static const MCP2515_Config_t _canConfig = {
    // masks.  Note each mask corresponds to a specific receive buffer.
    // RB0 is more flexible, in that if a second message arrives for RB0 before
    // it has been read, it will roll the message over into RB1 rather than lose it.
    {
        CAN_MASK29( 0X0000FF00UL ),  // Mask for RB0, match any message directed at us
        CAN_MASK29( 0X00FFFFFFUL )   // Mask for RB1, match exact ID, ignore priority
    },

    // filters
    {
        // first 2 filters apply to RB0
        CAN_FILTER29( 0X00008000UL ),
        //CAN_FILTER29( 0X00008700UL ),
        CAN_FILTER29( 0X00005500UL ) ,

        // next 4 filters apply to RB1
          CAN_FILTER29( 0X00000080UL << 8U ),  // filter message to VCM
        //CAN_FILTER29( CANADDR_VCM << 8U ),  // filter message to VCM
       
        CAN_FILTER29( 0X00FF2080UL ),
        CAN_FILTER29( 0X00555596UL ),
        CAN_FILTER29( 0X005555B6UL ),
        //0UL,                                // not used
        //0UL                                 // not used
    }
};

void button_task(can_t *);
void led_task(void);
void update_graphics(void);
void handle_can_messages_from_VCU(void);
void transmit_can_message_task(can_t *);
void power_down_task(void);
void direction_message_task(void);
void test_clock_rollover(void);

/**
 * @brief      Init at powerup or after an 'ENABLE'/keyswitch wakeup
 */
static void _Wakeup( void )
{
    memset( _switchStates, 0, sizeof( SwitchState_t ) * SWITCH_MATRIX_ROWS );
    memset( &_extraSwitchState, 0, sizeof( SwitchState_t ) );
    memset( &_faultInputState, 0, sizeof( SwitchState_t ) );
    memset( &_enableInputState, 0, sizeof( SwitchState_t ) );
    memset( LED_COL, 0, LED_MATRIX_ROWS );
//    _testMode = 0;
    // Init serial port for HMI communication
    EUSART_Initialize();
    // restore hardware to full function
    SYSTEM_Wakeup();
}


// Store the message we broadcast out containing switch states
static can_t can_send_message = {
    CANID_SWITCHES,     // id
    0,                  // pad1
    8,                  // buf_len
    {0, 0, 0, 0, 0, 0, 0, 0}  // buf
};

static can_t test_send_message = {
    CANID_SWITCHES,     // id
    0,                  // pad1
    8,                  // buf_len
    {0, 0, 0, 0, 0, 0, 0, 0}  // buf
};


// TODO
/*
 
 1.  speed icon shows one bar when the speed is zero                                                           no screen for no bars
 2   send pulse direction when the direction button is pressed                                                 done
 3.  drive the direction led off when there is a fault condition. can I use the operator on the machine bit?   done
 4.  direction message backwards                                                                               done
 
 * 
 * 
 * vcu sends operator on the machine message this allows the direction switch to be active
 * hmi responds to direction switch sends out a pulsed direction, turns on appropriate led
 * 
 
 
 
 */



/*******************************************************************************
 *
 ******************************************************************************/
void main()
{
    unsigned int LED_row = 0;

    // Init hardware
    SYSTEM_Initialize();
    Buttons_Initialize();
    SYSTEM_ClearFault();
    CAN_Initialize( &_canConfig );
    _Wakeup();
    EUSART_Initialize();
    LED_Initialize();
    SYSTEM_EnableInterrupts();

    while( 1 )
    {
        //test_clock_rollover();
        button_task(&can_send_message);  // scan buttons, capture the horn button
        
        led_task();                      // update led matrix
        update_graphics();               // send uart messages to the pic24
        handle_can_messages_from_VCU();  // 
        transmit_can_message_task(&can_send_message);
        direction_message_task();        // forces the direction message to act like a momentary switch
        power_down_task();               // power down if enable line goes hi
        CLRWDT();                        // must execute within 33 milliseconds    
    }
    return;
}


void test_clock_rollover(void){
    static TIMER_TICK rollover_tick = 0;
    static uint8_t counter = 0;
    if( TIMER_Since( rollover_tick ) >= 500 ){
        rollover_tick = TIMER_Now();
        counter++;
    }
}


void button_task(can_t *can_msg){
    static TIMER_TICK button_tick = 0;
    
    if( TIMER_Since( button_tick ) >= 10 ){
        button_tick = TIMER_Now();
        if(SW_row >= 5){
            SW_row = 0;
            }  
        Buttons_Update(SW_row,can_msg);  // capture horn button
        SWITCH_ROW_LAT = ( SWITCH_ROW_LAT & ~SWITCH_ROW_MASK );
        // activate the next row of switches
        SWITCH_ROW_LAT |= SW_ROW_ARRAY[ SW_row ];
        SW_row++; 
    }
}

void led_task(void){
    static uint8_t LED_row = 0;
    static TIMER_TICK led_tick = 0;
    
    if( TIMER_Since( led_tick ) >= 2 ){
        led_tick = TIMER_Now();
        if(LED_row >= 5)
        {
            LED_row = 0;
        }
        Update_LEDs(LED_row);
        LED_row++;
    }
}


void handle_can_messages_from_VCU(void){
    static TIMER_TICK can_tick = 0;
    
    if( TIMER_Since( can_tick ) >= 45 ){
        can_tick = TIMER_Now();
        Receive_CAN_Message();  
    }
}

typedef enum
{
  NO_DIRECTION = 0,
  DIRECTION,              
}DIRECTION_STATES;


void direction_message_task(void){
    static TIMER_TICK direction_tick = 0;
    static uint8_t direction_state = NO_DIRECTION;  // no direction
    
    if( TIMER_Since( direction_tick ) >= 1000 ){
        direction_tick = TIMER_Now();
        switch(direction_state){
            case NO_DIRECTION:
                if(machine_direction != 0x00)
                    direction_state = DIRECTION; 
                break;
            case DIRECTION:
                machine_direction = 0x00;
                direction_state = NO_DIRECTION;
                break;
            default:
                direction_state = NO_DIRECTION;
                break;
        } 
    }
}

void power_down_task(void){
    static TIMER_TICK power_down_tick = 0;
    
    if( TIMER_Since( power_down_tick ) >= 100 ){
        power_down_tick = TIMER_Now();
        while( 1 == IPIN_ENABLE )
            {
                SYSTEM_Sleep();
                // here the processor woke up, after we-don't-know-how-long
                // so reset switches and LEDs to the powerup defaults
                _Wakeup();
                // interrupt woke us up...
            }
    }
}
   


void update_graphics(void){
    
    if( EUSART_send_data_flag)
        {
            EUSART_Master_Request(); 
            EUSART_send_data_flag = 0;
        }
        /*the display board has replied and the message is ready to be read*/
    if( EUSART_receive_flag )  // waiting for 27 bytes....bug in the code other messages not received correctly  drw
        {                          // unless all messages are 27 bytes
            EUSART_Receive();
            EUSART_receive_flag = 0;
        }
}
    
void transmit_can_message_task(can_t *can_msg){
    
    static TIMER_TICK sendSwitchesTimer = 0;
    
    // If it's time to send the CAN message, do it
    if( TIMER_Since( sendSwitchesTimer ) >= SEND_SWITCHES_CNT )
    {
        sendSwitchesTimer = TIMER_Now();
        EUSART_TO_CAN(can_msg);
        CAN_Tx(&can_send_message);
    }
}





/////////////////////// delete ///////////////////////////////////////////////




