
#include "api.h"
#include "../app.h"
#include "test.h"
#include "timer.h"
#include "t_rex_operation.h"


typedef enum{ 
    IDLE_BRAKE = 0,
    STOP_TRACTION_MOTOR,
    WAIT_PWM_ZERO,
    BRAKE_DELAY,
    APPLY_BRAKE,
}BRAKE;


#define BRAKE_APPLIED true
#define BRAKE_RELEASED false
#define BRAKE_FAULT true
#define BRAKE_OK  false
bool _set_brake_flag = false;
static uint8_t brake_state = IDLE_BRAKE;
bool _brake_state = BRAKE_APPLIED;  // brake is applied, coil is not energized.


bool brake_fault(void){
    uint16_t brake_current;
    
    brake_current = API_ReadBrkSense();
    if(_brake_state == BRAKE_RELEASED){
        if(brake_current > 200)
            return BRAKE_OK;  // 
        else
            return BRAKE_FAULT;
    }
    else{  // no current should be applied
        if(brake_current > 200)
            return BRAKE_FAULT; // fault brake is applied, no current should be applied
        else 
            return BRAKE_OK;    // no fault, but this if the brake control wire is 
                                // removed this could indicate a false reading
    }
    
}

void apply_brake(void){
    _set_brake_flag = true;
}
     

void release_brake(void){
    brake_state = IDLE_BRAKE;
    _set_brake_flag = false;
    API_ReleaseBrake();
    _brake_state = BRAKE_RELEASED;
}


///****************************************************************************
//  Function:
//      void brake_task(void)
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
void brake_task(void){
    static MS_TICK brake_task_time_stamp;
    MS_TICK elapsed_time;
    uint8_t motor_pwm;
    static uint8_t fault_bypass = 0;
    
    elapsed_time = TIMER_Since( brake_task_time_stamp );
    if(elapsed_time > 250)
    {
        if(_set_brake_flag){
           switch(brake_state){
               case IDLE_BRAKE:
                   brake_state = STOP_TRACTION_MOTOR;
                   break;
               case STOP_TRACTION_MOTOR:
                   API_CanMotorSetThrottle( INDEX_TRACTION, 0 );
                   brake_state = WAIT_PWM_ZERO;
                   break;
               case WAIT_PWM_ZERO:
                   if(fault_bypass++>4)     // safety micro inhibits the reading of the pwm.  timeout in one second
                       brake_state = APPLY_BRAKE;
                   if( true != API_CanMotorPwm( INDEX_TRACTION, &motor_pwm ) )
                       motor_pwm = 0;
                   if(motor_pwm == 0)
                       brake_state = APPLY_BRAKE;
                   break;
               case BRAKE_DELAY:   // place holder if the brake is applied too quickly
               case APPLY_BRAKE:
                   API_SetBrake();
                   _set_brake_flag = false;
                   brake_state = IDLE_BRAKE;
                   _brake_state = BRAKE_APPLIED;
                   break;
               default:
                   brake_state = IDLE_BRAKE;                 
           }  
        }
        brake_task_time_stamp = TIMER_Now();   
    }  
}
