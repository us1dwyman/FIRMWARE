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
#include "can.h"
#include "hal.h"
#include "EUSART.h"
#include "buttons.h"
#include "LED.h"
/*******************************************************************************
 * FUNCTION: EUSART_initialize()
 * DESCRIPTION: Initializes the EUSART
 ******************************************************************************/
void EUSART_Initialize(void)
{
    /*Enable TX, RX and enable pins*/
    EUSART_TX1_TRIS = 0; 
    EUSART_RX1_TRIS = 1; 

    /*reset the receive flag */
    EUSART_receive_flag = 0;   
    
    /*Disables transmit receive interrupt*/
    PIE3bits.RC1IE = 0;
    PIE3bits.TX1IE = 0;

    /*sets up the baud register*/
    BAUD1CON = 0x08;                    // 16 bits; 
    
    /*sets up the baud rate*/ 
    SP1BRG = 0xCE;                      // 38400 baud rate
    SP1BRGH = 0x00;                     // 255 MHz  
 
    /*sets up the receive register*/
    RC1STA = 0xD0;                      // Asynchronous, 9bit reception, continuous receive

    /*sets up the transmit register*/
    TX1STA = 0x64;                      // Asynchronous, 9 bit transmit, transmit enabled, send break
    
    /*Code Configurator generated*/
    EUSART_Tx_head = 0;
    EUSART_Tx_tail = 0;
    EUSART_Tx_buffer_remaining = sizeof(EUSART_Tx_buffer);

    EUSART_Rx_head = 0;
    EUSART_Rx_tail = 0;
    EUSART_Rx_count = 0;
    
    /*Enable receive interrupt*/
    PIE3bits.RC1IE = 1;
    EUSART_slave_bad_response = 0x0000;
    
    /*turn on and set timer 1*/  
    TMR1IF = 0;
    T1CLK = 0b0001;
    T1CON = 0b00100110;
    TMR1IE = 1;
    T1CONbits.TMR1ON = 1; 
    EUSART_SET_SLAVE_ADDRESS();
    current_address_ID = EUSART_packet_data[EUSART_ADDRESS_BYTE];
    EUSART_packet_data[EUSART_MACHINE_STATUS_BYTE] = 0b00000001;
    CAN_ID_switch = SERIAL_NUMBER_TURN;
    return;
}

/*******************************************************************************
 * FUNCTION: EUSART_Transmit_ISR()
 * DESCRIPTION: Transmits serial data when flagged to do so.
 ******************************************************************************/
void EUSART_Transmit_ISR(void)
{   
    /*Code Configurator generated*/
    if(sizeof(EUSART_Tx_buffer) > EUSART_Tx_buffer_remaining)
    {
        TXSTA1bits.TX9D = EUSART_Parity_Count(EUSART_Tx_buffer[EUSART_Tx_tail]);        
        TXREG1 = EUSART_Tx_buffer[EUSART_Tx_tail++];
        if(sizeof(EUSART_Tx_buffer) <= EUSART_Tx_tail)
        {
            EUSART_Tx_tail = 0;
        }
        EUSART_Tx_buffer_remaining++;
    }
    else
    {
        PIE3bits.TX1IE = 0;
    }    
    return;
}

/*******************************************************************************
 * FUNCTION: EUSART_Receive_ISR()
 * DESCRIPTION: Receives serial data when flagged to do so.
 ******************************************************************************/
void EUSART_Receive_ISR(void)
{
    /*Code Configurator generated*/
    if(1 == RCSTA1bits.OERR)
    {
        // EUSART error - restart

        RCSTA1bits.CREN = 0;
        RCSTA1bits.CREN = 1;
    }
    if(RCSTA1bits.RX9D != EUSART_Parity_Count(RCREG1))
    {
        EUSART_Rx_parity_error = 1;
    }

    /* buffer overruns are ignored*/
    EUSART_Rx_buffer[EUSART_Rx_head++] = RCREG1;
    /*when the rx buffer is full set a flag to read the buffer */
    if(EUSART_RX_BUFFER_SIZE == EUSART_Rx_head)
    {
        EUSART_receive_flag = 1;
    }
    if(sizeof(EUSART_Rx_buffer) <= EUSART_Rx_head)
    {   
        EUSART_Rx_head = 0;
    }    
    EUSART_Rx_count++;
    return;
}
/*******************************************************************************
 * FUNCTION: EUSART_Receive();
 * DESCRIPTION: Receives serial data when flagged to do so.
 ******************************************************************************/
