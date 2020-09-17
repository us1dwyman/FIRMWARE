/** \mainpage API Documentation
 *
 * \section intro_sec Introduction
 *
 * The T-Rex Vehicle Control Module (VCM) coordinates I/O, system management, and communication with various
 * accessories.
 * This API package allows access to the various hardware features in order to facilitate
 * building custom implementations of the VCM for specific end products.
 *
 * For details on the hardware functions and interfaces, please refer to Control Solutions document DOC0004063A
 *
 * \section folders Project Structure
 * The project folder contains the following:
 * 		- /dist - executable (hex) files are stored here when the software is compiled and linked.
 * 		- /doc - contains these files and other documentation
 * 		- /scripts - contains utility scripts that may be useful to software developers.
 * 		- /src - source code files
 * 			- /src/api - source code for the API as documented here
 * 			- /src/apitest - source code that demonstrates the use of the API.  It is assumed that software packages
 * for a specfic product/platform will replace these files, which are provided for reference as examples of how to use
 * the API.
 * 		- /TREX.VCM.X - MPLABX project folder.  Open this Project in the MPLABX IDE
 * 			- /TREX.VCM.X/mcc_generated_files - drivers for hardware functions (part of API)
 * 		- /TREX.VCM_Boot.X project contains the project and code for the bootloader.
 * 	\n
 * 	@subsection projectconfig Project Configurations
 * 	The MPLABX project files contain at least the following selectable configurations:
 * 	 	- /TREX.VCM.X
 * 	 		- Upgrade: builds an image that is suitable for (field) upgrade.  It is loaded via the bootloader and will
 *not run on its own (it depends on the bootloader being present in the target hardware).  At the end of the build
 *process, a checksum is added and the file is stored in /dist/VCM.Upgrade.hex
 * 	 		- Standalone: builds an image that runs without the bootloader.  This is useful for debugging, since it can
 *be loaded via the ICSP port WITHOUT using the bootloader.  However, it CANNOT be used for field upgrade, since it is
 *not compatible with the bootloader.
 * 	 		- Factory: builds a single combined image that includes the bootloader and upgrade.  This allows one-step
 * programming in the factory.  Notice that if the bootloader changes, it must be built BEFORE this project
 * configuration.
 * 	 	- /TREX.VCM_Boot.X
 * 	 		- Production: builds the production image of the bootloader
 *
 * @subsection gettingstarted Getting Started
 *
 * 	After unzipping the project, you will be able to build the executables using MPLABX and XC16.
 * 	Refer to doc/api_changelog.txt to confirm the specific compiler and MPLABX versions that the software was
 *	validated with.
 *
 * 	- Ensure you have MPLABX, XC16, and NodeJS (NodeJS.org) installed.
 * 	- Open the TREX.VCM.X project and select the Upgrade configuration. Select 'Clean and Build' to build the
 *	  project and confirm a successful outcome.
 *	- In a command terminal, change to the /scripts folder and enter the command 'npm install'.  This will install
 *	  the utility programs.
 *	- Connect the VCM to your computer.
 *	- Use the loader.js script to load the file (use node loader.js -h to see command help).  By default, the
 *	  loader will use the Upgrade image in the /dist folder, and use the 'last' serial port detected on the
 *	  computer. You can override these choices with the command line arguments.
 *	- If the loader does not proceed past 'Checking communication' it means the board is either not connected, or
 *	  the bootloader is not active.
 *	- Currently, in order to enter the bootloader, you must power the board up with the key switch in the 'off'
 *	  position.  Note that if the USB cable is connected, the processor is powered from the USB and cannot be
 *	  turned off.
 *
 *
 * \section api_sec API Sections
 *
 * \ref taskloop "Startup and Task Loop"
 *
 * \ref callback "Callbacks"
 *
 * \ref liftactuator "Deck Actuator"
 *
 * \ref brake "Brake Control"
 *
 * \ref discreteoutputs "Discrete Outputs"
 *
 * \ref discreteinputs "Discrete Inputs"
 *
 * \ref analoginputs "Analog Inputs"
 *
 * \ref canbus "CAN bus Communication"
 *
 * \ref canmotor "CAN bus Motor interface"
 *
 * \ref imu "Inertial Measurement Unit"
 *
 * \ref nvram "Non-Volatile RAM"
 *
 * \ref spi "Serial Peripheral Interface"
 *
 * \ref diagnostic "Diagnostic Port (USB)"
 *
 *
 */

