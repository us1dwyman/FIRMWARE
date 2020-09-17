/**

 *
 */

#include "api.h"
#include "../app.h"
#include "test.h"
#include "timer.h"
#include "../KNA/battery_charger.h"
#include "../KNA/t_rex_operation.h"
#include "Quality_testing.h"
#include "../KNA/deck_actuator.h"
#include "brake.h"
#include "HMI_interface.h"
#include "micro_actuator.h"
//
////------------------------------//----------------------------------------------
//// Definitions
//


// Constants for traction state machine

// time delay between release of em brake and throttle application
#define TRACTION_BEGIN_MOVEMENT_TIMER_MS 250

// once pwm goes to 0, time delay before applying brake
#define TRACTION_END_MOVEMENT_TIMER_MS 1000

#define DECK_UP_MOVEMENT_TIMER_MS 10000

// if no operator detected, delay before applying brake
#define TRACTION_NO_OPERATOR_TIMER_MS 1000

// throttle ADC thresholds
#define TRACTION_THROTTLE_RELEASE_THRESHOLD 10    //
#define TRACTION_THROTTLE_APPLIED_THRESHOLD 15

// 1 second vacuum off delay
#define VACUUM_TURN_OFF_DELAY         2000 



//// time between sending HMI LED Update CAN messages
//#define HMI_UPDATE_LED_TIMER_MS 50
//


#define LOW_BATTERY_MASK      0x0001    // place holder
#define CRITICAL_BATTERY_MASK 0x0002
#define CHARGER_MASK          0x0004
#define COVER_REMOVED         0x0008        
#define KEY_SWITCH            0x0010    // keyswitch off = 0???
#define OPERATOR_PRESENT      0x0020    // switch failure







//
///**
// * define Traction Subsystem states
// */
typedef enum
{
    TRACTION_QUIESCENT = 0,
    TRACTION_BEGIN_NO_OPERATOR,
    TRACTION_NO_OPERATOR,
    TRACTION_OPERATOR,
    TRACTION_DIRECTION_INDICATED,
    TRACTION_STOPPED,
    TRACTION_BEGIN_MOVEMENT,
    TRACTION_MOVING,
    TRACTION_CHANGE_DIRECTION,
    TRACTION_BEGIN_STOP,
    TRACTION_HANDLE_FAULT,
    TRACTION_FAULT,
} TractionSystemState_e;






///**
// * define Traction Subsystem Status
// */
typedef struct
{
    TractionSystemState_e state;
    API_OpPresence_e opPres;
    uint32_t throttle;
    uint32_t maxSpeed;
    uint8_t pwm;
} TractionSystemStatus_t;


//
////------------------------------//----------------------------------------------
//// Global variables
//
////------------------------------//----------------------------------------------
//// Local variables
//
// A rolling counter of pings we send out
static uint8_t _txPingCount = 0;

//// to make sure we only report the test result once
static bool _testComplete = false;


static uint16_t _MOTOR_PWM;

//// Describes the Phoenix motor devices we expect to talk to
const CanMotorConfig_t _tractionMotorConfig = {CANADDR_TRACTION, true};
const CanMotorConfig_t _brushMotorConfig = {CANADDR_BRUSH, true};
const CanMotorConfig_t _vacuumMotorConfig = {CANADDR_VACUUM, false};
const CanMotorConfig_t _sidebroomMotorConfig = {CANADDR_SIDEBROOM, true};

//// Keep track of whether an operator is on the scrubber

TractionSystemStatus_t TRACTION_Status;
static MS_TICK _tractionBeginMovementStart;
static MS_TICK _tractionEndMovementStart;
static MS_TICK _tractionNoOperatorStart;
static MS_TICK _noDirectionIndicatedElapsed;
MS_TICK vacuum_off_delay_time_stamp;
MS_TICK decklift_off_delay_time_stamp;
bool _turn_off_vacuum = false;
static uint16_t PREVIOUS_Throttle_BRUSH = 0;
static uint16_t PREVIOUS_Throttle_VACUUM = 0;
static uint16_t PREVIOUS_Throttle_SIDEBROOM = 0;
static API_OpPresence_e PREVIOUS_operator = OPERATOR_NO_EVENT;



////------------------------------//----------------------------------------------
//// Local functions
//
//
void stop_machine(bool);
void reset_traction_motor(void);
void traction_motor_Task( uint8_t,uint32_t,uint32_t, bool, API_OpPresence_e);
void stop_machine_task(void);


