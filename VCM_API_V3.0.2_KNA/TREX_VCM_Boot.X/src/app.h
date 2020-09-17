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
#define NDEBUG

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <xc.h>

#ifdef NDEBUG
#define assert( ignore ) ( (void)0 )
#else
#undef assert
#undef __myassert
#define assert( expression ) ( (void)( ( expression ) ? 0 : ( __myassert( #expression, __FILE__, __LINE__ ), 0 ) ) )

#define __myassert( expression, file, line )                                                                           \
    printf( "Failed assertion `%s' at line %d of `%s'.", expression, line, file )
#endif

#ifdef NDEBUG
#define TRACE( x )

#else
// a debug print macro that can be #defined-out to remove the
// debug messages from the code
#define TRACE( x ) printf x;

#endif

//------------------------------//----------------------------------------------
// Versioning

#define VERSION_PRODUCT 1

// Semantic Software version (see https://semver.org)
// This is important, especially the major version, as it tells
// the host software how it shoud communicate with us.
#define VERSION_MAJOR 4
#define VERSION_MINOR 3

// Device addresses used on the CANBUS
#define CANADDR_VCM 0x80
#define CANADDR_HMI 0x81
#define CANADDR_TRACTION 0x82
#define CANADDR_BRUSH 0x83
#define CANADDR_VACUUM 0x84
#define CANADDR_SIDEBROOM 0x85

// Bitmapped buttons as represented in the CAN_PGN_HMI_BUTTONS message
#define HMI_BUTTON_POWER 0x0100
#define HMI_BUTTON_ACCESSORY 0x0200
#define HMI_BUTTON_VACUUM 0x0400
#define HMI_BUTTON_BRUSH 0x0800
#define HMI_BUTTON_FORWARD 0x1000
#define HMI_BUTTON_REVERSE 0x2000
#define HMI_BUTTON_SWITCH06 0x4000
#define HMI_BUTTON_SWITCH07 0x8000

//------------------------------//----------------------------------------------
// Command enumerations
#define MODBUS_COMMAND 0x47

#endif
