/* 
 * File:   battery_charger.h
 * Author: US1DWYMAN
 *
 * Created on April 20, 2020, 11:26 AM
 */

#ifndef BATTERY_CHARGER_H
#define	BATTERY_CHARGER_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif
/*
Bits    Variable               Value    Description         Notes
====    ===============        =====    ===========         ==================
15-12   Charge Cycle Type        0      No Active Cycle
                                 1      Charge
                                 2      Maintenance
                                 3-15   (Reserved)
11-8    Charge Indication        0      Inactive
                                 1      < 80%
                                 2      > 80%
                                 3      Finishing
                                 4      Complete
                                 5      Resting
                                 6      Equalize
                                 7      Power Supply Mode
                                 8-15   (Reserved)
7-6     Override Status          0      Disabled
                                 1      Enabled
                                 2-3    (Reserved)
5       Charger Status           0      Disabled            Same as bit 0 in 6001h
                                 1      Enabled
4       AC Connection Status     0      No AC Detected
                                 1      AC Detected         Actual Voltage at 2200h
3       Charger Derating Status  0      Charger is not derating
                                 1      Charger is derating output
2       Charger Hardware 
        Shutdown Status          0      Charger is running normally
                                 1      Charger hardware has shut down
1-0     (Reserved)

*/
///////////////////////////////////////////////////////////////////
//                   TPD01
//////////////////////////////////////////////////////////////////
/*
 
 BYTES     DATA       OBJECT                       VALUE
 =====     =====      ======================       ======
 0-1       00 00      2002h Charging Current       0
 2-3       87 0C      2101h Battery Voltage        87 0C -> 0C87h = 3207d
                                                   3207/256 = 12.5V
 4-5                  2006 Extended Charge Status
 6-7       00 00      zero filling
 
 */


// this is derived from the 
//"Delta-Q IC-series CANOpen Simplified v10 FINAL.pdf" document
// CAN OBJECT 2006h 
// CHARGE CYCLE TYPE
#define NO_ACTIVE_CYCLE               0b00     //0
#define CHARGE                        0b01     //1
#define MAINTENANCE                   0b10     //2
//      values 3-15                   reserved
// CHARGE INDICATION
#define INACTIVE                      0b0000   //0
#define LESS_THAN_80_PERCENCT         0b0001   //1
#define GREATER_THAN_80_PERCENT       0b0010   //2
#define FINISHING                     0b0011   //3
#define COMPLETE                      0b0100   //4
#define RESTING                       0b0101   //5
#define EQUALIZE                      0b0110   //6
#define POWER_SUPPLY_MODE             0b0111   //7
//      values 8-15                   reserved
// OVERRIDE STATUS
#define OVERRIDE_DISABLED             0b00     //0
#define OVERRIDE_ENABLED              0b01     //1
//      values 2-3                    reserved
// CHARGER STATUS
#define CHARGER_DISABLED              0b0       //0
#define CHARGER_ENABLED               0b1       //1
// AC CONNECTION STATUS
#define NO_AC_DETECTED                0b0       //0
#define AC_DETECTED                   0b1       //1
// CHARGER DERATING STATUS
#define CHARGER_IS_NOT_DERATING       0b0       //0
#define CHARGER_IS_DERATING_OUTPUT    0b1       //1
// CHARGER HARDWARE SHUTDOWN STATUS
#define CHARGER_IS_RUNNING_NORMALLY   0b0       //0
#define CHARGER_HARDWARE_IS_SHUT_DOWN 0b1       //1


/*
 CAN_charger_message
 The purpose of the battery charger task is to monitor the various battery 
 parameters(voltage,current,temperature, alarm states) 
 */

#define CHARGER_MESSAGE_CAN_ID    0x123
// CHARGER_STATUS
#define CHARGE_COMPLETE           0b000
#define CHARGE_LESS_THAN_80       0b001
#define CHARGE_GREATER_THAN_80    0b010
#define CHARGE_NO_AC_POWER        0b011
#define CHARGE_INACTIVE           0b100
//      values 5-7               reserved
// CHARGER_FAULTS
#define NO_FAULT                  0b000
#define CHARGER_DISABLED_         0b001
#define CHARGER_OVERTEMPARATURE   0b010
#define CHARGER_BATTERY_FAULT     0b011
//      values 4-7               reserved
// reserved    

#define TPD01                    0x018A     // can message ID from charger
#define TPD02                    0x028A     // can message ID from charger
#define TPD03                    0x038A     // can message ID from charger
#define TPD04                    0x048A

