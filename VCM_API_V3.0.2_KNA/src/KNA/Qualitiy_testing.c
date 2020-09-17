
#include "api.h"
#include "../KNA/t_rex_operation.h"
#include "HMI_interface.h"
#include "brake.h"


typedef enum
{
    OPERATOR_OFF_ = 0,
    OPERATOR_STEPS_ON_PRESSES_THROTTLE ,
    OPERATOR_RELEASES_THROTTLE,
    OPERATOR_SELECTS_FORWARD_DIRECTION,
    OPERATOR_PRESSES_TROTTLE_50_PERCENT,
    OPERATOR_RELEASES_THROTTLE_8_SECONDS,
    OPERATOR_PRESSES_THROTTLE_75_PERCENT,
    OPERATOR_STEPS_OFF_MACHINE,
    OPERATOR_STEPS_ON,
    OPERATOR_SELECTS_REVERSE_DIRECTION,
    OPERATOR_PRESSES_THROTTLE_25_PERCENT,
    OPERATOR_SELECTS_FORWARD_DIRECTION_1,
    OPERATOR_SELECTS_TRANSPORT_MODE,
    OPERATOR_SELECTS_NORMAL_MODE,
    OPERATOR_TOOTS_HORN,
    OPERATOR_TURNS_OFF_MACHINE,  
} OperatorSteps_e;



