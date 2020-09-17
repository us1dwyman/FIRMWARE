/*
 * File:   utility.h
 * Author: us1jbernath
 *
 * Created on August 17, 2020, 11:00 AM
 * 
 * These a general global function developed by KNA
 */ 


#include "api.h"

#ifndef _UTILITY_H_
#define _UTILITY_H_


typedef union {
    struct {
        uint8_t high_byte;
        uint8_t low_byte;
    } object;
    uint16_t value;
} can_object;

typedef union {
    struct {
        uint8_t fourth_byte;
        uint8_t third_byte;
        uint8_t second_byte;
        uint8_t first_byte;
    } object;
    uint32_t valueLong;
} can_object_long;


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
uint16_t swap_bytes(uint8_t byte_low, uint8_t byte_high);

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
uint32_t swap_bytes_long(uint8_t byte_first, uint8_t byte_second, uint8_t byte_third, uint8_t byte_fourth);

#endif