/**
 * @file api.h
 * @brief Main API interface definition
 *
 * @copyright Copyright (c) 2018 Control Solutions LLC.  All Rights Reserved.
 */

#ifndef _API_H_
#define _API_H_

// clang-format off
// Don't let clang alphabetize the includes, since we need app.h
// to be first
#include "app.h"
// clang-format on

#include "api_actuator.h"
#include "api_can.h"
#include "api_dinput.h"
#include "api_spi.h"
#include "api_imu.h"
#include "api_canmotor.h"
#include "eeprom.h"
#include "hardware.h"
#include "timer.h"
#include <time.h>



API_OpPresence_e get_operator_status(void);

//------------------------------//----------------------------------------------
// Definitions

// Defines the release version of the API.  Refer to semver.org
#define API_VERSION_MAJOR 3
#define API_VERSION_MINOR 0
#define API_VERSION_PATCH 2

/**
 * define system states
 */
typedef enum
{
    API_SYSTEM_FAULT = 0,
    API_SYSTEM_IDLE,
    API_SYSTEM_ACTIVE
} ApiSystemState_e;

// 80,000,000 Hz Fosc / 22,000 Hz = 3636 PWM tics
#define PWM_PERIOD_FOR_22KHZ 3636

/**
 * define Brake Subsystem constants
 */
#define BRAKE_PWM_PERIOD PWM_PERIOD_FOR_22KHZ

/**
 * define Horn Subsystem constants
 */
#define HORN_PWM_PERIOD PWM_PERIOD_FOR_22KHZ

//------------------------------//----------------------------------------------
// Global variables
extern uint16_t API_direction;


// temporary place holder for system status indication
extern ApiSystemState_e API_SystemState;

//------------------------------//----------------------------------------------
// Public functions

/** \defgroup taskloop Startup and Task Loop
 * Provides basic control of the API
 *
 */



void set_operator_status(API_OpPresence_e);


/** \ingroup taskloop
 * Power-up initialization of the API
 *
 * This function should be called once at powerup.  It performs
 * necessary hardware and state initialization for the API
 */
void API_Initialize( void );

/** \ingroup taskloop
 * Maintenance of the API
 *
 * This function must be called periodically by the application.
 * It maintains various states and timing functions.  The
 * interval at which you poll the API is not critical; Most
 * of the API event notifications happen in this loop, so
 * neglecting this call will mean you do not receive event notifications as
 * expected.  It is suggested that you call this function every 10ms.
 */
void API_Tasks( void );

/** \ingroup taskloop
 * Put the system in a low power, non-responsive state
 *
 * API functions go into a low power, non-operational state.
 * Call API_Wakeup() to restore operation
 *
 */
void API_Sleep( void );

/** \ingroup taskloop
 * Restores system operation after an API_Sleep() call
 *
 *
 */
void API_Wakeup( void );

/** \ingroup taskloop
 * Reset the controller processor
 *
 * Immediately restarts the controller processor.  Does not coordinate
 * with any external devices, etc - just reboots.
 */
void API_Reset( void );

/** \ingroup taskloop
 * Output API state to debug console
 *
 * Prints various state information to the debug console
 */
void API_Dump( void );

/** \ingroup taskloop
 * Debug command parsing function
 *
 * This function is called by (for example) a serial terminal
 * implementation when the user enters a debug API command.
 * The pString should point to the first character of the argument.
 *
 * @param      pToken The string to be parsed
 */