// Sends out a CAN message with the PING identifier
// Increments a sequence counter
// this is an example of sending a standard (11-bit) message
static void _SendPing( void )
{
    // Send out a PING message
    uCAN_MSG msg;

    msg.frame.id = CAN_PING;
    msg.frame.idType = CAN_FRAME_STD;
    msg.frame.dlc = 1;
    msg.frame.data0 = _txPingCount++;

    API_SendCanMessage( &msg );
}

//------------------------------//----------------------------------------------
// Public functions

/**
 * New message received on CAN bus 1
 *
 * The API will call this function when a CAN message matching the configured
 * filters is received.
 *
 */
void APICB_CanRx( uCAN_MSG *pMsg )
{
    if( CAN_FRAME_STD == pMsg->frame.idType )
    {
        // 11-bit identifiers
        switch( pMsg->frame.id )
        {

            case 0x549:
                TRACE(("HEARTBEAT\r\n"));
                break;

           case 0x4C9:
                TRACE(("COMMAND\r\n"));
                break;
                
                
            case CAN_PING:
                // another device pinging us. Respond immediately
                pMsg->frame.id = CAN_PONG;
                pMsg->frame.data0++;
                API_SendCanMessage( pMsg );
                break;

            case CAN_PONG:
                // If we got a ping response, send another one
                _SendPing();

                if( !_testComplete )
                {
                    // report that the test passed
                    TEST_Complete( "CAN1", true );
                    _testComplete = true;
                }
                break;
            case TPD02:
            case TPD03:
            case TPD04:
                handle_charger_can_messages( pMsg );
//                API_SendCanMessage( pMsg );  // simple echo message
//                TRACE(("get test message\r\n"));
                break;
            case CAN_HMI_VCM_0_ID:
                TRACE(("hmi MODE MESSAGE\r\n"));
                //handle_mode_message( pMsg );
                break;
            default:
                break;
        }
    }
    else if( CAN_FRAME_EXT == pMsg->frame.idType )
    {
        // 29 bit identifiers
        uint8_t ID = pMsg->frame.id & 0xFF;
        uint16_t pgn = ( pMsg->frame.id >> 8 ) & 0xFFFF;

        switch( pgn )
        {
            case CAN_PGN_HMI_MESSAGES:            // DRW need to fix this
                if( CAN_HMI_VCM_0_ID == ID )
                {
                    // message from the HMI
//                    handle_mode_message( pMsg );
                }
                if( CAN_HMI_VCM_1_ID == ID ){
                    handle_hmi_sn_message(pMsg);
                }
                if( CAN_HMI_VCM_2_ID == ID ){
                    // handle_xxx_message(pMsg);
                }
                break;

            default:
                break;
        }
    }
}


void get_fault(uint16_t * fault_condition){
       *fault_condition = 0x0000;   // no fault
}



///*
// This function should be called from the main loop
// need to keep track of the current mode of operation. when the mode of operation 
// changes it is only updated when the machine stops.  When transitioning to a new 
// task the machine needs to be in a stopped condition where all the motors are in a 
// known stopped condition*/
void machine_task(uint8_t mode, uint32_t effective_throttle,uint32_t throttle, bool fault, API_OpPresence_e operator)
{
    static uint8_t current_mode = MODE_OFF;
 
    if(current_mode != mode){        // look for mode changes
        //stop_machine_flag = true;    // this is set true for the default case
        if(current_mode == MODE_TOOL){ 
            stop_machine(VACUUM_DELAY);
        }
            
        current_mode = mode;
        if((mode != MODE_TRANSPORT )
                &&(mode != MODE_ECO )
                &&(mode != MODE_DEEP)){
            stop_machine(VACUUM_DELAY);
        }
    }
    
    switch(current_mode){
        case MODE_ECO:
        case MODE_TRANSPORT:
        case MODE_DEEP:
            traction_motor_Task(current_mode,effective_throttle,throttle,fault,operator);  // all modes related to movement 
            break;
        case MODE_TOOL:
            micro_actuator_task(fault,operator);
            break;
        case MODE_WELCOME:
        case MODE_SERVICE:
        case MODE_OFF:
        default:
            stop_machine_task();
            break;  
    }
}

/****************************************************************************
  Function:
      void stop_machine(void) 

  Description:  This function puts the machine in a stopped state.
  Precondition:
   CAN bus is set up and working 
  Parameters:
   none
  Return Values:
   none
  Remarks:
 none
 ****************************************************************************/
