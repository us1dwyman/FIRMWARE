/* 
 * File:   EUSART.h
 * Author: US1JKAZE
 *
 * Created on December 30, 2019, 2:22 PM
 */

#ifndef EUSART_H
#define	EUSART_H

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "can.h"

/*Set up pins*/
#define EUSART_TX1_TRIS    TRISCbits.TRISC6
#define EUSART_RX1_TRIS    TRISCbits.TRISC7
        

/*Code Configurator generated taken from a pic 18*/
#define EUSART_DataReady  (EUSART_Rx_Count)
#define EUSART_TX_BUFFER_SIZE 27
#define EUSART_RX_BUFFER_SIZE 27
#define EUSART_PACKET_DATA_SIZE 27

/*these are the different case for Rx data, used to determine how to treat each 
 * byte*/
#define EUSART_PACKET_ADDRESS 0x00
#define EUSART_PACKET_DATA 0x01
#define EUSART_PACKET_CHECKSUM 0x02

#define EUSART_ADDRESS                      0x55 // arbitrary
#define SERIAL_NUMBER_SALVE_ADDRESS         0x63 // arbitrary
#define SERVICE_LOG_SLAVE_ADDRESS           0xF0 // arbitrary

typedef enum
{
    EUSART_ADDRESS_BYTE = 0,        
    EUSART_ERROR_BYTE_1 = 1,
    EUSART_ERROR_BYTE_2 = 2,
    EUSART_ERROR_BYTE_3 = 3,
    EUSART_MACHINE_STATUS_BYTE = 4,
    EUSART_WARNING_BYTE = 5,
    EUSART_BATTERY_BYTE_1 = 6,
    EUSART_BATTERY_BYTE_2 = 7,
    EUSART_HOUR_METER_BYTE_1 = 8, //MSB
    EUSART_HOUR_METER_BYTE_2 = 9, 
    EUSART_HOUR_METER_BYTE_3 = 10, //LSB
    EUSART_SETTINGS_CONTROL_BYTE = 11, 
    EUSART_BRUSH_CURRENT_BYTE_1 = 12, //MSB 
    EUSART_BRUSH_CURRENT_BYTE_2 = 13, //LSB 
    EUSART_SIDE_BROOM_CURRENT_BYTE_1 = 14, //MSB 
    EUSART_SIDE_BROOM_CURRENT_BYTE_2 = 15, //LSB 
    EUSART_DRIVE_CURRENT_BYTE_1 = 16, //MSB 
    EUSART_DRIVE_CURRENT_BYTE_2 = 17, //LSB 
    EUSART_VACUUM_CURRENT_BYTE_1 = 18, //MSB 
    EUSART_VACUUM_CURRENT_BYTE_2 = 19, //LSB
    EUSART_BRUSH_TEMPERATURE_BYTE_1 = 20, //MSB 
    EUSART_BRUSH_TEMPERATURE_BYTE_2 = 21, //LSB 
    EUSART_SIDE_BROOM_TEMPERATURE_BYTE_1 = 22, //MSB 
    EUSART_SIDE_BROOM_TEMPERATURE_BYTE_2 = 23, //LSB 
    EUSART_DRIVE_TEMPERATURE_BYTE_1 = 24, //MSB 
    EUSART_DRIVE_TEMPERATURE_BYTE_2 = 25, //LSB    
 }EUSART_BYTES;

#define EUSART_SET_SLAVE_ADDRESS()                      EUSART_packet_data[EUSART_ADDRESS_BYTE] = 0x55 // arbitrary
#define EUSART_SET_SERIAL_NUMBER_SALVE_ADDRESS()        EUSART_packet_data[EUSART_ADDRESS_BYTE] = 0x63 // also used for manufacture date
#define EUSART_SET_SERVICE_LOG_SLAVE_ADDRESS()          EUSART_packet_data[EUSART_ADDRESS_BYTE] = 0xF0 // 

/**************set cleaning mode to be transmitted**************************/
#define EUSART_SET_CLEANING_MODE_OFF()                  EUSART_packet_data[EUSART_MACHINE_STATUS_BYTE] &= 0b11111000 & EUSART_mode_status_off;          \
                                                        EUSART_packet_data[EUSART_MACHINE_STATUS_BYTE] |= EUSART_mode_status_off
#define EUSART_SET_CLEANING_MODE_ON()                   EUSART_packet_data[EUSART_MACHINE_STATUS_BYTE] &= 0b11111000 & EUSART_mode_status_on;           \
                                                        EUSART_packet_data[EUSART_MACHINE_STATUS_BYTE] |= EUSART_mode_status_on
#define EUSART_SET_CLEANING_MODE_SERVICE()              EUSART_packet_data[EUSART_MACHINE_STATUS_BYTE] &= 0b11111000 & EUSART_mode_status_settings;      \
                                                        EUSART_packet_data[EUSART_MACHINE_STATUS_BYTE] |= EUSART_mode_status_settings
