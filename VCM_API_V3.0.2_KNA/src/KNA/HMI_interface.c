#include "api.h"
#include "test.h"
#include "timer.h"
#include "../app.h"
#include "HMI_interface.h"
#include "t_rex_operation.h"




// time between sending HMI LED Update CAN messages
#define HMI_UPDATE_LED_TIMER_MS 100
#define MODE_OF_OPERATION            0b00000111 // 000=off, 001=on, 010=service, 011=transport, 100=deed, 101=eco, 110=acc
#define DIRECTION_bits               0b00011000 // 00=none, 01=forward, 10=reverse
#define NODIRECTION                  0b00000000
#define DIRECTION_FORWARD            0b00001000
#define DIRECTION_REVERSE            0b00010000
#define HORN_bit                     0b10000000 // 1 = pressed, 0 = released

#define MOTOR                        0b11100000





#define SERIAL_NUMBER_5              0x55  // msb
#define SERIAL_NUMBER_4              0x54
#define SERIAL_NUMBER_3              0x43
#define SERIAL_NUMBER_2              0x32
#define SERIAL_NUMBER_1              0x21
#define SERIAL_NUMBER_0              0x00   //lsb

#define SOFTWARE_VERSION             0x0F
#define SOFTWARE_REVISION            0xAB


//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Local variables


static MS_TICK _updateHmiLedStart;
static uint8_t _Mode_of_operation = MODE_OFF;
static uint8_t _MOTOR_INDEX;
static DIRECTION _DIRECTION = NO_DIRECTION;
static DIRECTION _DIRECTION_LED_INDICATION = NO_DIRECTION;
bool _Horn = false;

//------------------------------//----------------------------------------------
// Local functions

void set_operating_mode(uint8_t mode);
uint8_t update_can_message_80h_byte_3(void);




uint8_t get_current_direction(void){
    return _DIRECTION;
}

uint8_t get_operating_mode(void){
    return _Mode_of_operation;
}

void set_operating_mode(uint8_t mode){
    _Mode_of_operation = mode;
}

/**
 * Send out an LED message (dest HMI)
 *
 */
void _SendLeds( uint32_t leds )
{
    uCAN_MSG msg;

    msg.frame.id = 0x10000000 + ( (uint32_t)CAN_PGN_VCM_LEDS << 8 ) + CANADDR_VCM;
    msg.frame.idType = CAN_FRAME_EXT;
    msg.frame.dlc = 4;
    msg.frame.data0 = ( uint8_t )( leds >> 24 );
    msg.frame.data1 = ( uint8_t )( leds >> 16 );
    msg.frame.data2 = ( uint8_t )( leds >> 8 );
    msg.frame.data3 = (uint8_t)leds;

    API_SendCanMessage( &msg );
}

/**
 * Send out an LED message (dest HMI)
 *
 */
void _SendStatus3()
{
    uCAN_MSG msg;

    msg.frame.id = 0x10000000 + ( (uint32_t)CAN_PGN_VCM_STATUS3 << 8 ) + CANADDR_VCM;
    msg.frame.idType = CAN_FRAME_EXT;
    msg.frame.dlc = 6;

    msg.frame.data0 = ( uint8_t )( API_ReadAin0() >> 2 );
    msg.frame.data1 = ( uint8_t )( API_ReadAin1() >> 2 );
    msg.frame.data2 = ( uint8_t )( API_ReadThrottle() >> 2 );
    msg.frame.data3 = ( uint8_t )( API_ReadBattLevel() >> 2 );
    msg.frame.data4 = ( uint8_t )( API_ReadActIsense() >> 2 );
    msg.frame.data5 = ( uint8_t )( API_ReadBrkSense() >> 2 );

    API_SendCanMessage( &msg );
}



/**
 * Send out an LED message (dest HMI)
 *
 */
void _SendStatus1()
{
    uCAN_MSG msg;

    msg.frame.id = 0x10000000 + ( (uint32_t)CAN_PGN_VCM_STATUS1 << 8 ) + CANADDR_VCM;
    msg.frame.idType = CAN_FRAME_EXT;
    msg.frame.dlc = 3;

    uint16_t word = API_GetDinputWord();

    msg.frame.data0 = ( uint8_t )( word );
    msg.frame.data1 = ( uint8_t )( word >> 8 );

    uint8_t byte;
    byte = ACT_EN_IO_RE12_GetValue();
    byte |= ( ACC_En_IO_RE14_GetValue() << 1 );
//    byte |= ( DMOD_EN_IO_RF0_GetValue() << 2 );
//    byte |= ( AUX_OUT1_IO_RC0_GetValue() << 3 );
    byte |= ( AUX_OUT2_IO_RC1_GetValue() << 4 );
    byte |= ( AUX_OUT3_IO_RC2_GetValue() << 5 );
    byte |= ( Horn_Out_uC_IO_RB13_GetValue() << 6 );

    // add contactor output here
    byte |= ( 0 << 7 );

    msg.frame.data2 = byte;

    API_SendCanMessage( &msg );
}