void stop_machine(bool vac_delay){
    API_CanMotorSetThrottle( INDEX_TRACTION, 0 );
    API_CanMotorSetThrottle( INDEX_BRUSH, 0 );
    API_CanMotorSetThrottle( INDEX_SIDEBROOM, 0 );
   
    set_deck_position(DECK_POSITION_UP);
    turn_off_vacuum(vac_delay);
    apply_brake();
    reset_traction_motor();    // reset throttle values 
    set_led_direction(NO_DIRECTION);
    close_gate();            // close the vacuum port
}


void stop_machine_task(void){
    static MS_TICK stop_machine_task_time_stamp = 0;
    MS_TICK elapsed_time;

    elapsed_time = TIMER_Since( stop_machine_task_time_stamp );
    if(elapsed_time > 250){
        stop_machine(NO_VACCUUM_DELAY);
        stop_machine_task_time_stamp = TIMER_Now();
    }
        
     
}
   


/**
 * Initialize the traction system
 */
void TRACTION_System_Initialize( void )
{
    TRACTION_Status.state = TRACTION_QUIESCENT;
    TRACTION_Status.opPres = OPERATOR_NO_EVENT;
    TRACTION_Status.throttle = 0;
    TRACTION_Status.maxSpeed = 0;

    // note, less than 100 is not supported by Prototype 1 hardware
    API_ConfigureBrake( 100 );

    // let us set brake.
    API_SetBrake();
}

void T_rex_Can1_Initialize( void )
{

    // kick off the ping-ponging with a single message
    _SendPing();

    API_CanMotorEnable( INDEX_TRACTION, &_tractionMotorConfig );

    API_CanMotorEnable( INDEX_BRUSH, &_brushMotorConfig );

    API_CanMotorEnable( INDEX_VACUUM, &_vacuumMotorConfig );

    API_CanMotorEnable( INDEX_SIDEBROOM, &_sidebroomMotorConfig );

    TRACTION_System_Initialize();
    
 //   HMI_Led_System_Initialize();
}


 

void reset_traction_motor(void){
    
    PREVIOUS_Throttle_BRUSH = 0;
    PREVIOUS_Throttle_VACUUM = 0;
    PREVIOUS_Throttle_SIDEBROOM = 0;
    //PREVIOUS_operator = OPERATOR_NO_EVENT;
    //TRACTION_Status.state = TRACTION_QUIESCENT;
    
}