#define EUSART_SET_CLEANING_MODE_TRANSPORT()            EUSART_packet_data[EUSART_MACHINE_STATUS_BYTE] &= 0b11111000 & EUSART_mode_status_transport;    \
                                                        EUSART_packet_data[EUSART_MACHINE_STATUS_BYTE] |= EUSART_mode_status_transport
#define EUSART_SET_CLEANING_MODE_DEEP()                 EUSART_packet_data[EUSART_MACHINE_STATUS_BYTE] &= 0b11111000 & EUSART_mode_status_deep;         \
                                                        EUSART_packet_data[EUSART_MACHINE_STATUS_BYTE] |= EUSART_mode_status_deep
#define EUSART_SET_CLEANING_MODE_ECO()                  EUSART_packet_data[EUSART_MACHINE_STATUS_BYTE] &= 0b11111000 & EUSART_mode_status_eco;          \
                                                        EUSART_packet_data[EUSART_MACHINE_STATUS_BYTE] |= EUSART_mode_status_eco
#define EUSART_SET_CLEANING_MODE_ACCESSORY()            EUSART_packet_data[EUSART_MACHINE_STATUS_BYTE] &= 0b11111000 & EUSART_mode_status_accessory;    \
                                                        EUSART_packet_data[EUSART_MACHINE_STATUS_BYTE] |= EUSART_mode_status_accessory
#define EUSART_SERVICE_TOOL_MODE_ON()                   EUSART_packet_data[EUSART_WARNING_BYTE] |= 0b00100000
#define EUSART_SERVICE_TOOL_MODE_OFF()                  EUSART_packet_data[EUSART_WARNING_BYTE] &= 0b11011111
/*******************return to settings without hitting back*******************/
#define EUSART_UP_BUTTON()                              EUSART_packet_data[EUSART_SETTINGS_CONTROL_BYTE] &= 0b11110000;                                 \
                                                        EUSART_packet_data[EUSART_SETTINGS_CONTROL_BYTE] |= 0b00000001
#define EUSART_DOWN_BUTTON()                            EUSART_packet_data[EUSART_SETTINGS_CONTROL_BYTE] &= 0b11110000;                                 \
                                                        EUSART_packet_data[EUSART_SETTINGS_CONTROL_BYTE] |= 0b00000010
#define EUSART_BACK_BUTTON()                            EUSART_packet_data[EUSART_SETTINGS_CONTROL_BYTE] &= 0b11110000;                                 \
                                                        EUSART_packet_data[EUSART_SETTINGS_CONTROL_BYTE] |= 0b00000100
#define EUSART_SELECT_BUTTON()                          EUSART_packet_data[EUSART_SETTINGS_CONTROL_BYTE] &= 0b11110000;                                 \
                                                        EUSART_packet_data[EUSART_SETTINGS_CONTROL_BYTE] |= 0b00001000
#define EUSART_BUTTON_CLEAR()                           EUSART_packet_data[EUSART_SETTINGS_CONTROL_BYTE] &= 0b11110000
#define EUSART_RETURN_TO_SETTINGS_SET()                 EUSART_packet_data[EUSART_SETTINGS_CONTROL_BYTE] |= 0b00010000
#define EUSART_RETURN_TO_SETTINGS_RESET()               EUSART_packet_data[EUSART_SETTINGS_CONTROL_BYTE] |= 0b00100000
#define EUSART_RETURN_TO_SETTINGS_CLEAR()               EUSART_packet_data[EUSART_SETTINGS_CONTROL_BYTE] &= 0b11001111
#define EUSART_SETTINGS_LOCK()                          EUSART_packet_data[EUSART_SETTINGS_CONTROL_BYTE] |= 0b10000000
#define EUSART_SETTINGS_UNLOCK()                        EUSART_packet_data[EUSART_SETTINGS_CONTROL_BYTE] &= 0b01111111

#define EUSART_SERVICE_LOG_WRITE_RECEIVED()             EUSART_packet_data[1] &= 0b11111011; EUSART_packet_data[1] |= 0b00000011
#define EUSART_SERVICE_LOG_READ_RECEIVED()              EUSART_packet_data[1] &= 0b11111100; EUSART_packet_data[1] |= 0b00000100

#define EUSART_SERIAL_NUMBER_WRITE_ON()                 EUSART_packet_data[1] &= 0b11111101;EUSART_packet_data[1] |= 0b00000001
#define EUSART_SERIAL_NUMBER_WRITE_OFF()                EUSART_packet_data[1] &= 0b11111110
#define EUSART_SERIAL_NUMBER_READ_ON()                  EUSART_packet_data[1] &= 0b11111110;EUSART_packet_data[1] |= 0b00000010
#define EUSART_SERIAL_NUMBER_READ_OFF()                 EUSART_packet_data[1] &= 0b11111101
 
