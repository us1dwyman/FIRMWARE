/*
 * File:   hour_meters.c
 * Author: us1jbernath
 *
 * Created on August 31, 2020, 10:13 AM
 * 
 * This code is used to control the hour meters.  There are 16 possible hour 
 * meter each is 32 bit long.  This code will read and write the hour meter 
 * values to the EEPROM, turn each on and off and return the value for each
 * hour meter.  
 */


#include "api.h"
#include "EEPROM.h"
#include "hour_meters.h"
#include "battery.h"
#include "HMI_interface.h"
#include "lockout.h"
#include "battery_charger.h"

#define TRACE( x ) printf x;

/*Used to masks lockout conditions that can trigger a EEPROM write*/
#define LOCKOUT_FLAG_MASK   0x0007;

/************************************************************************
 * FUNCTION: EEPROM_Initialize();
 *
 * DESCRIPTION: initializes the EEPROM and write all values to RAM
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/

void EEPROM_Initialize(void)
{
    Hour_Meters_Read_EEPROM();
    Battery_Read_EEPROM();
    
    
}

/************************************************************************
 * FUNCTION: EEPROM_Task();
 *
 * DESCRIPTION: This is the EEPROM task.  This checks for any of the triggers 
 * to write the EEPROM.
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/

void EEPROM_Task(void)
{
    static uint16_t last_lockout_state;
    static uint16_t current_lockout_state;
    static bool last_charger_AC_detect;
    static bool current_charger_AC_detect;
    static uint8_t last_operating_mode;
    static uint8_t current_operating_mode;
    
    current_charger_AC_detect = get_ac_power_status();
    current_lockout_state = Lockout_Get_Flags() & LOCKOUT_FLAG_MASK;
    current_operating_mode = get_operating_mode();
    if((current_lockout_state != last_lockout_state) 
            || (current_charger_AC_detect != last_charger_AC_detect)
            || (current_operating_mode != last_operating_mode))
    {
        Hour_Meters_Write_EEPROM();
        Battery_Write_EEPROM();
        
        last_charger_AC_detect = get_ac_power_status();
        last_lockout_state = Lockout_Get_Flags() & LOCKOUT_FLAG_MASK;
        last_operating_mode = get_operating_mode();
    }     
}