///****************************************************************************
//  Function:
//      bool traction_motor_Task(uint8_t direction)
//
//  Description:  This function controls the main operation of the machine.  This 
// * task steps through the states of the machine depending on the direction, speed,
// * operator presence,and overcurrent motor fault.  Depending on the state the motor
// * will be driven according to inputs from the speed actuator and speed limit pot.  
// * The brake will only be engaged and disengaged when the machine is not moving.
//  Precondition:
//   brake should be set, machine in parked non active state 
//  Parameters:
//   IN uint8_t mode_of_operation  all modes relating to movement 
//   IN uint32_t effectiveThrottle_TRACTION  derived from the max limit and throttle
//   IN uint32_t throttle,     input from the foot throttle
//   IN uint16_t fault  0=no fault, fault otherwise
//   in uint8_t operator  present,not present, switch fault
//  Return Values:
//   none
//  Remarks:
// variables are accessed with get functions
// ****************************************************************************/
void traction_motor_Task(uint8_t mode_of_operation,
                         uint32_t effectiveThrottle_TRACTION,
                         uint32_t throttle,
                         bool fault,
                         API_OpPresence_e operator)
{
    uint16_t Throttle_BRUSH;
    uint16_t Throttle_VACUUM;
    uint16_t Throttle_SIDEBROOM;
    MS_TICK _endMovementElapsed;
    MS_TICK _beginMovementElapsed;
    MS_TICK _noOperatorElapsed;
    MS_TICK _elapsed_time;
    static bool pwmZeroFlag = false;
    uint8_t Direction; //
    static bool last_fault = false;
    uint16_t brake_current;
    
    
    if( ADC_hasNewSample )
    {
        brake_current = API_ReadBrkSense();
        ADC_hasNewSample = false;
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // test for the case when the operator steps off the machine 
        // or if there is an operator present switch fault.
        if(fault != last_fault)
        {
            last_fault = fault;
            if(fault){           
                TRACE( ( "TRACTION - General: moving to: TRACTION_HANDLE_FAULT\r\n" ) );
                TRACTION_Status.state = TRACTION_HANDLE_FAULT;
            }  
        }
        if(operator != PREVIOUS_operator ){
            PREVIOUS_operator = operator;
            if(( operator != OPERATOR_ON) && (operator != OPERATOR_NO_EVENT)){
            TRACE( ( "TRACTION - General: NO operator: TRACTION_HANDLE_FAULT\r\n" ) );
                TRACTION_Status.state = TRACTION_HANDLE_FAULT;
            }
        }
        
        if( true != API_CanMotorPwm( INDEX_TRACTION, &TRACTION_Status.pwm ) )
        {
            TRACE( ( "TRACTION - False return from API_CanMotorPwm\r\n" ) );
            TRACTION_Status.pwm = 0;
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        
        switch( TRACTION_Status.state )
        {
            case TRACTION_QUIESCENT: 
                // do not leave until change in operator status
                if( operator != OPERATOR_NO_EVENT )  
                {
                    if( operator == OPERATOR_ON)
                    {
                        TRACE( ( "TRACTION - TRACTION_QUIESCENT: moving to: TRACTION_OPERATOR\r\n" ) );
                        TRACTION_Status.state = TRACTION_OPERATOR;
                        _noDirectionIndicatedElapsed = TIMER_Now();
                    }
                    else
                    {
                        TRACE( ( "TRACTION - TRACTION_QUIESCENT: moving to: TRACTION_NO_OPERATOR\r\n" ) );
                        TRACTION_Status.state = TRACTION_NO_OPERATOR;
                    }
                }
                break;
            case TRACTION_BEGIN_NO_OPERATOR:                      
                _noOperatorElapsed = TIMER_Since( _tractionNoOperatorStart );
                if( _noOperatorElapsed >= TRACTION_NO_OPERATOR_TIMER_MS )
                {
                    if(TRACTION_Status.pwm == 0){
                        apply_brake();
                        TRACE( ( "TRACTION - TRACTION_BEGIN_NO_OPERATOR: moving to: TRACTION_NO_OPERATOR\r\n" ) );
                        TRACTION_Status.state = TRACTION_NO_OPERATOR;
                    }
                }
                break;
            case TRACTION_NO_OPERATOR:                         
                // stay in this state until operator is present
                if( operator == OPERATOR_ON )
                {
                    TRACE( ( "TRACTION - TRACTION_NO_OPERATOR: moving to: TRACTION_OPERATOR\r\n" ) );
                    TRACTION_Status.state = TRACTION_OPERATOR;
                    get_direction();  // this will clear out any previous button presses.
                    _noDirectionIndicatedElapsed = TIMER_Now();
                }
                break;
            case TRACTION_OPERATOR:
                // do nothing until throttle released - catch the case where throttle
                // pressed as operator jumps on
                
                switch(mode_of_operation){
                    case MODE_TRANSPORT:  
                        Throttle_VACUUM = 0;
                        set_deck_position(DECK_POSITION_UP);
                        break;
                    case MODE_DEEP:
                        Throttle_VACUUM = THROTTLE_VACUUM;
                        set_deck_position(DECK_POSITION_DOWN);
                        break;
                    case MODE_ECO:
                        Throttle_VACUUM = THROTTLE_VACUUM_ECO;
                        set_deck_position(DECK_POSITION_DOWN);
                        break;
                    default:
                        break;   
                    }
                _elapsed_time = TIMER_Since(_noDirectionIndicatedElapsed);
                if(_elapsed_time > 30000){
                    Throttle_VACUUM = 0;
                    API_CanMotorSetThrottle( INDEX_VACUUM, Throttle_VACUUM );
                    set_deck_position(DECK_POSITION_UP);
                }else{
                    API_CanMotorSetThrottle( INDEX_VACUUM, Throttle_VACUUM );
                }
                    
                
                
                if( throttle < TRACTION_THROTTLE_RELEASE_THRESHOLD )
                {
                    TRACTION_Status.state = TRACTION_DIRECTION_INDICATED;
                        
                }
                break;
             case TRACTION_DIRECTION_INDICATED:
                // stay here until direction is pressed 
                Direction = get_direction();
                set_led_direction(Direction);
                if(Direction != NO_DIRECTION){
                    TRACE( ( "TRACTION - TRACTION_OPERATOR: moving to: TRACTION_STOPPED\r\n" ) );
                    TRACTION_Status.state = TRACTION_STOPPED;
                    API_CanMotorSetDirection(INDEX_TRACTION,Direction);  
                }
                break;
            case TRACTION_STOPPED:
                if( throttle > TRACTION_THROTTLE_APPLIED_THRESHOLD )
                {
                    TRACE( ( "TRACTION - TRACTION_STOPPED: moving to: TRACTION_BEGIN_MOVEMENT\r\n" ) );
                    TRACTION_Status.state = TRACTION_BEGIN_MOVEMENT;
                    release_brake();
                    _tractionBeginMovementStart = TIMER_Now();
                }
                
//                _endMovementElapsed = TIMER_Since( _tractionEndMovementStart );    
//                if( _endMovementElapsed >= DECK_UP_MOVEMENT_TIMER_MS )
//                    set_deck_position(DECK_POSITION_UP);
                break;
            case TRACTION_BEGIN_MOVEMENT:  
                // if throttle is released, fall back to stopped
                if( throttle < TRACTION_THROTTLE_RELEASE_THRESHOLD )
                {
                    TRACE( ( "TRACTION - TRACTION_BEGIN_MOVEMENT: moving to: TRACTION_STOPPED\r\n" ) );
                    TRACTION_Status.state = TRACTION_STOPPED;
                    apply_brake();
                }
                else
                {
                    // when the begin movement expires move to MOVING state where
                    // non-zero throttle is communicated to controller
                    _beginMovementElapsed = TIMER_Since( _tractionBeginMovementStart );

                    if( _beginMovementElapsed >= TRACTION_BEGIN_MOVEMENT_TIMER_MS )          // this gives time to release the brake
                    {
                        if(brake_current < 200){
                           TRACE( ( "TRACTION - TRACTION_BEGIN_MOVEMENT: moving to: Fault\r\n" ) );
                           TRACTION_Status.state = TRACTION_HANDLE_FAULT;
                        } else {
                            TRACE( ( "TRACTION - TRACTION_BEGIN_MOVEMENT: moving to: TRACTION_MOVING\r\n" ) );
                            TRACTION_Status.state = TRACTION_MOVING;
                        } 
                    }
                }
                break;
            case TRACTION_MOVING: 
                if(brake_current < 200){
                    TRACE( ( "TRACTION - TRACTION_BEGIN_MOVEMENT: moving to: Fault\r\n" ) );
                    TRACTION_Status.state = TRACTION_HANDLE_FAULT;
                } else {
                    switch(mode_of_operation){
                        case MODE_TRANSPORT:  
                            Throttle_BRUSH =  0;
                            Throttle_VACUUM = 0;
                            Throttle_SIDEBROOM = 0;
                            set_deck_position(DECK_POSITION_UP);
                            break;
                        case MODE_DEEP:
                            Throttle_BRUSH = THROTTLE_BRUSH;
                            Throttle_VACUUM = THROTTLE_VACUUM;
                            Throttle_SIDEBROOM = THROTTLE_SIDEBRUSH;
                            set_deck_position(DECK_POSITION_DOWN);
                            break;
                        case MODE_ECO:
                            Throttle_BRUSH = THROTTLE_BRUSH_ECO;
                            Throttle_VACUUM = THROTTLE_VACUUM_ECO;
                            Throttle_SIDEBROOM = THROTTLE_SIDEBRUSH_ECO;
                            set_deck_position(DECK_POSITION_DOWN);
                            break;
                        default:
                            break;   
                    }
                    // determine if we need to change state based on throttle releasing
                    if( throttle < TRACTION_THROTTLE_RELEASE_THRESHOLD )
                    {
                        TRACE( ( "TRACTION - TRACTION_MOVING (throttle caused): moving to: TRACTION_BEGIN_STOP\r\n" ) );
                        TRACTION_Status.state = TRACTION_BEGIN_STOP;
                        pwmZeroFlag = false;
                    }
                    else
                    {   //only call the API if values have changed, don't overload the can bus!
                        
                        API_CanMotorSetThrottle( INDEX_TRACTION, effectiveThrottle_TRACTION );
                        if(PREVIOUS_Throttle_BRUSH != Throttle_BRUSH){
                           API_CanMotorSetThrottle( INDEX_BRUSH, Throttle_BRUSH );
                           
                           PREVIOUS_Throttle_BRUSH = Throttle_BRUSH;
                        }
                        if(PREVIOUS_Throttle_VACUUM != Throttle_VACUUM ){
                            if(Throttle_VACUUM == 0)
                                turn_off_vacuum(VACUUM_DELAY);
                            else{
                               _turn_off_vacuum = false; // need this to prevent timed turn off
                                API_CanMotorSetThrottle( INDEX_VACUUM, Throttle_VACUUM );
                            }
                            PREVIOUS_Throttle_VACUUM = Throttle_VACUUM;
                        }
                        if(PREVIOUS_Throttle_SIDEBROOM != Throttle_SIDEBROOM){
                            API_CanMotorSetThrottle( INDEX_SIDEBROOM, Throttle_SIDEBROOM );
                            PREVIOUS_Throttle_SIDEBROOM = Throttle_SIDEBROOM;
                        }
                    }
                    Direction = get_direction();
                    if(Direction != NO_DIRECTION){             
                        //TRACTION_Status.state = TRACTION_CHANGE_DIRECTION;
                        API_CanMotorSetDirection(INDEX_TRACTION,Direction);
                        set_led_direction(Direction);
                        TRACE( ( "TRACTION - TRACTION_MOVING: Reversing motor direction\r\n" ) );
                    }
                }  
                break;
            case TRACTION_BEGIN_STOP:
                if(brake_current < 200){
                    TRACE( ( "TRACTION - TRACTION_BEGIN_STOP: moving to: Fault\r\n" ) );
                    TRACE( ( "Fault  brake not released. Motor engaged\r\n" ) );
                    TRACTION_Status.state = TRACTION_HANDLE_FAULT;
                } 
                
                // if throttle is re-applied and pwm still present fall back to MOVING
                else if( throttle > TRACTION_THROTTLE_APPLIED_THRESHOLD )
                {
                    TRACE( ( "TRACTION - TRACTION_BEGIN_STOP: moving to: TRACTION_MOVING\r\n" ) );
                    TRACTION_Status.state = TRACTION_MOVING;
                    API_CanMotorSetThrottle( INDEX_TRACTION, effectiveThrottle_TRACTION );
                }
                else
                {
                    // wait for pwm to go to 0, now we wait for timer before setting brake.
                    API_CanMotorSetThrottle( INDEX_TRACTION, 0 );
                    API_CanMotorSetThrottle( INDEX_BRUSH, 0 );
                    API_CanMotorSetThrottle( INDEX_SIDEBROOM, 0 );

                    PREVIOUS_Throttle_BRUSH = 0;
                    PREVIOUS_Throttle_VACUUM = 0;
                    PREVIOUS_Throttle_SIDEBROOM = 0;
                    
                    if ( false == pwmZeroFlag )
                    { 
                        if ( TRACTION_Status.pwm == 0 )
                        {
                            _tractionEndMovementStart = TIMER_Now();
                            pwmZeroFlag = true;
                        }
                    }
                    else
                    {
                        // when the end movement timer expires move to STOPPED state where
                        // zero throttle is communicated to controller
                        _endMovementElapsed = TIMER_Since( _tractionEndMovementStart );
                        
                        if( _endMovementElapsed >= TRACTION_END_MOVEMENT_TIMER_MS )
                        {
                            TRACE( ( "TRACTION - TRACTION_BEGIN_STOP: moving to: TRACTION_STOPPED\r\n" ) );
 
                            apply_brake();
                            turn_off_vacuum(VACUUM_DELAY);
//                            set_deck_position(DECK_POSITION_UP);
                            TRACTION_Status.state = TRACTION_STOPPED;
                        }
                    }
                }
                break;
            case TRACTION_CHANGE_DIRECTION:
                if(brake_current < 200){
                    TRACE( ( "TRACTION - TRACTION_CHANGE_DIRECTION: moving to: Fault\r\n" ) );
                    TRACTION_Status.state = TRACTION_HANDLE_FAULT;
                } else {
                    TRACTION_Status.state = TRACTION_STOPPED;
//                    API_CanMotorSetDirection(INDEX_TRACTION,Direction);
                    // need to add code to safely deaccelerate over time
                    // need to add a timer. 
                }
                break;
            case TRACTION_HANDLE_FAULT:
                stop_machine(NO_VACCUUM_DELAY);
                TRACTION_Status.state = TRACTION_FAULT;
                break;
            case TRACTION_FAULT:
                if(fault == 0){   
                    TRACTION_Status.state = TRACTION_BEGIN_NO_OPERATOR;
                    _tractionNoOperatorStart = TIMER_Now();
                }
                break;
            default:
                TRACTION_Status.state = TRACTION_FAULT;
                break;
        }
    }
}



void turn_off_vacuum(bool vac_delay){
    _turn_off_vacuum = true;
    if(vac_delay)
        vacuum_off_delay_time_stamp = TIMER_Now();
    else
        vacuum_off_delay_time_stamp = 0;
}




/****************************************************************************
  Function:
      void vacuum_task(void)

  Description:  This function turns off the vacuum 1 second after the machine 
 * is stopped.
  Precondition:
   turn_off_vacuum should be called to enable the delayed turn off. 
  Parameters:
   none
  Return Values:
   none
  Remarks:
 _turn_off_vacuum = false; // need this to prevent timed turn off 
 ****************************************************************************/
void vacuum_task(void){
    MS_TICK elapsed;
   
    if(_turn_off_vacuum){
        elapsed = TIMER_Since( vacuum_off_delay_time_stamp );
        if( elapsed > VACUUM_TURN_OFF_DELAY ){
            _turn_off_vacuum = false;
            API_CanMotorSetThrottle( INDEX_VACUUM, 0 );
        }
    }  
}




void horn_task(void){
    static bool current_horn = false;
    bool horn_enable;
    horn_enable = get_horn();
    if(horn_enable != current_horn){   // check for changes, keep from flooding the can bus
        current_horn = horn_enable; 
        API_HornEnable( horn_enable, (uint8_t) 30 ); 
    }      
}



void set_motor_pwm(uint8_t high_4_bits, uint8_t low_8_bits){
    uint16_t result;
    result = high_4_bits & 0b00001111;
    result <<= 4;
    result |= low_8_bits;
    _MOTOR_PWM = result;  
}


uint16_t get_motor_pwm(void){
    return _MOTOR_PWM;
}

uint32_t get_effective_throttle(void){
    uint32_t effective_throttle;
    uint16_t throttle_pot;
    uint16_t limit;
    
    throttle_pot = API_ReadThrottle();
    limit = API_ReadAin0();
   
    //TRACE (( "Throttle pot...: %d \r\n", (int)throttle_pot )) ;
    //TRACE (( "max speed pot: %d   \r\n",  (int)limit ) );
    
    if(limit > throttle_pot)
        effective_throttle = throttle_pot<<2;
    else
        effective_throttle = limit<<2;
        
    return effective_throttle;
}



typedef enum{
    SEND_MESSAGE_8001 = 0,
    SEND_MESSAGE_8002,
    UPDATE_SERIAL_NUMBER,        
} MESSAGE_STATES;

void update_system_messages_task(void){
    static MS_TICK system_messages = 0;
    static MESSAGE_STATES status_message_states = SEND_MESSAGE_8001;
    MS_TICK elapsed_time;
//    MOTOR_STATUS_MSG test_data;
    static int8_t do_every_second = 10;
    elapsed_time = TIMER_Since( system_messages );
    if(elapsed_time > 100)
    {
        switch(status_message_states){
            case SEND_MESSAGE_8001:
                update_status_message_8001h();
                status_message_states = SEND_MESSAGE_8002;
                break;
            case SEND_MESSAGE_8002:
                update_status_message_8002h();
                status_message_states = UPDATE_SERIAL_NUMBER;
                break;
            case UPDATE_SERIAL_NUMBER:
                if(do_every_second-- == 0){
                    update_serial_number_software_ver_rev();
                    do_every_second = 4;
                }
                status_message_states = SEND_MESSAGE_8001;
                break;
            default:
                status_message_states = SEND_MESSAGE_8001;
                break;
        }
        
//        test_data.frame.current = 0x1122;
//        test_data.frame.temperature = 0012;
//        update_motor_status(test_data, CAN_TRACTION_ID);
//        test_data.frame.current++;
//        test_data.frame.temperature++;
//        update_motor_status(test_data, CAN_BRUSH_ID);
//        test_data.frame.current++;
//        test_data.frame.temperature++;
//        update_motor_status(test_data, CAN_VACUUM_ID);
//        test_data.frame.current++;
//        test_data.frame.temperature++;
//        update_motor_status(test_data, CAN_SIDE_BROOM);
   
        system_messages = TIMER_Now();
    }
}





////////////////////////////// TO BE DELETED /////////////////////////////////




