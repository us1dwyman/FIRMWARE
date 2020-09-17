/* 
 * File:   hal.h
 * Author: US1JKAZE
 *
 * Created on December 30, 2019, 3:07 PM
 */

#ifndef HAL_H
#define	HAL_H

#include <stdint.h>
#include <xc.h>
#include <stdbool.h>
/** D E F I N I T I O N S *************************************************/
///////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
#define FORWARD         0b00001000
#define REVERSE         0b00010000

#define DELAY for(int i=0; i<10; i++) __delay_ms(10)
#define TRUE 1
#define FALSE 0


unsigned char SW_row;
unsigned char machine_direction;



#endif	/* HAL_H */