void API_Debug( char *pToken );

/** \ingroup taskloop
 * Report a MAJOR Fault
 *
 * This function is called to report a MAJOR fault (set a fault bit).
 * A MAJOR fault is one that should basically shut down the system and prevent
 * operation.
 *
 * @param      code the bitmapped fault code
 */
void API_SetMajorFault( uint32_t code );

/** \ingroup taskloop
 * Clears a MAJOR Fault
 *
 * This function is called to clear a MAJOR fault (the fault is no longer present).
 *
 * @param      code the bitmapped fault code
 */
void API_ClearMajorFault( uint32_t code );

/** \ingroup taskloop
 * Report a MINOR fault
 *
 * This function is called to report a MINOR fault (set a fault bit).
 * A MINOR fault is reported, but does not prevent the system from being operated.
 *
 * @param      code the bitmapped fault code
 */
void API_SetMinorFault( uint32_t code );

/** \ingroup taskloop
 * Clears a MINOR Fault
 *
 * This function is called to clear a MINOR fault (the fault is no longer present).
 *
 * @param      code the bitmapped fault code
 */
void API_ClearMinorFault( uint32_t code );

/** \ingroup taskloop
 * Returns the bitmapped major fault code value
 *
 * @return      fault(s).  Zero if none
 */
uint32_t API_GetMajorFaults( void );

/** \ingroup taskloop
 * Clears a MINOR Fault
 *
 * This function is called to clear a MINOR fault (the fault is no longer present).
 *
 * @param      fault(s).  Zero if none
 */
uint32_t API_GetMinorFaults( void );

/** \defgroup callback Callbacks
 *
 * Callbacks are functions implemented by the Application, which are
 * called by the API when certain events occur.
 *
 * If these functions are not provided, the application will fail to link
 * so you must implement (at least empty) callbacks.
 *
 */

/** \ingroup callback
 * Signals a change on the interlock input
 *
 * The API will call this function when change is detected on the
 * interlock line.
 * @param[in] bActive true if the input is in its active state
 */
void APICB_InterlockChanged( bool bActive );

/** \ingroup callback
 * Signals a change on the oppres inputs
 *
 * The API will call this function when change is detected on the
 * oppres lines.
 * @param[in] opPres reflects state of oppres inputs
 */
void APICB_OpPresChanged( API_OpPresence_e opPres );

/** \ingroup callback
 * Handles an external interrupt 2 event.
 *
 * Overrides the (weak) version in ext_int.c.
 * Up to the user to implement functionality.
 * This is plumbed to the Bag/Flow signal on processor RC9.
 *
 * For now we just bump a local variable.
 */
void EX_INT2_CallBack( void );

/** \ingroup callback
 * Signals a change on the Brk Rel input
 *
 * The API will call this function when change is detected on the
 * Brk Rel line.
 * @param[in] bActive true if the input is in its active state
 */
void APICB_BrkRelChanged( bool bActive );

/** \ingroup callback
 * Signals a change on the GPIN01 input
 *
 * The API will call this function when change is detected on the
 * GPIN01 line.
 * @param[in] bActive true if the input is in its active state
 */
void APICB_GpIn01Changed( bool bActive );

/** \ingroup callback
 * Signals a change on the Dmod Fault input
 *
 * The API will call this function when change is detected on the
 * Dmod Fault line.
 * @param[in] bActive true if the input is in its active state
 */
void APICB_DmodFaultChanged( bool bActive );

/** \ingroup callback
 * Signals a change on the e-stop input
 *
 * The API will call this function when change is detected on the
 * e-stop line.
 * @param[in] bActive true if the input is in its active state
 */
void APICB_EstopChanged( bool bActive );

/** \ingroup callback
 * Signals a change on the keyswitch input
 *
 * The API will call this function when change is detected on the
 * keyswitch line.
 * @param[in] bActive true if the input is in its active state
 */