#define CHARGER_DISCONNECTED true
#define CHARGER_CONNECTED false

typedef union {
    uint16_t word;
    struct
    {
        unsigned int charge_cycle_type                : 4;
        unsigned int charge_indication                : 4;
        unsigned int override_status                  : 2;
        unsigned int charger_status                   : 1;
        unsigned int ac_connection_status             : 1;
        unsigned int charger_derating_status          : 1;
        unsigned int charger_hardware_shutdown_status : 1;
        unsigned int reseved : 2;
    } bits;
} CAN_2006_object_t;


///////////////////////////////////////////////////////////////////
//                   TPD03
//////////////////////////////////////////////////////////////////

 /*
 BYTES       OBJECT                                      
 =====       ==================================          
 0-3         2060h   current displayed error/fault        
 4-5         2200h   ac voltage
 6           6080h   state of charge 0-100%
 7           2ffah   zero filling
 */




typedef union {
    uint8_t byte;
    struct
    {
        unsigned int charge_status                    : 3;
        unsigned int charge_fault                     : 3;
        unsigned int reserved                         : 2;
    } bits;
} CAN_charger_message_t;

typedef union {
    struct {
        uint8_t high_byte;
        uint8_t low_byte;
    } object;
    uint16_t value;
} charger_can_object;

typedef union {
    struct {
        uint8_t fourth_byte;
        uint8_t third_byte;
        uint8_t second_byte;
        uint8_t first_byte;
    } object;
    uint32_t valueLong;
} charger_can_object_long;


/****************************************************************************
  Function:
     void battery_test_code( void )

  Description:  This is a test function that sends charger can messages to be  
   parsed.  This simulates the messages from the Charger over the can bus.
  Precondition:
   none
  Parameters:
    none
  Return Values:
    none
  Remarks:

 ****************************************************************************/
void battery_test_code(void);

void AC_power_detection_test(void);
void charger_can_bus_heartbeat_task(void);


/****************************************************************************
  Function:
     void handle_charger_can_messages( uCAN_MSG * )

  Description:  This function parses messages from the charger. Periodically the
   charger sends out TPD01, TPD02 and TPD03 can object messages. This function
   should be called every time a message is received from the charger.  It will
   parse the messages and extract:
              charging current
              battery voltage
              battery state of charge 0-100%
              external AC power connection 
  Precondition:
   none
  Parameters:
   IN: can message pointer
  Return Values:
    none
  Remarks:

 ****************************************************************************/
bool handle_charger_can_messages(uCAN_MSG *);


/****************************************************************************
  Function:
      get_battery_charge_current( void )

  Description:  This function returns the charge current in milli-amps.
  Precondition:
   periodic can messages should be received containing the charger current value 
  Parameters:
   none
  Return Values:
   charge current milli-amperes
  Remarks:
  this value is updated every second

 ****************************************************************************/
uint16_t get_battery_charge_current(void);

/****************************************************************************
  Function:
     uint16_t get_battery_charge_voltage( void )

  Description:  This function returns the charge voltage in milli-volts.
  Precondition:
   periodic can messages should be received containing the charger voltage value 
  Parameters:
   none
  Return Values:
   charge voltage in millivolts
  Remarks:
  this value is updated every second

 ****************************************************************************/
uint16_t get_battery_charge_voltage(void);


/****************************************************************************
  Function:
      get_ac_power_status( void )

  Description:  This function returns the status of the external AC power
  Precondition:
   periodic can messages should be received containing the AC power status 
  Parameters:
   none
  Return Values:
   true if AC power is applied, false otherwise
  Remarks:
   this value is updated every second

 ****************************************************************************/
bool get_ac_power_status(void);

/****************************************************************************
  Function:
      get_extended_charge_status( void )

  Description:  This function returns the status of the charge
  Precondition:
   periodic can messages should be received containing the charge status status 
  Parameters:
   none
  Return Values:
   charge status
  Remarks:
   this value is updated every second

 ****************************************************************************/
uint16_t get_extended_charge_status(void);


/****************************************************************************
  Function:
     uint8_t get_battery_state_of_charge( void )

  Description:  This function returns the charge percentage between 0-100%.
  Precondition:
   periodic can messages should be received containing the charger state of
 * charge value 
  Parameters:
   none
  Return Values:
   charge state of charge in percent
  Remarks:
  this value is updated every second

 ****************************************************************************/
uint8_t get_battery_state_of_charge(void);

