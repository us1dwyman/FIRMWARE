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
#include "buttons.h"
#include "EUSART.h"
#include "can.h"
#include "LED.h"
#include "hal.h"

/*******************************************************************************
 * FUNCTION: Buttons_Initialize()
 * DESCRIPTION: Initializes the Buttons
 ******************************************************************************/
void Buttons_Initialize(void)
{
    person_on_machine_flag = false;
    // PORT C: matrix switch column inputs plus
    // RC1: CAN_INT
    // RC0: ENABLE (pulled up))
    // RC6: TX1
    // RC7: RX1
    ANSELC = 0;
    LATC = 0;
    TRISC = 0B10111111;
    WPUC = _WPUC_WPUC0_MASK;

    // PORT D: switch row outputs, extra switch inputs
    ANSELD = 0;
    LATD = 0;
    TRISD = 0B11100000;
    
    // interrupt on change port c pins 2,3,4,5
    IOCCN = 0b00111100;
    
    /*set up timer 2, this will this will have a period of about 10ms*/
    T2CONbits.T2OUTPS = 0b1111; 
    T2CONbits.TMR2ON = 0;
    T2CONbits.T2CKPS = 0b10;
    PR2 = 0x96;
    
    TMR2IE = 1;
    TMR2IF = 0;
    
    return;
}

/*******************************************************************************
 * FUNCTION: Buttons_Update_Machine_Mode()
 * DESCRIPTION: Updates the machine status based on mode selected
 ******************************************************************************/
