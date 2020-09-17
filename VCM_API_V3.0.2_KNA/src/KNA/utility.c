/*
 * File:   utility.c
 * Author: us1jbernath
 *
 * Created on August 17, 2020, 11:00 AM
 * 
 * These a general global function developed by KNA
 */


#include "api.h"
#include "utility.h"

/****************************************************************************
  Function:
      uint16_t swap_bytes(uint8_t byte_low, uint8_t byte_high) 

  Description:  This function swaps the byte order since the bytes are swapped
   when they are sent from the charger
  Precondition:
   none 
  Parameters:
   IN bytes to be swapped hi and low bytes
  Return Values:
   16 bit value with the hi and low bytes swapped
  Remarks:
 none
 ****************************************************************************/
uint16_t swap_bytes(uint8_t byte_low, uint8_t byte_high){
    can_object swap;
    swap.object.high_byte =  byte_low;
    swap.object.low_byte  =  byte_high;
    return swap.value;
}

/****************************************************************************
  Function:
      // uint16_t swap_bytes(uint8_t byte_low, uint8_t byte_high) 

  Description:  This function swaps the byte order since the bytes are swapped
   when they are sent from the charger
  Precondition:
   none 
  Parameters:
   IN bytes to be swapped hi and low bytes
  Return Values:
   16 bit value with the hi and low bytes swapped
  Remarks:
 none
 ****************************************************************************/
uint32_t swap_bytes_long(uint8_t byte_first, uint8_t byte_second, uint8_t byte_third, uint8_t byte_fourth){
    can_object_long swap;
    swap.object.fourth_byte  =  byte_first;
    swap.object.third_byte =  byte_second;
    swap.object.second_byte  =  byte_third;
    swap.object.first_byte =  byte_fourth;
    return swap.valueLong;
}