/****************************************************************************
  Function:
     uint32_t get_record_Ah_returned( void )

  Description:  This function returns the Ah returned to the battery as
 * reported by the charger periodic can messages should be received containing
 * the Ah returned of
 * charge value 
  Parameters:
   none
  Return Values:
  Ah retured 
  Remarks:
  this value is updated every second

 ****************************************************************************/
uint32_t get_record_Ah_returned(void);

/****************************************************************************
  Function:
     uint16_t get_record_Wh_returned( void )

  Description:  This function returns the Wh returned to the battery as
 * reported by the charger periodic can messages should be received containing
 * the Wh returned of
 * charge value 
  Parameters:
   none
  Return Values:
  Wh retured 
  Remarks:
  this value is updated every second

 ****************************************************************************/
uint16_t get_record_Wh_returned(void);


/****************************************************************************
  Function:
     uint16_t get_record_elapsed_time( void )
  Description:  This function returns the elapsed time as
 * reported by the charger periodic can messages should be received containing
 * elapsed time
  Parameters:
   none
  Return Values:
  elapsed time
  Remarks:
  this value is updated every second

 ****************************************************************************/
uint16_t get_record_elapsed_time(void);

/****************************************************************************
  Function:
     uint32_t get_record_current_displayed_fault( void )

  Description:  This function returns the current displayed fault as
 * reported by the charger periodic can messages should be received containing
 * the current displayed fault
 * charge value 
  Parameters:
   none
  Return Values:
  the current displayed fault
  Remarks:
  this value is updated every second

 ****************************************************************************/
uint32_t get_record_current_displayed_fault(void);


/****************************************************************************
  Function:
     uint16_t get_record_AC_voltage( void )
  Description:  This function returns the AC voltage as
 * reported by the charger periodic can messages should be received containing
 * elapsed time
  Parameters:
   none
  Return Values:
  elapsed time
  Remarks:
  this value is updated every second

 ****************************************************************************/
uint16_t get_record_AC_voltage(void);

void charger_detection_task(void);
// true charger is disconnected, false otherwise
bool get_charger_status(void);

#endif	/* BATTERY_CHARGER_H */

///////////////////////////////////////////////////////////////////////////////


