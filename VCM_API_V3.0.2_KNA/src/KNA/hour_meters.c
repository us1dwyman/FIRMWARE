/*
 * File:   hour_meters.c
 * Author: us1jbernath
 *
 * Created on April 24, 2020, 9:10 AM
 * 
 * This code is used to control the hour meters.  There are 16 possible hour 
 * meter each is 32 bit long.  This code will read and write the hour meter 
 * values to the EEPROM, turn each on and off and return the value for each
 * hour meter.  
 */


#include "api.h"
#include "hour_meters.h"
#include "timer.h"
#include "EEPROM.h"
#include "t_rex_operation.h"

//#define TRACE( x ) printf x;

/*this is the sample rate for the hour meter.  1000 is 1 second*/
#define HOUR_METER_UPDATE_TIMER_MS 1000

/*This is the index for the PWM setting for the DMC*/
#define INDEX_PWM_VOLTAGE   2

/*This word is used to track which hour meter is currently on and counting
 writing to this word will turn the hour meters on and off*/
static uint16_t hour_meters_on_off_states;

/*this array contains the values for all of the hour meters.  This value will
 be read from and written to the EEPROM, and updated when a motor is running.*/
static uint32_t hour_meters_array[HOUR_METERS_NUMBER_OF_HOUR_METERS];

void Hour_Meter_Check_Motor_States(void);
void Hour_Meter_Increment_Motor(void);

/************************************************************************
 * FUNCTION: Hour_Meters_Initialize();
 *
 * DESCRIPTION: initializes the hour meter
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/

void Hour_Meters_Initialize(void)
{
    /*turn off all our meters*/
    hour_meters_on_off_states = 0;
}

/************************************************************************
 * FUNCTION: Hour_Meters_Task();
 *
 * DESCRIPTION: This function is called by the by the main and
 * is used to count up any of the
 *  hour meters that are currently on. 
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/

void Hour_Meters_Task(void)
{
    uint16_t i;

    static MS_TICK _updateHourMeterStart = 0;
    
    MS_TICK _updateHourMeterElapsed;

    _updateHourMeterElapsed = TIMER_Since( _updateHourMeterStart );
    if( _updateHourMeterElapsed >= HOUR_METER_UPDATE_TIMER_MS )
    {
        Hour_Meter_Increment_Motor();
        _updateHourMeterStart = TIMER_Now();
        Hour_Meters_Write_EEPROM();
    }
}

/************************************************************************
 * FUNCTION: Hour_Meters_Return();
 *
 * DESCRIPTION: hour_meters_bit - this is the hour meter that will be returned.  
 * #defines for the hour meters are in hour_meters.h.  
 * ie. HOUR_METERS_TRACTION_MOTOR_BIT is for the traction motor hour meter.
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/

uint32_t Hour_Meters_Return (uint8_t hour_meters_index)
{
    return hour_meters_array[hour_meters_index];
}

/************************************************************************
 * FUNCTION: Hour_Meter_Increment_Motor();
 *
 * DESCRIPTION: Used to increment the motor hour meter if it is running
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/

void Hour_Meter_Increment_Motor(void)
{
    if (API_CanMotor_Return_Status(INDEX_TRACTION,0,INDEX_PWM_VOLTAGE))
    {
        hour_meters_array[INDEX_TRACTION] += 1;
    }
    if (API_CanMotor_Return_Status(INDEX_BRUSH,0,INDEX_PWM_VOLTAGE))
    {
        ++hour_meters_array[INDEX_BRUSH];
    }    
    if (API_CanMotor_Return_Status(INDEX_VACUUM,0,INDEX_PWM_VOLTAGE))
    {
        ++hour_meters_array[INDEX_VACUUM];
    }
    if (API_CanMotor_Return_Status(INDEX_SIDEBROOM,0,INDEX_PWM_VOLTAGE))
    {
        ++hour_meters_array[INDEX_SIDEBROOM];
    }   
}

/************************************************************************
 * FUNCTION: Hour_Meters_Read_EEPROM();
 *
 * DESCRIPTION: Reads hour meters EEPROM data and stores it in RAM
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/

void Hour_Meters_Read_EEPROM(void)
{  
    uint8_t hour_meters_byte_array [HOUR_METERS_NUMBER_OF_HOUR_METERS * HOUR_METERS_BYTE_LENGTH_OF_HOUR_METERS];
    uint16_t i;
    
    API_ReadNvram( HOUR_METERS_EEPROM_PAGE_ADDRESS * PAGE_LENGTH
            , hour_meters_byte_array
            , HOUR_METERS_NUMBER_OF_HOUR_METERS * HOUR_METERS_BYTE_LENGTH_OF_HOUR_METERS );
    
    for (i = 0; i < HOUR_METERS_NUMBER_OF_HOUR_METERS * HOUR_METERS_BYTE_LENGTH_OF_HOUR_METERS; i++)
    {
        hour_meters_array[i/HOUR_METERS_BYTE_LENGTH_OF_HOUR_METERS] = hour_meters_array[i/HOUR_METERS_BYTE_LENGTH_OF_HOUR_METERS] << BYTE_LENGTH;
        hour_meters_array[i/HOUR_METERS_BYTE_LENGTH_OF_HOUR_METERS] += hour_meters_byte_array[i];
    }
        //reset the values to 0 during the first read.
    for (i = 0; i < HOUR_METERS_NUMBER_OF_HOUR_METERS; i++) 
    {
        if (hour_meters_array[i] == 0xFFFFFFFF)
        {
            hour_meters_array[i] = 0;
        }
    }
}



/************************************************************************
 * FUNCTION: Hour_Meters_Write_EEPROM();
 *
 * DESCRIPTION: Writes hour meters RAM data to the EEPROM
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/

void Hour_Meters_Write_EEPROM(void)
{
    uint8_t hour_meters_byte_array [HOUR_METERS_NUMBER_OF_HOUR_METERS * HOUR_METERS_BYTE_LENGTH_OF_HOUR_METERS];
    uint16_t i;
    
    for (i = 0; i < HOUR_METERS_NUMBER_OF_HOUR_METERS * HOUR_METERS_BYTE_LENGTH_OF_HOUR_METERS; i++)
    {
        /*This will reverse the write order so that read and write work 
         * together. This will shift bits in the following order 24, 16, 8, 0 
         and repeat*/
        hour_meters_byte_array[i] = hour_meters_array[i/HOUR_METERS_BYTE_LENGTH_OF_HOUR_METERS] 
                >> (BYTE_LENGTH * ((HOUR_METERS_BYTE_LENGTH_OF_HOUR_METERS - 1) -( i % HOUR_METERS_BYTE_LENGTH_OF_HOUR_METERS)));
    }
        
    API_WriteNvram( HOUR_METERS_EEPROM_PAGE_ADDRESS * PAGE_LENGTH
            , hour_meters_byte_array
            , HOUR_METERS_NUMBER_OF_HOUR_METERS * HOUR_METERS_BYTE_LENGTH_OF_HOUR_METERS );
}




