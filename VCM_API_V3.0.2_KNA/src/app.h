/**
 * Application global definitions
 *
 * You probably want this file included in all .c files
 */

#ifndef _APP_H_
#define _APP_H_

#ifndef __XC16__
// This is here because my linter (CLANG) does not like the __prog__ type
// qualifier
// so it is defined to empty if not compiling for the XC compiler
#define __prog__
#endif

// Define this to turn off assert() code generation for production
//#define NDEBUG

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <xc.h>

#ifndef NDEBUG
#define assert( ignore ) ( (void)0 )
#else
#undef assert
#undef __myassert
#define assert( expression ) ( (void)( ( expression ) ? 0 : ( __myassert( #expression, __FILE__, __LINE__ ), 0 ) ) )

#define __myassert( expression, file, line )                                                                           \
    printf( "Failed assertion `%s' at line %d of `%s'.", expression, line, file )
#endif

// define this to enable debug outputs like the TRACE macro.
// You can undefine it globally, or at the top of a specific
// module (after the includes) to disable debug output for that module.
#define ENABLE_DEBUG

#if defined( ENABLE_DEBUG )
// a debug print macro that can be #defined-out to remove the
// debug messages from the code
#define TRACE( x ) printf x;

#else
#define TRACE( x )
#endif

//------------------------------//----------------------------------------------
// Analog to digital converter Setup
// See adc.h for details.  These should not be changed arbitrarily as they
// affect not only memory usage but the effective sampling delay of the ADC

// Maximum channels to sample (the highest ADC channel you can enable is
// (ADC_MAX_CHANNELS-1))
#define ADC_MAX_CHANNELS 32

// The number of samples to take per channel, before processing them.
#define ADC_SAMPLES_PER_CHANNEL 8

//------------------------------//----------------------------------------------
// CANBUS Setup

// Define if hardware has CAN2 support (and you want to use it)
#define CAN2_ENABLE

// bus speed (125,250,500,1000)
#define CAN2_BITRATE 500

// The number of outgoing pending messages we can queue
// power of 2, max size restricted, see cbuf.h
#define CAN2_txMessageQ_SIZE 16U
#define CAN2_rxMessageQ_SIZE 8U

// defines the maximum number of outgoing mailboxes for CAN2
// there is no particular limit to these
#define CAN2_TX_MAILBOXES 8U

// These are message IDs that are used on the CANBUS for testing.  They
// are arbitrary values and assume there is something else on the bus
// that understands these test messages.  Note: hardware message filters
// must be set to allow these messages if you want to use them!
#define CAN_PING 0x110UL
#define CAN_PONG 0x111UL

// CAN Message identifiers (PGNs)
#define CAN_PGN_PROPRIETARY_A 0xEF00
#define CAN_PGN_PHOENIX_STATUS0 0xFF00
#define CAN_PGN_PHOENIX_STATUS1 0xFF01
#define CAN_PGN_PHOENIX_STATUS2 0xFF02
#define CAN_PGN_PHOENIX_STATUS3 0xFF03
#define CAN_PGN_PHOENIX_STATUS4 0xFF04
#define CAN_PGN_HMI_BUTTONS 0xFF10U
#define CAN_PGN_VCM_LEDS 0xFF20U
#define CAN_PGN_VCM_STATUS1 0xFF21U
#define CAN_PGN_VCM_IMU 0xFF22U
#define CAN_PGN_VCM_STATUS3 0xFF23U
#define CAN_IO_DEBUG_PGN 0xFFB1U

// Device addresses used on the CANBUS
#define CANADDR_VCM 0x80UL
#define CANADDR_HMI 0x81UL
#define CANADDR_TRACTION 0x82UL
#define CANADDR_BRUSH 0x83UL
#define CANADDR_VACUUM 0x84UL
#define CANADDR_SIDEBROOM 0x85UL
#define CANADDR_GLOBAL 0xFFUL

// Bitmapped buttons as represented in the CAN_PGN_HMI_BUTTONS message
#define HMI_BUTTON_POWER 0x0100
#define HMI_BUTTON_ACCESSORY 0x0200
#define HMI_BUTTON_VACUUM 0x0400
#define HMI_BUTTON_BRUSH 0x0800
#define HMI_BUTTON_FORWARD 0x1000
#define HMI_BUTTON_REVERSE 0x2000
#define HMI_BUTTON_SWITCH06 0x4000
#define HMI_BUTTON_SWITCH07 0x8000

// Bitmapped LEDs as represented in the CAN_PGN_VCM_STATUS0 message
#define HMI_LED_POWER 0x01000000
#define HMI_LED_ACCESSORY 0x02000000
#define HMI_LED_VACUUM 0x04000000
#define HMI_LED_BRUSH 0x08000000
#define HMI_LED_FORWARD 0x10000000
#define HMI_LED_REVERSE 0x20000000
#define HMI_LED_LED06 0x40000000
#define HMI_LED_LED07 0x80000000
#define HMI_LED_LED08 0x00010000
#define HMI_LED_LED09 0x00020000
#define HMI_LED_LED10 0x00040000
#define HMI_LED_LED11 0x00080000
#define HMI_LED_LED12 0x00100000
#define HMI_LED_LED13 0x00200000
#define HMI_LED_LED14 0x00400000
#define HMI_LED_LED15 0x00800000
#define HMI_LED_LED16 0x00000100
#define HMI_LED_LED17 0x00000200
#define HMI_LED_LED18 0x00000400
#define HMI_LED_LED19 0x00000800
#define HMI_LED_LED20 0x00001000
#define HMI_LED_LED21 0x00002000
#define HMI_LED_LED22 0x00004000
#define HMI_LED_LED23 0x00008000
#define HMI_LED_LED24 0x00000001

//------------------------------//----------------------------------------------
// Actuator Setup
//
// define a usable starting PWM Duty Cycle, if too low,
// produces no movement
#define ACT_PWM_START_DUTY_CYCLE_PERCENT 15

// ADC count which triggers overcurrent indication
#define ACTUATOR_OVERCURRENT_ADC 850

//------------------------------//----------------------------------------------
// Traction motor brake Setup

#define PWM_PERIOD_FOR_10KHZ 8000
#define PWM_BRAKE_MAX_DUTY_CYCLE_PERCENT 100

//------------------------------//----------------------------------------------
// Fault codes

// these are bitmapped fault codes, that can be reported using the API_Fault
// module.  There are 32 possible fault bits (this is not an enumeration, they
// are bitmapped so that more than one fault can be active at a time)
#define FAULT_IMU_HW 0x00000001
#define FAULT_CAN2_HW 0x00000002

#endif
