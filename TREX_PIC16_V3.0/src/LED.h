/* 
 * File:   LED.h
 * Author: US1JKAZE
 *
 * Created on December 30, 2019, 2:35 PM
 */

#ifndef LED_H
#define	LED_H

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define LED_ROW_1_TRIS                      TRISAbits.TRISA0
#define LED_ROW_2_TRIS                      TRISAbits.TRISA1
#define LED_ROW_3_TRIS                      TRISAbits.TRISA2
#define LED_ROW_4_TRIS                      TRISAbits.TRISA3
#define LED_ROW_5_TRIS                      TRISAbits.TRISA4

#define LED_COLUMN_1_TRIS                   TRISFbits.TRISF0
#define LED_COLUMN_2_TRIS                   TRISFbits.TRISF1
#define LED_COLUMN_3_TRIS                   TRISFbits.TRISF2
#define LED_COLUMN_4_TRIS                   TRISFbits.TRISF3
#define LED_COLUMN_5_TRIS                   TRISFbits.TRISF4

#define LED_1_TRIS                          TRISEbits.TRISE0
#define LED_2_TRIS                          TRISEbits.TRISE1
#define LED_3_TRIS                          TRISEbits.TRISE2

#define LED_ROW                             LATA
#define LED_COLUMN                          LATF
#define LED                                 LATE

//------------------------------//----------------------------------------------
// LED Outputs

#define LED_ROW1_PORTA 0B00000001
#define LED_ROW2_PORTA 0B00000010
#define LED_ROW3_PORTA 0B00000100
#define LED_ROW4_PORTA 0B00001000
#define LED_ROW5_PORTA 0B00010000

// the hardware port controlling the LED row outputs
#define LED_ROW_LAT LATA

// defines the bits we care about in the LED row port register
#define LED_ROW_MASK 0B00011111U

// the hardware port that controls the columns
#define LED_COLUMN_LAT LATF

// defines the bits we care about in the LED column port register
#define LED_COLUMN_MASK 0B00011111U

// the hardware port that controls the 'extra' LEDs (25,26,27)
// These are 0=ON, 1=OFF
#define LED_EXTRA_LAT LATF

// defines the bits we care about in the extra LED port register
#define LED_EXTRA_MASK 0B11100000U

#define OPIN_OUT1 LATFbits.LATF5
#define OPIN_OUT2 LATFbits.LATF6
#define OPIN_OUT3 LATFbits.LATF7

//------------------------------//----------------------------------------------

#define PIC_24_POWER 0b10000000U

#ifdef REVA_HARDWARE
#define LED_TRANSPORT_MASK          0x08
#define LED_DEEP_EXTRACT_MASK       0x04
#define LED_ECO_MASK                0x01
#define LED_FORWARD                 0x04
#define LED_REVERSE                 0x08
#define LED_ACCESSORY_MASK          0x10
#define LED_SETTINGS_MASK           0x10
#else
#define LED_TRANSPORT_MASK          0x02  
#define LED_DEEP_EXTRACT_MASK       0x04  
#define LED_ECO_MASK                0x10  
#define LED_FORWARD                 0x04
#define LED_REVERSE                 0x02
#define LED_ACCESSORY_MASK          0x01   
#define LED_SETTINGS_MASK           0x01   
#endif



        

//        LED_column_array[0]  |= 0b00000100;   // deep
//        LED_column_array[0]  |= 0b00000001;   //  config
//        LED_column_array[0]  |= 0b00000010;   // transport
        
//        LED_column_array[1] |= 0b00000100;     // reverse
//        LED_column_array[1] |= 0b00000010;     // forward
//        LED_column_array[1] |= 0b00000001;     // tool
        
 //         LED_column_array[4] |= 0b00010000;     // eco mode



unsigned char LED_row_array[5];
unsigned char LED_column_array[5];

/*******************************************************************************
 * FUNCTION: LED_initialize()
 * DESCRIPTION: Initializes the LEDs to off
 ******************************************************************************/
void LED_Initialize(void);

/*******************************************************************************
 * FUNCTION: Update_LEDs(unsigned int row)
 * DESCRIPTION: LEDs have to be toggled on and off at a rate that we perceive 
 * only certain LEDs are on. This function updates the matrix of the LEDs
 * based on the ROW that was given to turn on and off the appropriate LEDs
 ******************************************************************************/
void Update_LEDs(unsigned int row);
#endif	/* LED_H */