void APICB_KeySwChanged( bool bActive );

/** \ingroup callback
 * Signals a change on the CS LED input
 *
 * The API will call this function when change is detected on the
 * CS LED line.
 * @param[in] bActive true if the input is in its active state
 */
void APICB_CsLedChanged( bool bActive );

/** \ingroup callback
 * Signals a change in actuator over current has been detected
 *
 * The API will call this function when a change is detected in
 * actuator overcurrent.
 *
 * @param[in] bActive true if the overcurrent status has changed
 */
void APICB_ActuatorOverCurrentOccurred( bool bActive );

/** \ingroup callback
 * Enternal interrupt detected on INT_IN
 *
 * The API will call this function when an interrupt signal is detected.
 * NOTE: this function is called on an interrupt context.
 *
 */
void APICB_IntInOccurred( void );

/** \defgroup brake Brake Control
 * Brake Drive Outputs: J9 BRK+/BRK-
 *
 * The BRK+ and BRK- pins provide a means to release an electromagnetic brake.
 *
 */

/** @ingroup    brake
 *
 * Releases the EM brake (activates brake release output to allow vehicle motion)
 */
void API_ReleaseBrake( void );

/** @ingroup    brake
 *
 * Sets the EM brake (deactivates brake release output to prevent vehicle motion)
 */
void API_SetBrake( void );

/** @ingroup    brake
 *
 * Sets up the behavior of the brake
 *
 * @param[in]  pullPower  Percentage (0-100) of PWM to apply to the brake when releasing
 */
void API_ConfigureBrake( uint8_t pullPower );

/** \defgroup discreteoutputs Discrete Outputs
 * This section provides control of various discrete outputs.
 *
 */

/** \ingroup discreteoutputs
 * Controls the Horn Drive Output
 *
 * @param[in]  bOn true if horn output should be enabled.
 * @param[in] dutyCycle sets the PWM duty cycle in percent (0..100)
 */
void API_HornEnable( bool bOn, uint8_t dutyCycle );

/** \ingroup discreteoutputs
 * Sets the U5_ENABLE Output high
 *
 */
void API_U5EnableSet( void );

/** \ingroup discreteoutputs
 * Clears the U5_ENABLE Output
 *
 */
void API_U5EnableClr( void );

/** \ingroup discreteoutputs
 * Sets the OE_n_uC Output high
 *
 */
void API_OeNuCSet( void );

/** \ingroup discreteoutputs
 * Clears the OE_n_uC Output
 *
 */
void API_OeNuCClr( void );

/** \ingroup discreteoutputs
 * Configures the ACC_OUT1 output on connector J4 to forward
 *
 */
void API_AccOut1Fwd( void );

/** \ingroup discreteoutputs
 * Configures the ACC_OUT1 output on connector J4 to reverse
 *
 */
void API_AccOut1Rev( void );

/** \ingroup discreteoutputs
 * Configures the ACC_OUT1 output on connector J4 to standby
 *
 */
void API_AccOut1Standby( void );

/** \ingroup discreteoutputs
 * Configures the ACC_OUT1 output on connector J4 to brake
 *
 */
void API_AccOut1Brake( void );

/** \ingroup discreteoutputs
 * Controls the ACC_OUT2 output on connector J4
 *
 * @param[in]  bEnabled   boolean true if the output should be enabled
 */
void API_AccOut2Enable( bool bEnabled );

/** \ingroup discreteoutputs
 * Controls the ACC_OUT3 output on connector J4
 *
 * @param[in]  bEnabled   boolean true if the output should be enabled
 */
void API_AccOut3Enable( bool bEnabled );

/** \ingroup discreteoutputs
 * Controls the ACC_En_Out output on connector J4
 *
 * @param[in]  bEnabled   boolean true if the output should be enabled
 */