void EUSART_Receive(void)
{
    /*zero check sum before adding up message8*/
    EUSART_Rx_checksum = 0;
    /*add up all bytes stack over flow is okay*/
    for(unsigned int i = 0; i < (EUSART_RX_BUFFER_SIZE - 1); ++i)
    {
        EUSART_Rx_checksum += EUSART_Rx_buffer[i];
    }
    /*check if received check sum byte equals calculated, flag an error if they don't*/
    if(EUSART_Rx_checksum != EUSART_Rx_buffer[EUSART_RX_BUFFER_SIZE - 1])
    {
        EUSART_Rx_checksum_error = 1;
    }
    
    /*if there were any errors in request a new reply setting the timer1 to 0xFF00 does that
     maybe take this out and just wait till the next time*/
    if(EUSART_Rx_parity_error || EUSART_Rx_checksum_error || ((EUSART_Rx_buffer[EUSART_ADDRESS_BYTE] != EUSART_ADDRESS)
    && (EUSART_Rx_buffer[EUSART_ADDRESS_BYTE] != SERIAL_NUMBER_SALVE_ADDRESS) && (EUSART_Rx_buffer[EUSART_ADDRESS_BYTE] != SERVICE_LOG_SLAVE_ADDRESS)))
    {
        TMR1 = 0xFF00;
        EUSART_Rx_head = 0;
        EUSART_Rx_tail = 0;
        EUSART_Rx_count = 0;
        EUSART_Rx_checksum_error = 0;
        EUSART_Rx_parity_error = 0;
        return;
    }
    /*if there are no errors store values in the EUSART_packet_data string to be read
     Some of the received bits are ignored because they are not bits that the display 
     board can write to*/
    else
    {
        if (current_address_ID != EUSART_Rx_buffer[EUSART_ADDRESS_BYTE])
        {
            CLEAR_EUSART_DATA();
        }
        current_address_ID = EUSART_Rx_buffer[EUSART_ADDRESS_BYTE];
        switch(EUSART_Rx_buffer[EUSART_ADDRESS_BYTE])
        {       
            case SERIAL_NUMBER_SALVE_ADDRESS:
                EUSART_SET_SERIAL_NUMBER_SALVE_ADDRESS();
                EUSART_packet_data[1] = ((EUSART_Rx_buffer[1] & 0b00001100U) | (EUSART_packet_data[1] & 0b11110011U));
                EUSART_packet_data[8] = ((EUSART_Rx_buffer[8] & 0b00001100U) | (EUSART_packet_data[8] & 0b11110011U));
                if(EUSART_packet_data[1] & 0b00000010)
                {
                    EUSART_packet_data[2] = EUSART_Rx_buffer[2];
                    EUSART_packet_data[3] = EUSART_Rx_buffer[3];
                    EUSART_packet_data[4] = EUSART_Rx_buffer[4];
                    EUSART_packet_data[5] = EUSART_Rx_buffer[5];
                    EUSART_packet_data[6] = EUSART_Rx_buffer[6];
                    EUSART_packet_data[7] = EUSART_Rx_buffer[7];
                }
                if(EUSART_packet_data[8] & 0b00000010)         //Manufacturer Date Read 1 = Read, 0 = Don't Read
                {
                    EUSART_packet_data[11] = EUSART_Rx_buffer[11];
                    EUSART_packet_data[12] = EUSART_Rx_buffer[12];
                    EUSART_packet_data[13] = EUSART_Rx_buffer[13];
                }
                break;
                
            case SERVICE_LOG_SLAVE_ADDRESS:
                EUSART_SET_SERVICE_LOG_SLAVE_ADDRESS();
                EUSART_packet_data[1] = ((EUSART_Rx_buffer[1] & 0b00000010U) | (EUSART_packet_data[1] & 0b11111101U));
                if(!(EUSART_packet_data[1] & 0b00000001U))
                {
                    EUSART_packet_data[2] = EUSART_Rx_buffer[2];
                    EUSART_packet_data[3] = EUSART_Rx_buffer[3];
                    EUSART_packet_data[4] = EUSART_Rx_buffer[4];
                    EUSART_packet_data[5] = EUSART_Rx_buffer[5];
                }
                break;
                
            case EUSART_ADDRESS:    
            default:
                EUSART_SET_SLAVE_ADDRESS();
                
                EUSART_packet_data[EUSART_MACHINE_STATUS_BYTE] = (EUSART_Rx_buffer[EUSART_MACHINE_STATUS_BYTE] & 0b00000111) 
                                                               | (EUSART_packet_data[EUSART_MACHINE_STATUS_BYTE] & 0b11111000);
                if((EUSART_packet_data[EUSART_MACHINE_STATUS_BYTE] & 0b00000111U) == 0b00000001U)
                {
                    EUSART_SET_CLEANING_MODE_ON();
                    LED_column_array[0] &= ~LED_SETTINGS_MASK;
                }
                
                EUSART_packet_data[EUSART_WARNING_BYTE] = (EUSART_Rx_buffer[EUSART_WARNING_BYTE]& 0b01000000U) 
                                                        | (EUSART_packet_data[EUSART_WARNING_BYTE] & 0b10111111U);
                
                EUSART_packet_data[EUSART_SETTINGS_CONTROL_BYTE] = (EUSART_Rx_buffer[EUSART_SETTINGS_CONTROL_BYTE]& 0b11100000U) 
                                                                 | (EUSART_packet_data[EUSART_SETTINGS_CONTROL_BYTE] & 0b00011111U);
                if(EUSART_packet_data[EUSART_SETTINGS_CONTROL_BYTE]&0b00100000U)
                {
                    EUSART_RETURN_TO_SETTINGS_CLEAR();
                }
        }
    }
    return;
}
/*******************************************************************************
 * FUNCTION: EUSART_Master_Request();
 * DESCRIPTION: This is the master request from the slave
 ******************************************************************************/
