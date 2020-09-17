/* 
 * File:   buttons.h
 * Author: US1JKAZE
 *
 * Created on December 18, 2019, 4:37 PM
 */

#ifndef BUTTONS_H
#define	BUTTONS_H

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "can.h"


#define BUTTON_ROW_1_TRIS                               TRISDbits.TRISD0
#define BUTTON_ROW_2_TRIS                               TRISDbits.TRISD1
#define BUTTON_ROW_3_TRIS                               TRISDbits.TRISD2
#define BUTTON_ROW_4_TRIS                               TRISDbits.TRISD3
#define BUTTON_ROW_5_TRIS                               TRISDbits.TRISD4

#define BUTTON_COLUMN_1_TRIS                            TRISCbits.TRISC2
#define BUTTON_COLUMN_2_TRIS                            TRISCbits.TRISC3
#define BUTTON_COLUMN_3_TRIS                            TRISCbits.TRISC4
#define BUTTON_COLUMN_4_TRIS                            TRISCbits.TRISC5

#define BUTTON_1_TRIS                                   TRISDbits.TRISD5
#define BUTTON_2_TRIS                                   TRISDbits.TRISD6
#define BUTTON_3_TRIS                                   TRISDbits.TRISD7

#define BUTTON_ROW_1()                                  (!PORTDbits.RD0)
#define BUTTON_ROW_2()                                  (!PORTDbits.RD1)
#define BUTTON_ROW_3()                                  (!PORTDbits.RD2)
#define BUTTON_ROW_4()                                  (!PORTDbits.RD3)
#define BUTTON_ROW_5()                                  (!PORTDbits.RD4)

#define BUTTON_COLUMN_1()                               (!PORTCbits.RC2)
#define BUTTON_COLUMN_2()                               (!PORTCbits.RC3)
#define BUTTON_COLUMN_3()                               (!PORTCbits.RC4)
#define BUTTON_COLUMN_4()                               (!PORTCbits.RC5)

#define BUTTON_1()                                      (!PORTDbits.RD5)
#define BUTTON_2()                                      (!PORTDbits.RD6)
#define BUTTON_3()                                      (!PORTDbits.RD7)

// Hardware port that controls the row outputs
#define SWITCH_ROW_LAT                                  LATD

#define SW_ROW1                                         0b00000001
#define SW_ROW2                                         0b00000010
#define SW_ROW3                                         0b00000100
#define SW_ROW4                                         0b00001000
#define SW_ROW5                                         0b00010000

#define SW_ROW_MASK                                     0b00011111U

#define HORN_BUTTON_MASK                                0b10000000

unsigned char button_column_1;
unsigned char button_column_2;
unsigned char button_column_3;
unsigned char button_column_4;
unsigned char button_5;
unsigned char button_6;
unsigned char button_7;
unsigned char SW_ROW_ARRAY[ 5 ] = {SW_ROW1, SW_ROW2, SW_ROW3, SW_ROW4, SW_ROW5};
bool person_on_machine_flag;
/*variable indicating that the rotary button has changed*/
signed char button_flag = 0;


/*******************************************************************************
 * FUNCTION: Buttons_Initialize()
 * DESCRIPTION: Initializes the buttons and LEDs
 ******************************************************************************/
void Buttons_Initialize(void);

/*******************************************************************************
 * FUNCTION: Buttons_Update_Machine_Mode()
 * DESCRIPTION: Updates the machine status based on mode selected
 ******************************************************************************/
void Buttons_Update(unsigned char SW_row, can_t *switch_state);

/*******************************************************************************
 * FUNCTION: Button_Debounce();
 * DESCRIPTION: debounce the button presses
 ******************************************************************************/
void Button_Debounce(void);


#endif	/* BUTTONS_H */