void API_AccEnOutEnable( bool bEnabled );

/** \ingroup discreteoutputs
 * Controls the U5_Contactor_Enable_uC output of the micro
 *
 * @param[in]  bEnabled   boolean true if the output should be enabled
 */
void API_U5_Contactor_Enable_uC( bool bEnabled );


#define API_ADC_MAX 1023

/** \defgroup    analoginputs Analog Inputs
 * Provides an interface to read analog inputs.
 *
 * Analog Inputs are sampled periodically, and the value made available via
 * these interface functions.  A global flag (ADC_hasNewSample) is set when a
 * new ADC value is avaiable.
 * A/D values are read using 10-bit accuracy, and therefore have a range of
 * 0-API_ADC_MAX corresponding to 0-3.3V at the processor input pin.  At the board edge,
 * these signals may be 0-5V - check the specification for details
 *
 */

/** \ingroup analoginputs
 * Read Analog Input 0
 *
 * Returns the most recent value of the ANIN0 input.
 *
 * @return value in the range of 0 to API_ADC_MAX
 */
uint16_t API_ReadAin0( void );

/** \ingroup analoginputs
 * Read Analog Input 1
 *
 * Returns the most recent value of the ANIN1 input.
 *
 * @return value in the range of 0 to API_ADC_MAX
 */
uint16_t API_ReadAin1( void );

/** \ingroup analoginputs
 * Read ACT Isense
 *
 * Returns the most recent value of the input.
 *
 * @return value in the range of 0 to API_ADC_MAX
 */
uint16_t API_ReadActIsense( void );

/** \ingroup analoginputs
 * Read BattLevel
 *
 * Returns the most recent value of the input.
 *
 * @return value in the range of 0 to API_ADC_MAX
 */
uint16_t API_ReadBattLevel( void );

/** \ingroup analoginputs
 * Read Throttle
 *
 * Returns the most recent value of the input.
 *
 * @return value in the range of 0 to API_ADC_MAX
 */
uint16_t API_ReadThrottle( void );

/** \ingroup analoginputs
 * Read BRK Sense
 *
 * Returns the most recent value of the input.
 *
 * @return value in the range of 0 to API_ADC_MAX
 */
uint16_t API_ReadBrkSense( void );

/** \ingroup analoginputs
 * Read VControlLevel
 *
 * Returns the most recent value of the input.
 *
 * @return value in the range of 0 to API_ADC_MAX
 */
uint16_t API_ReadVControlLevel( void );

/** \defgroup    nvram Non-Volatile RAM
 * Provides an interface to read and write the non-volatile RAM
 *
 * The NVRAM stores persistent data.  It is written synchronously;
 * this function will block until the read or write is completed.
 *
 * the macro TOTAL_NVM_BYTES defines the total number of bytes available in the
 * eeprom chip
 */

/** \ingroup nvram
 * Read NVRAM into a buffer
 *
 * The buffer will only be filled with valid bytes (If the requested read
 * goes past the end of NVRAM, the read will stop but no error will be
 * raised).
 *
 * @param[in] address NVRAM address to start reading
 * @param[in] pBuf buffer to receive the data
 * @param[in] length number of bytes to read
 */
#define API_ReadNvram( address, pBuf, length ) EEPROM_Read( pBuf, address, length )

/** \ingroup nvram
 * Write NVRAM from a buffer
 *
 * Writes to the device from the specified buffer
 *
 * @param[in] address NVRAM address to start writing
 * @param[in] pBuf buffer containing the data
 * @param[in] length number of bytes to write
 */
#define API_WriteNvram( address, pBuf, length ) EEPROM_Write( pBuf, address, length, 0 )

/** \defgroup    diagnostic Diagnostic Port Interface
 * Provides an interface to the Diagnostic (USB) interface
 *
 * The diagnostic UART interface is a buffered serial interface.
 * Refer to the example console implementation in console.c for
 * details.
 */

#endif
