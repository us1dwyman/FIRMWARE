/******************************************************************************
 *
 * Description: Code needed to drive all of the motors inputs
 *
 * Author:      us1jkaze
 * Created:     March 20, 2020, 2:46 PM
 *
 * Changed:     April 30, 2020, 3:15 PM
 * Changed by:  us1jkaze
 * Revision:    Rev: 2.0
 *
 * Copyright (C) 2013-2017 by Alfred Kaercher GmbH & Co. KG.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 ******************************************************************************/


#include <xc.h>
#include "hal.h"
#include "LED.h"
#include "EUSART.h"

/*******************************************************************************
 * FUNCTION: LED_initialize()
 * DESCRIPTION: Initializes the LEDs to off
 ******************************************************************************/
void LED_Initialize(void)
{
    // PORTE: LED 25,26,27 off to start
    ANSELE = 0;
    LATE = 0b00000111;
    TRISE = 0;

    // PORT F: LED column outputs
    ANSELF = 0;
    LATF = 0;
    TRISF = 0;
//    LED_ROW_1_TRIS = 0;
//    LED_ROW_2_TRIS = 0;
//    LED_ROW_3_TRIS = 0;
//    LED_ROW_4_TRIS = 0;
//    LED_ROW_5_TRIS = 0;
//    
//    LED_COLUMN_1_TRIS = 0;
//    LED_COLUMN_2_TRIS = 0;
//    LED_COLUMN_3_TRIS = 0;
//    LED_COLUMN_4_TRIS = 0;
//    LED_COLUMN_5_TRIS = 0;
//    
//    LED_1_TRIS = 0;
//    LED_2_TRIS = 0;
//    LED_3_TRIS = 0;
    LED_column_array[0] = 0x00;
    LED_column_array[1] = 0x00;
    LED_column_array[2] = 0x00;
    LED_column_array[3] = 0x00;
    LED_column_array[4] = 0x00;
    
    LED_row_array[0] = LED_ROW1_PORTA;
    LED_row_array[1] = LED_ROW2_PORTA;
    LED_row_array[2] = LED_ROW3_PORTA;
    LED_row_array[3] = LED_ROW4_PORTA;
    LED_row_array[4] = LED_ROW5_PORTA;
    return;
}

/*******************************************************************************
 * FUNCTION: Update_LEDs(unsigned int row)
 * DESCRIPTION: LEDs have to be toggled on and off at a rate that we perceive 
 * only certain LEDs are on. This function updates the matrix of the LEDs
 * based on the ROW that was given to turn on and off the appropriate LEDs
 ******************************************************************************/
void Update_LEDs(unsigned int row)
{
    // disable all columns before switching row, just to be safe
    LED_COLUMN_LAT &= ~LED_COLUMN_MASK;

    // turn off (all) rows (only one should be on anyway)
    LED_ROW_LAT &= (~LED_ROW_MASK) | PIC_24_POWER;

    // enable the columns we want
    LED_COLUMN_LAT |= ( LED_column_array[row] & LED_COLUMN_MASK );

    // Now turn on power to the row
    LED_ROW_LAT |= ( LED_row_array[row] & (LED_ROW_MASK | PIC_24_POWER));
}