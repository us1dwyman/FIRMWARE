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
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HMI_INTERFACE_H
#define	XC_HMI_INTERFACE_H

#include <xc.h> // include processor files - each processor file is guarded.  

// TODO Insert appropriate #include <>

// TODO Insert C++ class definitions if appropriate

// TODO Insert declarations

// Comment a function and leverage automatic documentation with slash star star
/**
    <p><b>Function prototype:</b></p>
  
    <p><b>Summary:</b></p>

    <p><b>Description:</b></p>

    <p><b>Precondition:</b></p>

    <p><b>Parameters:</b></p>

    <p><b>Returns:</b></p>

    <p><b>Example:</b></p>
    <code>
 
    </code>

    <p><b>Remarks:</b></p>
 */
// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

//#define CAN_PGN_HMI_MESSAGES     0x10FF1081

#define CAN_PGN_HMI_MESSAGES     0xFF10
#define CAN_VCM_HMI_0_MESSAGE    0x080
#define CAN_HMI_VCM_0_ID         0x081   // RECIEVED MESSAGES
#define CAN_TRACTION_ID         0x082
#define CAN_BRUSH_ID            0x083
#define CAN_VACUUM_ID           0x084
#define CAN_SIDE_BROOM          0x085
#define CAN_PC_0_ID              0x086
#define CAN_VCM_HMI_1_MESSAGE   0x090  
#define CAN_HMI_VCM_1_ID         0x091  // receive  HMI serial number
#define CAN_PC_1_ID              0x096
#define CAN_HMI_VCM_2_ID         0x0A1  // receive service logs
#define CAN_PC_2_ID              0x0A6
#define CAN_PC_3_ID              0x0B6


// handle messages 0x082,0x083,0x084,0x085
typedef union {
    struct {
        uint16_t current;
        uint16_t temperature;
    } frame;
    unsigned char array[4];
} MOTOR_STATUS_MSG;

typedef union {
    struct {
        uint8_t sn_msb_5;
        uint8_t sn_4;
        uint8_t sn_3;
        uint8_t sn_2;
        uint8_t sn_1;
        uint8_t sn_lsb;
        uint8_t software_version;
        uint8_t software_revision;
    } frame;
    unsigned char array[8];
} VCM_SERIAL_NUMBER_MSG;


//000=off, 001=on, 010=service, 011=transport, 100=deed, 101=eco, 110=acc

//typedef enum
//{
//    MODE_OFF = 0,
//    MODE_MAINTAINACE,
//    MODE_TRANSPORT,
//    MODE_DEEP,
//    MODE_ECO,
//    MODE_TOOL,
//} OperationMode_e;


typedef enum
{
    MODE_OFF = 0,
    MODE_WELCOME,
    MODE_SERVICE,
    MODE_TRANSPORT,
    MODE_DEEP,
    MODE_ECO,
    MODE_TOOL,
} OperationMode_e;


typedef enum{
    REVERSE = 0,
    FORWARD,
    NO_DIRECTION,
    STOP,
} DIRECTION;

void TEST_Can1HmiLed_Tasks( void );
void handle_mode_message_81h(uCAN_MSG );
uint8_t get_operating_mode(void);
void set_horn(bool horn);
bool get_horn(void);
void set_direction(DIRECTION);
DIRECTION get_direction(void);
bool set_motor(uint8_t);
uint8_t get_motor(void);
void set_motor_pwm(uint8_t , uint8_t);
uint16_t get_motor_pwm(void);
uint8_t get_current_direction(void);
void update_motor_status(MOTOR_STATUS_MSG, uint16_t);
void update_serial_number_software_ver_rev(void);
void update_status_message_8001h(void);
void update_status_message_8002h(void);
void handle_hmi_sn_message(uCAN_MSG *pMsg);
void set_led_direction(DIRECTION);
DIRECTION get_led_direction(void);
#endif	/* XC_HMI_INTERFACE_H */