//Cleaning Mode 
//000=off, 001=on, 010=service, 011=transport, 100=deed, 101=eco, 110=acc
void handle_mode_message_81h(uCAN_MSG pMsg){
    uint8_t operating_mode;
    uint8_t direction;
    operating_mode = pMsg.frame.data0 & MODE_OF_OPERATION;
    

    set_operating_mode(operating_mode);
    set_horn( pMsg.frame.data0 & HORN_bit);
    direction = pMsg.frame.data0 & DIRECTION_bits;
    if( pMsg.frame.data0 & DIRECTION_FORWARD){
        set_direction(FORWARD);
    }
    else if( pMsg.frame.data0 & DIRECTION_REVERSE){
        set_direction(REVERSE);
    }  
    else{
        set_direction(NO_DIRECTION);
    }

    
//    if(operating_mode == MODE_MAINTAINACE){
//        set_motor(pMsg.frame.data1 & MOTOR);
//        set_motor_pwm(pMsg.frame.data1,pMsg.frame.data2);
//    }
}

void handle_hmi_sn_message(uCAN_MSG *pMsg){
    
}

bool set_motor(uint8_t message){
    uint8_t motor;
    
    motor = message >> 5;
    _MOTOR_INDEX = motor;
    return true;
}


uint8_t get_motor(void){
    return _MOTOR_INDEX;
}


void set_direction(DIRECTION new_direction){
    _DIRECTION = new_direction;
}

DIRECTION get_direction(void){
    DIRECTION temp;
    temp =_DIRECTION;
    _DIRECTION = NO_DIRECTION;
    return temp;
}

void set_led_direction(DIRECTION new_direction){
    _DIRECTION_LED_INDICATION = new_direction;
}

DIRECTION get_led_direction(void){
    return _DIRECTION_LED_INDICATION;
}

void set_horn(bool horn){
    _Horn = horn;
}

bool get_horn(void){
    return _Horn;
}



/**
 * Initialize the HMI LED system
 */
void HMI_Led_System_Initialize( void )
{

    _updateHmiLedStart = TIMER_Now();
}



/**
 * Run the task loop for the test - send LED message periodically to the HMI
 */
void TEST_Can1HmiLed_Tasks( void )
{
    MS_TICK _updateHmiLedElapsed;

    _updateHmiLedElapsed = TIMER_Since( _updateHmiLedStart );

    if( _updateHmiLedElapsed >= HMI_UPDATE_LED_TIMER_MS )
    {
        // _SendLeds( _ledMap );
        _SendStatus3();
        _SendStatus1();
        _updateHmiLedStart = TIMER_Now();
    }
}



void update_motor_status(MOTOR_STATUS_MSG motor_data, uint16_t message_ID){
    uCAN_MSG msg;

    msg.frame.id = message_ID;
    msg.frame.msgtype = CAN_MSG_DATA;  
    msg.frame.idType = CAN_FRAME_STD;
    msg.frame.dlc = 4;
    msg.frame.data0 = ( uint8_t )motor_data.array[0];
    msg.frame.data1 = ( uint8_t )motor_data.array[1];
    msg.frame.data2 = ( uint8_t )motor_data.array[2];
    msg.frame.data3 = ( uint8_t )motor_data.array[3];
    API_SendCanMessage( &msg );
}


//data3
#define BATTERY_CHARGE_WARNING      0b10000000
#define SERVICE_SOON                0b01000000
#define CHANGE_PATH_WARNING         0b00100000
#define SIDE_BRUSH_WARNING          0b00010000
#define BRUSH_WARNING               0b00001000
#define TRACTION_SPEED_MASK         0b00000111


// data4
#define BATTERY_UNKNOWN             0b00000000
#define BATTERY_LITHIUM             0b01000000
#define BATTERY_AGM                 0b10000000
#define BATTERY_FLOODED             0b11000000
#define OPERATOR_PRESENT            0b00100000
#define NOT_OPERATOR_PRESENT        0b00000000
#define HOURS_CHARGE_REMAINING_MASK 0b00011111


typedef union {
    struct {
        uint8_t hour_0;
        uint8_t hour_1;
        uint8_t hour_2;
        uint8_t hour_3;
    } time;
    uint32_t hours;
} uHOURS;


// this should be from the battery.c
bool battery_charge_warning(void){
    return false;
}
// hour meter
bool service_soon(void){
    return false;
}
//??
bool change_path_warning(void){
    return false;
}
// check over current on the dmc
bool side_brush_warning(void){
    return false;
}

bool brush_warning(void){
    return false;
}


uint8_t get_battery_type(void){
    return BATTERY_AGM;          // hardcode for now
}