void Buttons_Update(unsigned char SW_row, can_t *switch_state)
{
    Button_Debounce();
    // populate the current switch states in the message


    switch(SW_row)
    {
//        case 0:
//            break;
        case 1:
            if(!button_column_1)
            {
                EUSART_UP_BUTTON();
            }
            else if(!button_column_2)
            {
                EUSART_DOWN_BUTTON();
            }
            else if(!button_column_3)
            {
                EUSART_SELECT_BUTTON();
            }
            else if(!button_column_4)
            {
                EUSART_BACK_BUTTON();
            }
            else
            {
                EUSART_BUTTON_CLEAR();
            }
            break;
        case 2: // only allow for mode changes when we have no errors
            if(!EUSART_packet_data[EUSART_ERROR_BYTE_1] && !EUSART_packet_data[EUSART_ERROR_BYTE_1] && !EUSART_packet_data[EUSART_ERROR_BYTE_1])
            {
                if(!button_column_1)
                {
                    if(((EUSART_packet_data[EUSART_MACHINE_STATUS_BYTE] & 0b00000111)== EUSART_mode_status_settings) 
                    && !(EUSART_packet_data[EUSART_SETTINGS_CONTROL_BYTE]&0b00110000))
                    {
                        EUSART_RETURN_TO_SETTINGS_SET();
                    }
                    else
                    {
                        EUSART_RETURN_TO_SETTINGS_CLEAR();
                    }
                    EUSART_SET_CLEANING_MODE_SERVICE();
                    LED_column_array[0] &= ~LED_TRANSPORT_MASK;
                    LED_column_array[4] &= ~LED_ECO_MASK;
                    LED_column_array[0] &= ~LED_DEEP_EXTRACT_MASK;
                    LED_column_array[1] &= ~LED_ACCESSORY_MASK;
                    LED_column_array[0] |= LED_SETTINGS_MASK;
                }
                else if(!button_column_2 && !Get_Settings_Lock_Status())
                {
                    EUSART_SET_CLEANING_MODE_TRANSPORT();
                    LED_column_array[0] |= LED_TRANSPORT_MASK;
                    LED_column_array[4] &= ~LED_ECO_MASK;
                    LED_column_array[0] &= ~LED_DEEP_EXTRACT_MASK;
                    LED_column_array[1] &= ~LED_ACCESSORY_MASK;
                    LED_column_array[0] &= ~LED_SETTINGS_MASK;
                }
                else if(!button_column_3 && !Get_Settings_Lock_Status())
                {
                    EUSART_SET_CLEANING_MODE_DEEP();
                    LED_column_array[0] |= LED_DEEP_EXTRACT_MASK;
                    LED_column_array[4] &= ~LED_ECO_MASK;
                    LED_column_array[0] &= ~LED_TRANSPORT_MASK;
                    LED_column_array[1] &= ~LED_ACCESSORY_MASK;
                    LED_column_array[0] &= ~LED_SETTINGS_MASK;
                }
                else if(!button_column_4 && !Get_Settings_Lock_Status())
                {
                    EUSART_SET_CLEANING_MODE_ECO();
                    LED_column_array[4] |= LED_ECO_MASK;
                    LED_column_array[0] &= ~LED_TRANSPORT_MASK;
                    LED_column_array[0] &= ~LED_DEEP_EXTRACT_MASK;
                    LED_column_array[1] &= ~LED_ACCESSORY_MASK;
                    LED_column_array[0] &= ~LED_SETTINGS_MASK;
                }
            }
            else // we have an error illuminate all the LEDs
            {
                LED_column_array[4] |= LED_ECO_MASK;
                LED_column_array[0] |= LED_TRANSPORT_MASK;
                LED_column_array[0] |= LED_DEEP_EXTRACT_MASK;
                LED_column_array[1] |= LED_ACCESSORY_MASK;
                LED_column_array[0] |= LED_SETTINGS_MASK;
                LED_column_array[1] &= ~LED_REVERSE;
                LED_column_array[1] &= ~LED_FORWARD;
                machine_direction = 0x00;
            }
            break;
        case 3:
            if(!EUSART_packet_data[EUSART_ERROR_BYTE_1] && !EUSART_packet_data[EUSART_ERROR_BYTE_2] && !EUSART_packet_data[EUSART_ERROR_BYTE_3])
            //if(!EUSART_packet_data[EUSART_ERROR_BYTE_1] && !EUSART_packet_data[EUSART_ERROR_BYTE_1] && !EUSART_packet_data[EUSART_ERROR_BYTE_1]) copy and paste error drw
            {
                if(!button_column_1 && !Get_Settings_Lock_Status())
                {
                    EUSART_SET_CLEANING_MODE_ACCESSORY();
                    LED_column_array[4] &= ~LED_ECO_MASK;
                    LED_column_array[0] &= ~LED_TRANSPORT_MASK;
                    LED_column_array[0] &= ~LED_DEEP_EXTRACT_MASK;
                    LED_column_array[1] |= LED_ACCESSORY_MASK;
                    LED_column_array[0] &= ~LED_SETTINGS_MASK;
                }
            }
            else // we have an error illuminate all the LEDs
            {
                LED_column_array[4] |= LED_ECO_MASK;
                LED_column_array[0] |= LED_TRANSPORT_MASK;
                LED_column_array[0] |= LED_DEEP_EXTRACT_MASK;
                LED_column_array[1] |= LED_ACCESSORY_MASK;
                LED_column_array[0] |= LED_SETTINGS_MASK;
            }
            if(!button_column_2)
            {
                switch_state->buf[7] |= HORN_BUTTON_MASK;
            }
            else
            {
                switch_state->buf[7] &= ~HORN_BUTTON_MASK;
            }
            if(person_on_machine_flag && ((EUSART_packet_data[EUSART_MACHINE_STATUS_BYTE] & 0b00000111) != EUSART_mode_status_accessory)  
                                      && ((EUSART_packet_data[EUSART_MACHINE_STATUS_BYTE] & 0b00000111) != EUSART_mode_status_settings))

            {
                if(!button_column_3 && !Get_Settings_Lock_Status())
                {
                    // reverse
                    LED_column_array[1] &= ~LED_FORWARD;
                    LED_column_array[1] |= LED_REVERSE;
                    machine_direction = REVERSE;
                }
                else if(!button_column_4 && !Get_Settings_Lock_Status())
                {
                    // forward
                    LED_column_array[1] &= ~LED_REVERSE;
                    LED_column_array[1] |= LED_FORWARD;
                    machine_direction = FORWARD;
                }
            }
            else
            {
//                LED_column_array[1] &= ~LED_REVERSE;  drw
//                LED_column_array[1] &= ~LED_FORWARD;
                machine_direction = 0x00;
            }
            break;
//        case 4:
//            break;
    }
    button_column_1 = 0;
    button_column_2 = 0;
    button_column_3 = 0;
    button_column_4 = 0;
    
    button_5 = 0;
    button_6 = 0;
    button_7 = 0;
}

/*******************************************************************************
 * FUNCTION: Button_Debounce()
 * DESCRIPTION: debounce the button presses
 ******************************************************************************/
void Button_Debounce(void)
{ 
   for( int i = 0; i < 16; i++)
    {
        button_column_1 = button_column_1 << 1;
        button_column_1 += BUTTON_COLUMN_1();
        button_column_2 = button_column_2 << 1;
        button_column_2 += BUTTON_COLUMN_2();
        button_column_3 = button_column_3 << 1;
        button_column_3 += BUTTON_COLUMN_3();
        button_column_4 = button_column_4 << 1;
        button_column_4 += BUTTON_COLUMN_4();
    }
}