/*
 
 
 
 Object 2060h: Current Displayed Error/Fault
Sub-index 0:
Scale: NA
Unit: NA
Read: Highest Priority Currently Active Alarm/Fault

This object holds the highest priority active alarm/fault originally signalled by the Emergency Object. Faults have higher priority than alarms.

Fault Format:
 - Most-Significant 16 bits: 	0xXX00 from CiA301 standard 
	- XX = 50 - Hardware Error - Generic
 - Least-Significant 16 bits: 	0xC000h | (DQT Fault ID)
Alarm Format:
 - Most-Significant 16 bits: 	0xXX00 from CiA301 standard 
	- XX = 90 - External Error - Generic
	- XX = 81 - Communications - Generic
	- XX = 60 - Software - Generic
	- XX = 10 - Generic Error
 - Least-Significant 16 bits: 	0x8000h | (DQT Alarm ID) 
High Priority Alarm Format
 - Most-Significant 16 bits: 	0xXX00 from CiA301 standard 
	- XX = 90 - External Error - Generic
	- XX = 81 - Communications - Generic
	- XX = 60 - Software - Generic
	- XX = 10 -  Generic Error
 - Least-Significant 16 bits: 	0x0000h | (DQT High Priority Alarm ID) 

NOTE: Due to an idiosyncrasy in Delta-Q's CANopen stack, the 16-bit Delta-Q alarm/fault codes are stored in little-endian order in the EMCY message and big-endian order in the 1003h object (see JIRA reference SSP-4514).

Charger Faults:
0x500001C0: Fault LLC Excessive Leakage
0x500002C0: Fault PFC Excessive Leakage
0x500003C0: Fault PFC Boost Failure
0x500004C0: Fault Offset Calibration Failure
0x500005C0: Fault DC relay
0x500006C0: Fault DC-DC battery current difference
0x500007C0: Fault Hardware component failure
0x50000aC0: Feedback signal from interlock relay not responding as expected
0x50000bC0: Vref or 5V Supply voltage is out of valid range. Likely HW failure.


other faults with the following signature:
(fault code) & 0xFFFFC00 == 0x1000C000

Charger Alarms:
0x90000180: Alarm Battery Voltage Invalid High
0x90000280: Alarm Battery Voltage Invalid Low
0x90000380: Alarm Charge Timeout
0x90000480: Alarm Battery Not Qualified
0x90000680: Alarm Battery Temperature is shorted, and is required by ALG
0x90000780: Alarm AH limit exceeded
0x90000880: Alarm Battery Temperature Out of Range
0x90000C80: Alarm Battery Reverse Polarity
0x90000D80: Alarm Battery Not Taking Current
0x90001580: Alarm Battery Voltage High during Charge
0x90001680: Alarm Battery Voltage Low during Charge
0x90001780: Alarm AC Voltage High
0x90001980: Alarm AC Voltage Low
0x90001D80: Alarm Communication CAN-bus error
0x90001E80: Alarm battery module error
0x90002480: Alarm Battery Temperature is not installed, but required by ALG
0x90002680: Alarm Fan error
0x90002B80: Alarm PDO CRC Error
0x90002C80: Alarm PDO Sequence Count Error
0x90002D80: Alarm Battery Disconnected Error
0x90002F80: Alarm Battery Platform Overvoltage
0x90003080: Alarm Multiple Parallel Masters on CAN bus
0x90003180: Alarm Out of resources. Typically too many parallel slaves on bus
0x90003280: Alarm Unexpected parallel charging messages. Incompatible chargers on the bus.
0x90003380: Alarm No Parallel Master device on the bus
0x90003480: Alarm Missing Parallel Slaves on the bus
0x90003580: Alarm Communications with Slave device unexpectedly lost
0x90003680: Slave has reverse polarity
0x90003780: Alarm Parallel Slave DC connection issue
0x90003880: Alarm Unspecified Parallel Slave fault
0x90003D80: BMS sensed current differ from charger sensed current
0x90003E80: BMS sensed voltage differ from charger sensed voltage
0x90003F80: The master has detected an incompatible slave on the bus
0x90004080: Cannot claim J1939 device address 
0x90004180: Another device cannot claim its J1939 device address 
0x90004280: sensed voltage higher than expected
0x90004380: Alarm Communication CAN-bus error
0x90004580: EVSE not compatible
0x90004680: Lock failed to lock or unlock
0x90004780: EVSE not powered
0x90004880: EVSE is reporting a fault
0x90004980: Fault detected with the EVSE
0x90004A80: Fault detected internal vehicle wiring of the Proximity circuit
0x90004B80: Short circuit on the Proximity circuit
0x90004C80: Proximity voltage invalid or erratic
0x90004D80: Control Pilot PWM Duty Cycle has an illegal value

0x81302080: Alarm Communication heartbeat/PDO lost error
0x81303980: Alarm BMS Response Timeout
0x81303A80: Alarm Network Management (NM) Down
0x81303C80: Alarm Never Received Monitored Message
0x81304480: Alarm Communication heartbeat/PDO lost error

0x82102E80: Alarm Invalid PDO Length

0x60002580: Alarm CAN Download Failed
0x60002A80: Alarm CAN configuration error

0x10000980: Suspicious Battery Temp Reading from AI 
0x10000A80: Suspicious Battery Connections 
0x10000E80: Alarm Number of Cells Invalid
0x10000F80: Alarm End of Charge Voltage Not in Algorithm
0x10001480: Alarm Active Algorithm Not Set
0x10001880: Alarm Charger Initialization Failure
0x10001C80: Alarm Incompatible Algorithm
0x10001F80: Alarm Half VCC Out of Range
0x10002180: Alarm Charger Target Voltage Configuration Too High
0x10002280: Alarm Battery capacity configuration is not set
0x10002380: Alarm Charger target voltage configuration too low
0x10002780: Alarm Button stuck down
0x10002880: Alarm Fan supply output voltage low
0x10002980: Alarm Software internal
0x10004E80: Alarm 12V Supply Out of Range



other alarms with the following signature:
(Alarm code) & 0xFFFFC00 == 0x10008000


Charger High Priority Alarms:
0x90000200: High Priority Alarm BMS Status Error
0x90000300: High Priority Alarm BMS Not Established
0x90000400: High Priority Alarm BMS Pre-charge Timeout


other high priority alarms with the following signature:
(Alarm code) & 0xFFFFC00 == 0x10000000
OBJECT DESCRIPTION
INDEX	2060
Name	Current Displayed Error/Fault
Object Code	Variable
Data Type	UNSIGNED32
ENTRY DESCRIPTION
Access	RO
PDO Mapping	Yes
Default Value	0x0
 
 
 
 
 
 
 
 
 
 
 
 
 */