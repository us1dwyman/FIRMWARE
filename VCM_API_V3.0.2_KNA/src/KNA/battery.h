/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * File:   battery.h
 * Author: us1jbernath
 *
 * Created on July 27, 2020, 9:10 AM
 * 
 *This code is used to control the hour meters.  There are 16 possible hour 
 * meter each is 32 bit long.  This code will read and write the hour meter 
 * values to the EEPROM, turn each on and off and return the value for each
 * hour meter.  
 */ 


#include "api.h"

#ifndef _BATTERY_H_
#define _BATTERY_H_

//This the is number of battery charge levels that can be displayed
#define BATTERY_NUMBER_OF_CHARGE_LEVELS     10

//These are the different battery state of charges that are tracked
#define BATTERY_DEAD                    0   //Dead and will not transport
#define BATTERY_0_SOC                   1   //0% SOC as indicated on HMI
#define BATTERY_13_SOC                  2   //13% SOC as indicated on HMI
#define BATTERY_25_SOC                  3   //25% SOC as indicated on HMI
#define BATTERY_38_SOC                  4   //38% SOC as indicated on HMI
#define BATTERY_50_SOC                  5   //50% SOC as indicated on HMI
#define BATTERY_63_SOC                  6   //63% SOC as indicated on HMI
#define BATTERY_75_SOC                  7   //75% SOC as indicated on HMI
#define BATTERY_88_SOC                  8   //88% SOC as indicated on HMI
#define BATTERY_100_SOC                 9   //100% SOC as indicated on HMI

#define TPD01BAT                 0x0190     // can message ID from Battery
#define TPD02BAT                 0x0290     // can message ID from Battery
#define TPD03BAT                 0x0390     // can message ID from Battery
#define TPD04BAT                 0x0490     // can message ID from Battery

#define BATTERY_COULOMB_COUNT_BYTE_LENGTH       4
#define BATTERY_HEALTH_BYTE_LENGTH              4
#define BATTERY_SOC_BYTE_LENGTH                 1

/*This is the address of the page in the to store the battery information in NVM. 
 The NVM function that will be using this value will multiply it by 64 since it 
 address the byte*/
#define BATTERY_EEPROM_PAGE_ADDRESS         9


/*These values define the slope for the threshold voltage when compared to the 
 * total machine current draw. */ 
static const uint8_t battery_slope [BATTERY_NUMBER_OF_CHARGE_LEVELS] = 
{
   0,      //Battery should neve get here. 
   51,    //Dead battery do not transport, based on 0% SOC slope
   51,    //0% State of charge
   58,    //13% State of charge
   65,    //25% State of charge
   72,    //38% State of charge
   76,    //50% State of charge
   78,    //63% State of charge
   77,    //75% State of charge
   72,    //88% State of charge
};

/*These values define the slope for the threshold voltage when compared to the 
 * total machine current draw. */ 
static const uint16_t battery_y_intercept [BATTERY_NUMBER_OF_CHARGE_LEVELS] =
{
    
    0,        //Battery should never get here. 
    35503,    //Dead battery do not transport, based on 0% State of charge - 0.5V
    36003,    //0% State of charge
    36540,    //13% State of charge
    37004,    //25% State of charge
    37542,    //38% State of charge
    37878,    //50% State of charge
    38282,    //63% State of charge
    38625,    //75% State of charge
    38964,    //88% State of charge
//    0,        //Battery should never get here. 
//    34503,    //Dead battery do not transport, based on 0% State of charge - 0.5V
//    35003,    //0% State of charge
//    35503,    //13% State of charge
//    36003,    //25% State of charge
//    36540,    //38% State of charge
//    37004,    //50% State of charge
//    37542,    //63% State of charge
//    37878,    //75% State of charge
//    38282,    //88% State of charge
};

/************************************************************************
 * FUNCTION: Battery_Tasks();
 *
 * DESCRIPTION: Check to see if the battery needs to be updated and updates.
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
void Battery_Tasks(void);

/************************************************************************
 * FUNCTION: battery_Initialize();
 *
 * DESCRIPTION: initializes the battery level monitor
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
void Battery_Initialize(void);

/************************************************************************
 * FUNCTION: Battery_SOC_Get();
 *
 * DESCRIPTION: Gets the battery state of charge level 
 * checked
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
uint8_t Battery_SOC_Get(void);

/************************************************************************
 * FUNCTION: Battery_SOC_Get();
 *
 * DESCRIPTION: Sets the battery state of charge level 
 * checked
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
void Battery_SOC_Set(uint8_t new_battery_SOC_level);
/****************************************************************************
  Function:
      bool handle_battery_can_messages(uCAN_MSG *charger_MSG)

  Description:  This function parses message from the battery
   and extracts the data from the battery
   none  
  Parameters:
   IN uCAN_MSG *charger_MSG pointer
  Return Values:
   none
  Remarks:
 variables are accessed with get functions
 ****************************************************************************/
bool handle_battery_can_messages(uCAN_MSG *battery_MSG);

/****************************************************************************
  Function:
      remap_TPDO2(void)

  Description:  remap TPDO2 to include minimum cell voltage and state of health
   none  
  Parameters:
  Return Values:
   none
  Remarks:
 variables are accessed with get functions
 ****************************************************************************/
void lithium_remap_TPDO2 (void);

/************************************************************************
 * FUNCTION: battery_runtime_get();
 *
 * Returns the runtime in seconds
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
uint32_t battery_runtime_get(void);

/************************************************************************
 * FUNCTION: Battery_Health_Get();
 *
 * DESCRIPTION: Gets the battery health 
 * checked
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
uint32_t Battery_Health_Get(void);

/************************************************************************
 * FUNCTION: Battery_Health_Set();
 *
 * DESCRIPTION: Sets the battery health
 * checked
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
void Battery_Health_Set(uint32_t new_battery_health);

/************************************************************************
 * FUNCTION: Battery_Read_EEPROM();
 *
 * rReads battery information from the EEPROM
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
void Battery_Read_EEPROM(void);

/************************************************************************
 * FUNCTION: Battery_Write_EEPROM();
 *
 * Writes battery information to the EEPROM
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
void Battery_Write_EEPROM(void);
#endif
