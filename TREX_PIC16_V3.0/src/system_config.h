/**
 * Hardware definitions
 *
 *
 */

#ifndef __SYSTEM_CONFIG_H
#define __SYSTEM_CONFIG_H
#include <stdint.h>
#include <xc.h>

//------------------------------//----------------------------------------------
// Pins

// IO Pin definitions
#define OPIN_SPARE LATAbits.LATA5
#define OPIN_FAULT LATBbits.LATB0

#define IPIN_FAULT PORTBbits.RB0
#define TRIS_FAULT TRISBbits.TRISB0
#define OPIN_FAULT LATBbits.LATB0

#define IPIN_ENABLE PORTCbits.RC0
#define IPIN_CAN_INT PORTCbits.RC1

#define OPIN_CAN_CS LATBbits.LATB4
#define OPIN_CAN_STBY LATBbits.LATB5

#define CAN_CSOFF() OPIN_CAN_CS = 1
#define CAN_CSON() OPIN_CAN_CS = 0

#define OPIN_HORN LATAbits.LATA6
#define OPIN_3V_ENABLE LATAbits.LATA7

// True if a fault is detected on the FAULT pin
#define SYSTEM_IsFaulted() ( IPIN_FAULT == 0 )

// Set fault pin to the faulted state (drive low))
#define SYSTEM_SetFault()                                                                                              \
    OPIN_FAULT = 0;                                                                                                    \
    TRIS_FAULT = 0;

// Clear fault pin (make it an input))
#define SYSTEM_ClearFault() TRIS_FAULT = 1;

//------------------------------//----------------------------------------------
// System timer

// counter that determines when we sample a row of switches
// 500us*4 = 2ms
// So each of the 5 rows gets sampled every 10ms
#define SW_CNT 4

// Number of microseconds in a system ticker
#define TIMER_TICK_PERIOD 1000U

// number of ticks for common intervals
#define TIMER_50MS ( 50000U / TIMER_TICK_PERIOD )
#define TIMER_10MS ( 10000U / TIMER_TICK_PERIOD )
#define TIMER_1MS ( 1000U / TIMER_TICK_PERIOD )

// counter that determines when we send the CAN switch status message
#define SEND_SWITCHES_CNT TIMER_50MS

//typedef uint8_t TIMER_TICK;
typedef uint32_t TIMER_TICK;  

// keeps track of timer ticks, for the purpose of measuring time durations
extern TIMER_TICK SYSTEM_tick;

#define TIMER_Now() SYSTEM_tick
#define TIMER_Since( start ) ( TIMER_TICK )( TIMER_Now() - start )

//------------------------------//----------------------------------------------
// LED Outputs

#define LED_ROW1_PORTA 0B00000001
#define LED_ROW2_PORTA 0B00000010
#define LED_ROW3_PORTA 0B00000100
#define LED_ROW4_PORTA 0B00001000
#define LED_ROW5_PORTA 0B00010000

// the hardware port controlling the LED row outputs
#define LED_ROW_LAT LATA

// defines the bits we care about in the LED row port register
#define LED_ROW_MASK 0B00011111U

// the hardware port that controls the columns
#define LED_COLUMN_LAT LATF

// defines the bits we care about in the LED column port register
#define LED_COLUMN_MASK 0B00011111U

// the hardware port that controls the 'extra' LEDs (25,26,27)
// These are 0=ON, 1=OFF
#define LED_EXTRA_LAT LATF

// defines the bits we care about in the extra LED port register
#define LED_EXTRA_MASK 0B11100000U

#define OPIN_OUT1 LATFbits.LATF5
#define OPIN_OUT2 LATFbits.LATF6
#define OPIN_OUT3 LATFbits.LATF7

//------------------------------//----------------------------------------------
// Switch inputs

#define SW_ROW1_PORTD 0B00000001
#define SW_ROW2_PORTD 0B00000010
#define SW_ROW3_PORTD 0B00000100
#define SW_ROW4_PORTD 0B00001000
#define SW_ROW5_PORTD 0B00010000

// Hardware port that controls the row outputs
#define SWITCH_ROW_LAT LATD

// the bits related to switch rows in the switch_row_lat
#define SWITCH_ROW_MASK 0b00011111U

// The hardware port that has the matrix swtich inputs
#define SWITCH_COLUMN_PORT PORTC

// defines the bits we care about in the switch column port
#define SWITCH_COLUMN_MASK 0b00111101U

// the hardware port connected to the extra switches
#define SWITCH_EXTRA_PORT PORTD

// defines the bits we care about in the extra switch port
#define SWITCH_EXTRA_MASK 0b11100000U

//------------------------------//----------------------------------------------
// SPI

#define SPI1_ENABLE

//------------------------------//----------------------------------------------
// Debugging

// Debug printf macro not available on this platform
#define TRACE( x )

// Debug assert macro not available on this platform
#define assert( x )

#endif