#define EUSART_MANUFACTURE_DATE_WRITE_ON()              EUSART_packet_data[8] &= 0b11111101; EUSART_packet_data[8] |= 0b00000001
#define EUSART_MANUFACTURE_DATE_WRITE_OFF()             EUSART_packet_data[8] &= 0b11111110
#define EUSART_MANUFACTURE_DATE_READ_ON()               EUSART_packet_data[8] &= 0b11111110; EUSART_packet_data[8] |= 0b00000010
#define EUSART_MANUFACTURE_DATE_READ_OFF()              EUSART_packet_data[8] &= 0b11111101

#define SERIAL_NUMBER_TURN                              true
#define MANUFACTURE_DATE_TURN                           false
 
#define CLEAR_EUSART_DATA()                             for(int i = 0; i < EUSART_RX_BUFFER_SIZE; i++) {EUSART_packet_data[i] = 0x00;}
/*define the machine mode status that are received from the main board*/
typedef enum
{
  EUSART_mode_status_off = 0,
  EUSART_mode_status_on = 1,        
  EUSART_mode_status_settings = 2,
  EUSART_mode_status_transport = 3,   
  EUSART_mode_status_deep = 4,
  EUSART_mode_status_eco = 5,
  EUSART_mode_status_accessory = 6,      
}EUSART_CLEANING_MODE_STATUS;

/*Code Configurator generated*/
unsigned char EUSART_Tx_head = 0;
unsigned char EUSART_Tx_tail = 0;
unsigned char EUSART_Tx_buffer[EUSART_TX_BUFFER_SIZE];
unsigned char EUSART_Tx_buffer_remaining;

/*Code Configurator generated*/
unsigned char EUSART_Rx_head = 0;
unsigned char EUSART_Rx_tail = 0;
unsigned char EUSART_Rx_buffer[EUSART_RX_BUFFER_SIZE];
unsigned char EUSART_Rx_count;

unsigned char current_address_ID;

/*used to count the number of ones for determine parity being transmitted and received*/
unsigned char EUSART_parity_count;
/*used to count the number of ones for determine parity being received*/
unsigned char EUSART_byte_parity;
/*Flags a parity count error*/
unsigned char EUSART_Rx_parity_error;
/*used to store the string being received*/
unsigned char EUSART_packet_data[EUSART_PACKET_DATA_SIZE];
/*the checksum value received */
unsigned char EUSART_packet_checksum;
/*the calculated checksum value of the byes being sent*/
unsigned char EUSART_Tx_checksum;
/*the calculated checksum value of the byes being received*/
unsigned char EUSART_Rx_checksum;
/*a flag indicating that there was a checksum error*/
unsigned char EUSART_Rx_checksum_error;
/*store the address and command of the last request, and zeros if the correct 
 * response was received, if a bad response was received the address and command 
 * will be pulled from this*/
unsigned int EUSART_slave_bad_response;
/*A flag to send data at a regular interval or if the slave does not respond or responds incorrectly*/
unsigned char EUSART_send_data_flag;
/*this flags is set in receive interrupts and are used in the main to complete the receive*/
unsigned char EUSART_receive_flag;

bool CAN_ID_switch;

/*******************************************************************************
 * FUNCTION: EUSART_initialize();
 * DESCRIPTION: Initializes the EUSART
 ******************************************************************************/
void EUSART_Initialize(void);

/*******************************************************************************
 * FUNCTION: EUSART_Transmit_ISR();
 * DESCRIPTION: Transmits serial data when flagged to do so.
 ******************************************************************************/
void EUSART_Transmit_ISR(void);

/*******************************************************************************
 * FUNCTION: EUSART_Receive_ISR();
 * DESCRIPTION: Receives serial data when flagged to do so.
 ******************************************************************************/
void EUSART_Receive_ISR(void);

/*******************************************************************************
 * FUNCTION: EUSART_Receive();
 * DESCRIPTION: Receives serial data when flagged to do so.
 ******************************************************************************/
void EUSART_Receive(void);

/*******************************************************************************
 * FUNCTION: EUSART_Master_Request();
 * DESCRIPTION: This is the master request from the slave
 ******************************************************************************/
void EUSART_Master_Request(void);

/*******************************************************************************
 * FUNCTION: EUSART_Parity_Count(unsigned char EUSART_byte_parity)
 * DESCRIPTION: check the byte parity
 ******************************************************************************/
bool EUSART_Parity_Count(unsigned char EUSART_byte_parity);

/*******************************************************************************
 * FUNCTION: Get_Settings_Lock_Status()
 * DESCRIPTION: return status if we can change out of settings
 ******************************************************************************/
unsigned char Get_Settings_Lock_Status(void);

/*******************************************************************************
 * FUNCTION: CAN_To_EUSART(can_t *message)
 * DESCRIPTION: convert received can messages to EUSART messages
 ******************************************************************************/
void CAN_To_EUSART(can_t *message);

/*******************************************************************************
 * FUNCTION: EUSART_TO_CAN(can_t *message)
 * DESCRIPTION: populate the can message with data from the EUSART
 ******************************************************************************/
void EUSART_TO_CAN(can_t *message);

#endif	/* EUSART_H */

