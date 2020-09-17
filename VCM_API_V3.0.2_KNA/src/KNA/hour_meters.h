/*
 * File:   hour_meters.h
 * Author: us1jbernath
 *
 * Created on April 24, 2020, 9:10 AM
 * 
 *This code is used to control the hour meters.  There are 16 possible hour 
 * meter each is 32 bit long.  This code will read and write the hour meter 
 * values to the EEPROM, turn each on and off and return the value for each
 * hour meter.  
 */ 


#include "api.h"

#ifndef _HOUR_METERS_H_
#define _HOUR_METERS_H_

/*This is the total number of hour meters that is used.  this value must be 
 updated when an hour meter is added.*/
#define HOUR_METERS_NUMBER_OF_HOUR_METERS           4
#define HOUR_METERS_BYTE_LENGTH_OF_HOUR_METERS      4

/*This is the address of the page in the EEPROM that will store the hour meter. 
 The NVM function that will be using this value will multiply it by 64 since it 
 address the byte*/
#define HOUR_METERS_EEPROM_PAGE_ADDRESS         8

/*This is the hour meters resolution in seconds*/
#define HOUR_METERS_RESOLUTION                  1

/*this array contains the values for all of the hour meters.  This value will
 be read from and written to the EEPROM, and updated when a motor is running.*/
static uint32_t hour_meters_array[HOUR_METERS_NUMBER_OF_HOUR_METERS];


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

void Hour_Meters_Initialize(void);

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

void Hour_Meters_Task(void);

/************************************************************************
 * FUNCTION: Hour_Meters_Start();
 *
 * DESCRIPTION: This function is used to start an hour meter
 *
 * INPUT:  hour_meters_bit - this is the bit that will be turned on.  #defines 
 * for the hour meters are in hour_meters.h.  ie. HOUR_METERS_TRACTION_MOTOR_BIT 
 * is for the traction motor hour meter.
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/

void Hour_Meters_Start(uint16_t hour_meters_bit);

/************************************************************************
 * FUNCTION: Hour_Meters_Stop();
 *
 * DESCRIPTION: This function is used to stop an hour meter
 *
 * INPUT:  hour_meters_bit - this is the bit that will be turned off.  #defines 
 * for the hour meters are in hour_meters.h.  ie. HOUR_METERS_TRACTION_MOTOR_BIT 
 * is for the traction motor hour meter.
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/

void Hour_Meters_Stop(uint16_t hour_meters_bit);

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

uint32_t Hour_Meters_Return (uint8_t hour_meters_index);

/************************************************************************
 * FUNCTION: Hour_Meter_Write_Byte();
 *
 * DESCRIPTION: check the motor states and turn the hour meter on if running 
 * turn off if not. Running is based on reported PWM setting from DMC
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/

void Hour_Meter_Write_Byte(uint8_t location, uint8_t value);

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

void Hour_Meters_Read_EEPROM(void);

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

void Hour_Meters_Write_EEPROM(void);

#endif