void EUSART_Master_Request(void)
{
    /*reset all of the values incase of an incomplete message*/
    EUSART_Tx_buffer_remaining = 0;
    EUSART_Tx_head = 0;
    EUSART_Tx_tail = 0;
    EUSART_Tx_checksum = 0;
    //EUSART_SET_SLAVE_ADDRESS();
    
    /*Transfer the information from the EUSART_packet_data to the Tx buffer 
     and add all of the bytes for the check sum value overflow is okay*/
    for(unsigned int i=0; i<(EUSART_TX_BUFFER_SIZE - 1); ++i)
    {
        EUSART_Tx_buffer[i] = EUSART_packet_data[i];
        EUSART_Tx_checksum += EUSART_packet_data[i];
    }
    /*Transfer the information from the check sum byte to the Tx buffer*/
   // EUSART_Tx_buffer[EUSART_ADDRESS_BYTE] = EUSART_ADDRESS;
    EUSART_packet_data[EUSART_TX_BUFFER_SIZE - 1] = EUSART_Tx_checksum;
    EUSART_Tx_buffer[EUSART_TX_BUFFER_SIZE - 1] = EUSART_Tx_checksum;
    
    /*enable the transmit interrupt service routine*/
    PIE3bits.TX1IE = 1;
           
    return;
}

/*******************************************************************************
 * FUNCTION: EUSART_Parity_Count(unsigned char EUSART_byte_parity)
 * DESCRIPTION: check the byte parity
 ******************************************************************************/