uint8_t get_operator(void){
    API_OpPresence_e status;
    status = get_operator_status();
    if(OPERATOR_ON  == status)
        return OPERATOR_PRESENT;
    else
        return NOT_OPERATOR_PRESENT;
}

uint8_t get_remaing_charge(void){
    static int8_t charge_level = 0b00011111;   //31
    static uint8_t update_speed = 0;
    update_speed++;
    if(update_speed>10){
        update_speed = 0;
        charge_level--;
        if(charge_level<0){
            charge_level = 32;
        } 
    }
    
    return (uint8_t)charge_level;          // hardcode for now
}

uint8_t update_can_message_80h_byte_3(void){
    uint8_t data_byte_3 = 0;
    uint8_t motor_pwm;
    
    if(battery_charge_warning())      
        data_byte_3 |= BATTERY_CHARGE_WARNING;
    if(service_soon())               
        data_byte_3 |= SERVICE_SOON; 
    if(change_path_warning())
        data_byte_3 |= CHANGE_PATH_WARNING;
    if(side_brush_warning())
        data_byte_3 |= SIDE_BRUSH_WARNING;
    if(brush_warning())
        data_byte_3 |= BRUSH_WARNING;          
    
    API_CanMotorPwm( INDEX_TRACTION, &motor_pwm );

    motor_pwm >>= 5;
    motor_pwm &= TRACTION_SPEED_MASK;
    data_byte_3 |= motor_pwm;
    
    return data_byte_3;
    
}


uint8_t update_can_message_80h_byte_4(void){
    uint8_t data_byte_4 = 0;  // 0xA0;

    data_byte_4 |= get_battery_type();
    data_byte_4 |= get_operator();
    data_byte_4 |= get_remaing_charge();  
    return data_byte_4;
}

uint32_t update_machine_time(void){
    static uint8_t machine_hours = 0;
    machine_hours++;
    return machine_hours;
}


void update_status_message_8001h(void){
    
    uint8_t data3;
    uint8_t data4;
    uHOURS machine_hours;
    uCAN_MSG msg;
    
    data3 = update_can_message_80h_byte_3();
    data4 = update_can_message_80h_byte_4();
    machine_hours.hours =  update_machine_time();
    
    msg.frame.id = 0x10FF8000;
    msg.frame.msgtype = CAN_MSG_DATA;  
    msg.frame.idType = CAN_FRAME_EXT;
    msg.frame.dlc = 8;
    msg.frame.data0 = ( uint8_t )0x00;
    msg.frame.data1 = ( uint8_t )0x00;
    msg.frame.data2 = ( uint8_t )0x00;
    msg.frame.data3 = ( uint8_t )data3; 
    msg.frame.data4 = ( uint8_t )data4;  
    msg.frame.data5 = ( uint8_t )machine_hours.time.hour_2;
    msg.frame.data6 = ( uint8_t )machine_hours.time.hour_1;
    msg.frame.data7 = ( uint8_t )machine_hours.time.hour_0;   
    API_SendCanMessage( &msg );
}

void update_status_message_8002h(void){
    
    uCAN_MSG msg;
    DIRECTION direction;
    uint8_t direction_message = 0;
    
    direction = get_led_direction();
    
    if(direction == REVERSE){
        direction_message |= 0b00010000;
    }else if(direction == FORWARD)
        direction_message |= 0b00001000;
    else
        direction_message &= ~0b00011000;
    
    msg.frame.id = 0x10FF8002;
    msg.frame.msgtype = CAN_MSG_DATA;  
    msg.frame.idType = CAN_FRAME_EXT;
    msg.frame.dlc = 1;
    msg.frame.data0 = ( uint8_t )direction_message;
     
    API_SendCanMessage( &msg );
}

  
void update_serial_number_software_ver_rev(void){
    uCAN_MSG msg;
    msg.frame.id = CAN_VCM_HMI_1_MESSAGE;
    msg.frame.msgtype = CAN_MSG_DATA;  
    msg.frame.idType = CAN_FRAME_STD;
    msg.frame.dlc = 8;
    msg.frame.data0 = ( uint8_t )SOFTWARE_REVISION;
    msg.frame.data1 = ( uint8_t )SOFTWARE_VERSION;
    msg.frame.data2 = ( uint8_t )SERIAL_NUMBER_0;   // lsb
    msg.frame.data3 = ( uint8_t )SERIAL_NUMBER_1;
    msg.frame.data4 = ( uint8_t )SERIAL_NUMBER_2;
    msg.frame.data5 = ( uint8_t )SERIAL_NUMBER_3;
    msg.frame.data6 = ( uint8_t )SERIAL_NUMBER_4;
    msg.frame.data7 = ( uint8_t )SERIAL_NUMBER_5;   // msb
    API_SendCanMessage( &msg );
}