/**
 * Tests the Actuator Output by cycling it up and down
 *
 * The actuator will continue to cycle indefinitely.
 * During the first cycle, we look to see if there is actuator
 * current detected, and declare the test a pass if so
 *
 */

#include "api.h"
#include "test.h"
#include "timer.h"
#include "micro_actuator.h"
#include "t_rex_operation.h"


//------------------------------//----------------------------------------------
// Definitions

//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Local variables


//------------------------------//----------------------------------------------
// Local functions

//------------------------------//----------------------------------------------
// Public functions



typedef enum actuator_position
{ 
    ACTUATOR_OPEN,
    ACTUATOR_MOVING_CLOSE,
    DWELL,
    ACTUATOR_CLOSE,
    ACTUATOR_MOVING_OPEN,
    ACTUATOR_STOPPED,
    DWELL_1,
} MICRO_ACTUATOR_STATE;


typedef enum 
{ 
    ACTUATOR_BRAKE = 0,
    ACTUATOR_MOVING,
} MICRO_ACTUATOR_TASK_STATE;


typedef enum actuator_state_position
{ 
    BRAKE = 0,
    OPEN_VALVE,
    CLOSE_VALVE,
} MICRO_ACTUATOR_POSITION_STATE;


enum actuator_state_position micro_act = BRAKE;
enum actuator_state_position current_direction = BRAKE;


void micro_Actuator_Tasks( void )
{
    static MS_TICK actuator_tick = 0;
    static MICRO_ACTUATOR_TASK_STATE actuator_state = ACTUATOR_BRAKE;
    static uint8_t dwell_count;
    
    MS_TICK elapsed = TIMER_Since( actuator_tick );
    if(elapsed > 100)
    {
        switch(actuator_state){
            case ACTUATOR_BRAKE:
                dwell_count = 0;
                API_AccOut1Brake();
                if(micro_act == OPEN_VALVE){          // out
                    actuator_state = ACTUATOR_MOVING;
                    API_AccOut1Rev();
                    current_direction = OPEN_VALVE;
                } else if(micro_act == CLOSE_VALVE) { // in
                    actuator_state = ACTUATOR_MOVING;
                    API_AccOut1Fwd();
                    current_direction = CLOSE_VALVE;
                }   
                break;
            case ACTUATOR_MOVING:
                if(current_direction != micro_act)
                    actuator_state = ACTUATOR_BRAKE;
                if(dwell_count++ > 10){
                    micro_act = BRAKE;
                    actuator_state = ACTUATOR_BRAKE;
                }
                break;
            default:
                actuator_state = ACTUATOR_BRAKE;
                break;
        }
    actuator_tick = TIMER_Now();
    }
}




bool open_gate(void){
    bool result = false;
    
    //if(micro_act == BRAKE){
        micro_act = OPEN_VALVE;
        result = true;
    //}
    return result;
}



bool close_gate(void){
    bool result = false;
    
    //if(micro_act == BRAKE){
        micro_act = CLOSE_VALVE;
        result = true;
   // }
    return result;
}

///****************************************************************************
//  Function:
//      void micro_actuator_task(void)
//
//  Description:  This function is a task that controls the brake action. This 
//     is a non blocking function and safely engages the brake when the motor
//     is confirmed to be de-energized.  This 
//  Precondition:
//   timer should be active, can bus should be active
//  Parameters:
//   none
//  Return Values:
//   none
//  Remarks:
// 
// ****************************************************************************/      
void micro_actuator_task(bool fault,
                         API_OpPresence_e operator){
    static MS_TICK tool_task_time_stamp = 0;
    MS_TICK elapsed_time;
    uint16_t Throttle_VACUUM;
    
    if((fault == false) && (operator == OPERATOR_ON )){
        Throttle_VACUUM = THROTTLE_VACUUM_TOOL;
        elapsed_time = TIMER_Since( tool_task_time_stamp );
        if(elapsed_time > 250)
        {
            open_gate();
            API_CanMotorSetThrottle( INDEX_VACUUM, Throttle_VACUUM );
            tool_task_time_stamp = TIMER_Now();   
        } 
    }   
}



void test_micro_actuator(void){
    TEST_micro_Actuator_Tasks();
    micro_Actuator_Tasks();
}


/**
 * Run the task loop for the test
 */
void TEST_micro_Actuator_Tasks( void )
{
    static MS_TICK actuator_tick = 0;
    static MICRO_ACTUATOR_STATE actuator_state = ACTUATOR_OPEN;
    
    MS_TICK elapsed = TIMER_Since( actuator_tick );
    if(elapsed > 2000)
    {
        switch(actuator_state){
            case ACTUATOR_OPEN:
                if(open_gate())
                    actuator_state = DWELL;
                break;
            case DWELL:
                actuator_state = ACTUATOR_CLOSE;
                break;
            case ACTUATOR_CLOSE:
                if(close_gate())
                    actuator_state = DWELL_1;
                break;
            case DWELL_1:
                actuator_state = ACTUATOR_OPEN;
                break;
            default:
                actuator_state = ACTUATOR_OPEN;
                break;
        }
    actuator_tick = TIMER_Now();
    }
}


void TEST_micro_Actuator_Tasks1( void )
{
    static MS_TICK actuator_tick = 0;
    static MICRO_ACTUATOR_STATE actuator_state = ACTUATOR_STOPPED;
    

    
    MS_TICK elapsed = TIMER_Since( actuator_tick );
    if(elapsed > 2000)
    {
        switch(actuator_state){
            case ACTUATOR_OPEN:
                API_AccOut1Standby();
                actuator_state = ACTUATOR_MOVING_CLOSE;
                break;
            case ACTUATOR_MOVING_CLOSE:
                API_AccOut1Rev();
                actuator_state = ACTUATOR_CLOSE;
                break;
            case ACTUATOR_CLOSE:
                API_AccOut1Standby();
                actuator_state = ACTUATOR_STOPPED;
                break;
            case ACTUATOR_MOVING_OPEN:
                API_AccOut1Fwd();  // open the gate
                actuator_state = ACTUATOR_OPEN;
                break;
            case ACTUATOR_STOPPED:
                API_AccOut1Brake();
                actuator_state = ACTUATOR_MOVING_OPEN;
                break;
            default:
                break;
        }
    actuator_tick = TIMER_Now();
    }
}