bool EUSART_Parity_Count(unsigned char EUSART_byte_parity)
{
    /*reset parity count*/
    EUSART_parity_count = 0;
    
    /*check if the byte is even or odd parity and return*/
    for(int i = 0; i<8; i++)
    {
        if( EUSART_byte_parity & 0x01 << i )
        {
            EUSART_parity_count++;
        }
    }
    if( EUSART_parity_count % 2 )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*******************************************************************************
 * FUNCTION: Get_Settings_Lock_Status()
 * DESCRIPTION: return status if we can change out of settings
 ******************************************************************************/
unsigned char Get_Settings_Lock_Status(void)
{
    return (EUSART_packet_data[EUSART_SETTINGS_CONTROL_BYTE] & 0b10000000U); 
    
}

/*******************************************************************************
 * FUNCTION: CAN_To_EUSART(can_t *message)
 * DESCRIPTION: convert received can messages to EUSART messages
 ******************************************************************************/
void CAN_To_EUSART(can_t *message)
{
    uint8_t machine_directionA;
    // last byte of the ID is all we need to know
    //switch((message->id>>8) & 0x000000FFU)
    switch(message->id & 0x0000FFFFU)
    {
        //case CANADDR_VCM:
        case CANADDR_VCM_0:
            if(EUSART_packet_data[EUSART_ADDRESS_BYTE] == EUSART_ADDRESS)
            {
                EUSART_packet_data[EUSART_ERROR_BYTE_1] = message->buf[0];
                EUSART_packet_data[EUSART_ERROR_BYTE_2] = message->buf[1];
                EUSART_packet_data[EUSART_ERROR_BYTE_3] = message->buf[2];
                
                // parse speed value
                EUSART_packet_data[EUSART_MACHINE_STATUS_BYTE] = (((message->buf[3] & 0b00000111U) << 3) 
                                                               | (EUSART_packet_data[EUSART_MACHINE_STATUS_BYTE] & 0b00000111U));
                
                
                EUSART_packet_data[EUSART_WARNING_BYTE] = (((message->buf[3] >> 3) | (EUSART_packet_data[EUSART_WARNING_BYTE] & 0b01000000U))
                                                        | ((message->buf[2] & 0b10000000U) >> 2));
                
                EUSART_packet_data[EUSART_BATTERY_BYTE_1] = (message->buf[4]  &0b00011111)<<2;   // 5 bits
                
                person_on_machine_flag = (message->buf[4] &0b00100000);
                EUSART_packet_data[EUSART_HOUR_METER_BYTE_2] = message->buf[5];
                EUSART_packet_data[EUSART_HOUR_METER_BYTE_3] = message->buf[6];
                EUSART_packet_data[EUSART_BATTERY_BYTE_2] = message->buf[7];
            }    
            EUSART_SET_SLAVE_ADDRESS();
            break;
        case CANADDR_VCM_2:
            machine_directionA = message->buf[0] &0b00011000;
            if(machine_directionA == FORWARD ){
                LED_column_array[1] &= ~LED_REVERSE;
                LED_column_array[1] |= LED_FORWARD;
            } else if(machine_directionA == REVERSE ){
                LED_column_array[1] &= ~LED_FORWARD;
                LED_column_array[1] |= LED_REVERSE;
            } else {
                LED_column_array[1] &= ~LED_REVERSE;
                LED_column_array[1] &= ~LED_FORWARD;
                if(!person_on_machine_flag)
                {
                    EUSART_SET_CLEANING_MODE_ON();
                    LED_column_array[0] &= ~LED_TRANSPORT_MASK;
                    LED_column_array[4] &= ~LED_ECO_MASK;
                    LED_column_array[0] &= ~LED_DEEP_EXTRACT_MASK;
                    LED_column_array[1] &= ~LED_ACCESSORY_MASK;
                    LED_column_array[0] &= ~LED_SETTINGS_MASK;
                }
            }
            break;
        case CANADDR_TRACTION:
            EUSART_packet_data[EUSART_DRIVE_CURRENT_BYTE_1] = message->buf[0];
            EUSART_packet_data[EUSART_DRIVE_CURRENT_BYTE_2] = message->buf[1];
            EUSART_packet_data[EUSART_DRIVE_TEMPERATURE_BYTE_1] = message->buf[2];
            EUSART_packet_data[EUSART_DRIVE_TEMPERATURE_BYTE_2] = message->buf[3];
            break;
        case CANADDR_BRUSH:
            EUSART_packet_data[EUSART_BRUSH_CURRENT_BYTE_1] = message->buf[0];
            EUSART_packet_data[EUSART_BRUSH_CURRENT_BYTE_2] = message->buf[1];
            EUSART_packet_data[EUSART_BRUSH_TEMPERATURE_BYTE_1] = message->buf[2];
            EUSART_packet_data[EUSART_BRUSH_TEMPERATURE_BYTE_2] = message->buf[3];
            break;
        case CANADDR_VACUUM:
            EUSART_packet_data[EUSART_VACUUM_CURRENT_BYTE_1] = message->buf[0];
            EUSART_packet_data[EUSART_VACUUM_CURRENT_BYTE_2] = message->buf[1];
            break;
        case CANADDR_SIDEBROOM:
            EUSART_packet_data[EUSART_SIDE_BROOM_CURRENT_BYTE_1] = message->buf[0];
            EUSART_packet_data[EUSART_SIDE_BROOM_CURRENT_BYTE_2] = message->buf[1];
            EUSART_packet_data[EUSART_SIDE_BROOM_TEMPERATURE_BYTE_1] = message->buf[2];
            EUSART_packet_data[EUSART_SIDE_BROOM_TEMPERATURE_BYTE_2] = message->buf[3];
            break;
        case CANADDR_CHARGER:
            EUSART_packet_data[EUSART_BATTERY_BYTE_1] = message->buf[0];
            break;
        case CANADDR_VCM_SERIAL_NUMBER:
            EUSART_SET_SERIAL_NUMBER_SALVE_ADDRESS();
            EUSART_packet_data[1] = (((message->buf[0] & 0b00100000U)>> 5) | ((message->buf[0] & 0b01000000U)>> 4));
            EUSART_packet_data[2] = message->buf[2];
            EUSART_packet_data[3] = message->buf[3];
            EUSART_packet_data[4] = message->buf[4];
            EUSART_packet_data[5] = message->buf[5];
            EUSART_packet_data[6] = message->buf[6];
            EUSART_packet_data[7] = message->buf[7];
            EUSART_packet_data[8] = message->buf[1];
            EUSART_packet_data[9] = (message->buf[0] & 0b00011111U);
            break;    
        case CANADDR_PC:
            EUSART_SERVICE_TOOL_MODE_ON();
            break;
        case CANADDR_PC_SERIAL_NUMBER:
            if(EUSART_packet_data[EUSART_ADDRESS_BYTE] == SERIAL_NUMBER_SALVE_ADDRESS)
            {
                if(message->buf[7] == 1)
                {
                    EUSART_packet_data[2] = message->buf[0];
                    EUSART_packet_data[3] = message->buf[1];
                    EUSART_packet_data[4] = message->buf[2];
                    EUSART_packet_data[5] = message->buf[3];
                    EUSART_packet_data[6] = message->buf[4];
                    EUSART_packet_data[7] = message->buf[5];
                    EUSART_SERIAL_NUMBER_WRITE_ON();
                }
                else
                {
                    EUSART_SERIAL_NUMBER_READ_ON();
                }
            }
            EUSART_MANUFACTURE_DATE_WRITE_OFF();
            EUSART_MANUFACTURE_DATE_READ_OFF();
            EUSART_SET_SERIAL_NUMBER_SALVE_ADDRESS();
            break;
        case CANADDR_PC_SERVICE_LOG:
            EUSART_SET_SERVICE_LOG_SLAVE_ADDRESS();
            if(message->buf[7] & 0x01)
            {
                EUSART_packet_data[2] = message->buf[1];
                EUSART_packet_data[3] = message->buf[2];
                EUSART_packet_data[4] = message->buf[3];
                EUSART_packet_data[5] = message->buf[4];
            }
            break;
        case CANADDR_PC_MANUFACTURE_DATE:
            if(EUSART_packet_data[EUSART_ADDRESS_BYTE] == SERIAL_NUMBER_SALVE_ADDRESS)
            {    
                if(message->buf[4] & 0b00000001)
                {
                    EUSART_packet_data[11] = message->buf[5];
                    EUSART_packet_data[12] = message->buf[6];
                    EUSART_packet_data[13] = message->buf[7];
                    EUSART_MANUFACTURE_DATE_WRITE_ON(); 
                }
                else if (message->buf[4] & 0b00000010)
                {
                    EUSART_MANUFACTURE_DATE_READ_ON();
                }
                else
                {
                    EUSART_MANUFACTURE_DATE_WRITE_OFF();
                    EUSART_MANUFACTURE_DATE_READ_OFF();
                }
            }
            EUSART_SERIAL_NUMBER_WRITE_OFF();
            EUSART_SERIAL_NUMBER_READ_OFF();
            EUSART_SET_SERIAL_NUMBER_SALVE_ADDRESS();
            break;
    }
}

/*******************************************************************************
 * FUNCTION: EUSART_TO_CAN(can_t *message)
 * DESCRIPTION: populate the can message with data from the EUSART
 ******************************************************************************/
void EUSART_TO_CAN(can_t *message)
{
    message->id = (message->id & 0xFFFFFF00);
    switch(EUSART_packet_data[EUSART_ADDRESS_BYTE])
    {
        case EUSART_ADDRESS:
        default:
            message->id |= CANADDR_HMI;
            message->buf[0] = (EUSART_packet_data[EUSART_MACHINE_STATUS_BYTE] & 0b00000111U) 
                            | (message->buf[7] & HORN_BUTTON_MASK);
            message->buf[0] |= machine_direction;
            message->buf[0] |= ((EUSART_packet_data[EUSART_ERROR_BYTE_3] & 0b00010000U) << 1);
            message->buf[0] |= (EUSART_packet_data[EUSART_WARNING_BYTE] & 0b01000000U);
            message->buf_len = 1;
            break;
        case SERIAL_NUMBER_SALVE_ADDRESS:
            if(CAN_ID_switch)
            {
                message->id |= CANADDR_HMI_SERIAL_NUMBER;
                message->buf[0] = EUSART_packet_data[2];
                message->buf[1] = EUSART_packet_data[3];
                message->buf[2] = EUSART_packet_data[4];
                message->buf[3] = EUSART_packet_data[5];
                message->buf[4] = EUSART_packet_data[6];
                message->buf[5] = EUSART_packet_data[7];
                if(EUSART_packet_data[1] & 0b00000001)
                {
                    message->buf[7] = (EUSART_packet_data[1] & 0b00000001U);
                }
                else
                {
                    message->buf[7] = 0x02;
                }
                if((EUSART_packet_data[8] & 0b00000011) || !(EUSART_packet_data[1] & 0b00000011))
                {
                    CAN_ID_switch = MANUFACTURE_DATE_TURN;
                }
            }
            else
            {
                message->id |= CANADDR_HMI_MANUFACTURE_DATE;
                message->buf[0] = EUSART_packet_data[11];
                message->buf[1] = EUSART_packet_data[12];
                message->buf[2] = EUSART_packet_data[13];
                if(EUSART_packet_data[8] & 0b00000001)
                {
                    message->buf[7] = (EUSART_packet_data[1] & 0b00000001U);
                }
                else
                {
                    message->buf[7] = 0x02;
                }
                if((EUSART_packet_data[1] & 0b00000011) || !(EUSART_packet_data[8] & 0b00000011))
                {
                    CAN_ID_switch = SERIAL_NUMBER_TURN;
                }
            }
            break;
        case SERVICE_LOG_SLAVE_ADDRESS:
            message->id |= CANADDR_HMI_SERVICE_LOG;
            
            message->buf[1] = (EUSART_packet_data[1] & 0b00000010U);
            message->buf[2] = EUSART_packet_data[2];
            message->buf[3] = EUSART_packet_data[3];
            message->buf[4] = EUSART_packet_data[4];
            message->buf[5] = EUSART_packet_data[5];
            break;
    }
}