void QC_test_task ( uint8_t * mode, uint32_t * effective_throttle, uint32_t * throttle, bool * fault_condition, API_OpPresence_e * operator ) {
    
   static uint8_t step;
   static MS_TICK _cycleStart = 0;
   MS_TICK elapsed;
   static int8_t task_duration = 10;
   
   
   *fault_condition = false;
   
   
   elapsed = TIMER_Since( _cycleStart );
   if( elapsed > 1000 )
    {
       switch(step){
           
           case OPERATOR_OFF_:
               if(task_duration-- < 0){
                   TRACE( ( "QC - : OPERATOR_STEPS_ON_PRESSES_THROTTLE 5sec\r\n" ) );
                   step = OPERATOR_STEPS_ON_PRESSES_THROTTLE;
                   task_duration = 5;       // operator steps on for 5 seconds
                   *throttle = 4096;        // max throttle
                   *effective_throttle = 1000;  // can never be larger than throttle, invalid test 
                   *operator = OPERATOR_ON;
                   *mode = MODE_DEEP;
               } 
               break;
           case OPERATOR_STEPS_ON_PRESSES_THROTTLE:
               if(task_duration-- < 0){
                   TRACE( ( "QC - : OPERATOR_RELEASES_THROTTLE 1sec \r\n" ) )
                   step = OPERATOR_RELEASES_THROTTLE;
                   task_duration = 1;
                   *throttle = 0;
                   effective_throttle = 0;
                   *operator = OPERATOR_ON;
                   *mode = MODE_DEEP;
               }
               break;
           case OPERATOR_RELEASES_THROTTLE:
               if(task_duration-- < 0){
                   TRACE( ( "QC - : OPERATOR_SELECTS_FORWARD_DIRECTION 1sec \r\n" ) );
                   step = OPERATOR_SELECTS_FORWARD_DIRECTION;
                   set_direction(FORWARD);
                   task_duration = 1;
                   *throttle = 0;
                   *effective_throttle = 0;
                   *operator = OPERATOR_ON;
                   *mode = MODE_DEEP;
               }
               break;
           case OPERATOR_SELECTS_FORWARD_DIRECTION:
               if(task_duration-- < 0){
                   TRACE( ( "QC - : OPERATOR_PRESSES_TROTTLE_50_PERCENT 20sec \r\n" ) );
                   step = OPERATOR_PRESSES_TROTTLE_50_PERCENT;
                   *throttle = 4095;
                   *effective_throttle = 2046;
                   task_duration = 20;  
                   *operator = OPERATOR_ON;
                   *mode = MODE_DEEP;
               }
               break;
           case OPERATOR_PRESSES_TROTTLE_50_PERCENT:
               if(task_duration-- < 0){
                   TRACE( ( "QC - : OPERATOR_RELEASES_THROTTLE_8_SECONDS\r\n" ) );
                   step = OPERATOR_RELEASES_THROTTLE_8_SECONDS;
                   *throttle = 0;
                   *effective_throttle = 0;
                   task_duration = 8;
                   *operator = OPERATOR_ON;
                   *mode = MODE_DEEP;
               }
               break;
           case OPERATOR_RELEASES_THROTTLE_8_SECONDS:
               if(task_duration-- < 0){
                   TRACE( ( "QC - : OPERATOR_PRESSES_THROTTLE_75_PERCENT 20sec\r\n" ) );
                   step = OPERATOR_PRESSES_THROTTLE_75_PERCENT;
                   *throttle = 4095;  // throttle pressed down completely
                   *effective_throttle = 3000;
                   task_duration = 20;
                   *operator = OPERATOR_ON; 
                   *mode = MODE_DEEP;
               }
               break;
           case OPERATOR_PRESSES_THROTTLE_75_PERCENT:
               if(task_duration-- < 0){
                   TRACE( ( "QC - : OPERATOR_STEPS_OFF_MACHINE 20sec\r\n" ) );
                   step = OPERATOR_STEPS_OFF_MACHINE;
                   *throttle = 0;
                   *effective_throttle = 0;
                   task_duration = 20;
                   *operator = OPERATOR_OFF;
                   *mode = MODE_DEEP;
               }
               break;
           case OPERATOR_STEPS_OFF_MACHINE:
               if(task_duration-- < 0){
                   TRACE( ( "QC - : OPERATOR_STEPS_ON 5 sec\r\n" ) );
                   step = OPERATOR_STEPS_ON;
                   *throttle = 0;
                   *effective_throttle = 0;
                   task_duration = 5;
                   *operator = OPERATOR_ON;
                   *mode = MODE_DEEP;
               }
               break;
           case OPERATOR_STEPS_ON:
               if(task_duration-- < 0){
                   TRACE( ( "QC - : OPERATOR_SELECTS_REVERSE_DIRECTION 1sec\r\n" ) );
                   step = OPERATOR_SELECTS_REVERSE_DIRECTION;
                   *throttle = 0;
                   *effective_throttle = 0;
                   task_duration = 1;
                   *operator = OPERATOR_ON;
                   *mode = MODE_DEEP;
               }
               break;
           case OPERATOR_SELECTS_REVERSE_DIRECTION:
               if(task_duration-- < 0){
                   TRACE( ( "QC - : OPERATOR_PRESSES_THROTTLE_25_PERCENT 30sec\r\n" ) );
                   step = OPERATOR_PRESSES_THROTTLE_25_PERCENT;
                   *throttle = 4095;
                   *effective_throttle = 1000;
                   task_duration = 30;
                   set_direction(REVERSE);
                   *operator = OPERATOR_ON;
                   *mode = MODE_DEEP;
               }
               break;
           case OPERATOR_PRESSES_THROTTLE_25_PERCENT:
               if(task_duration-- < 0){
                   TRACE( ( "QC - : OPERATOR_SELECTS_FORWARD_DIRECTION_1 30sec\r\n" ) );
                   step = OPERATOR_SELECTS_FORWARD_DIRECTION_1;
                   *throttle = 1000;
                   *effective_throttle = 1000;
                   task_duration = 30;
                   set_direction(FORWARD);
                   *operator = OPERATOR_ON;
                   *mode = MODE_DEEP;
               }
               break;
           case OPERATOR_SELECTS_FORWARD_DIRECTION_1:
                if(task_duration-- < 0){
                    TRACE( ( "QC - : OPERATOR SELECTS TRANSPORT 20 SECONDS\r\n" ) );
                   step = OPERATOR_SELECTS_TRANSPORT_MODE;
                   *throttle = 4095;
                   *effective_throttle = 4095;
                   task_duration = 20;
                   *operator = OPERATOR_ON; 
                   *mode = MODE_TRANSPORT;
               }
               break;
           case OPERATOR_SELECTS_TRANSPORT_MODE:
               if(task_duration-- < 0){
                    TRACE( ( "QC - : OPERATOR SELECTS NORMAL MODE\r\n" ) );
                   step = OPERATOR_SELECTS_NORMAL_MODE;
                   *throttle = 1000;
                   *effective_throttle = 1000;
                   task_duration = 1;
                   *operator = OPERATOR_ON; 
                   *mode = MODE_DEEP;
               }
               break;
           case OPERATOR_SELECTS_NORMAL_MODE:
               if(task_duration-- < 0){
                    TRACE( ( "QC - : OPERATOR_TOOTS_HORN 1 SECOND \r\n" ) );
                   step = OPERATOR_TOOTS_HORN;
                   set_horn(true);
                   *throttle = 1000;
                   *effective_throttle = 1000;
                   task_duration = 0;
                   *operator = OPERATOR_ON; 
                   *mode = MODE_DEEP;
               }
               break;
            case OPERATOR_TOOTS_HORN:
            if(task_duration-- < 0){
                set_horn(false);
                TRACE( ( "QC - : OPERATOR_TURNS_OFF_MACHINE\r\n" ) );
                step = OPERATOR_TURNS_OFF_MACHINE;
                *throttle = 1000;
                *effective_throttle = 1000;
                task_duration = 1;
                *operator = OPERATOR_ON; 
                *mode = MODE_DEEP;
            }
           break;
           case OPERATOR_TURNS_OFF_MACHINE:
               if(task_duration-- < 0){
                   TRACE( ( "QC - : OPERATOR_OFF_\r\n" ) );
                   step = OPERATOR_OFF_;
                   *throttle = 0;
                   *effective_throttle = 0;
                   task_duration = 5;
                   *operator = OPERATOR_OFF;
                   *mode = MODE_DEEP;
               }
               break;
           default:
               step = OPERATOR_OFF_;
               *throttle = 0;
               *effective_throttle = 0;
               task_duration = 5;
               *operator = OPERATOR_OFF;
               *mode = MODE_DEEP;
       }
       _cycleStart = TIMER_Now();
    } 
}



void maintainance_task(uint8_t direction, uint8_t motor, uint16_t motor_pwm){
    static uint8_t previous_direction = 0;
    static uint8_t previous_motor = 0;
    static uint16_t previous_motor_pwm = 0;
    
    release_brake();
    
    if((direction != previous_direction )||
            (motor != previous_motor )||
            (motor_pwm != previous_motor_pwm)){
        
        previous_direction = direction;
        previous_motor = motor;
        previous_motor_pwm = motor_pwm;
        API_CanMotorSetDirection(motor,direction);
        API_CanMotorSetThrottle( motor, motor_pwm );  
    }
}
