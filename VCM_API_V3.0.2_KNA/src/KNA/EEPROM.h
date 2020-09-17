/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 

 * File:   hour_meters.h
 * Author: us1jbernath
 *
 * Created on August 31, 2020, 10:13 AM
 * 
 *This code is used to control the hour meters.  There are 16 possible hour 
 * meter each is 32 bit long.  This code will read and write the hour meter 
 * values to the EEPROM, turn each on and off and return the value for each
 * hour meter.  
 */ 


#include "api.h"

#ifndef _EEPROM_H_
#define _EEPROM_H_

#define BYTE_LENGTH             8
#define PAGE_LENGTH             64

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

void EEPROM_Initialize(void);

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

void EEPROM_Task(void);

#endif

