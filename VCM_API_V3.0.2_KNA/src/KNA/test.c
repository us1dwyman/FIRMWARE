/**
 * Test Runner
 *
 * This module runs tests on the API.  These tests may be useful as example
 * code showing how to use the API
 *
 */

#include "api.h"
#include "test.h"
#include "KNA/deck_actuator.h"
#include "KNA/dinputs.h"
#include "KNA/ainputs.h"
#include "KNA/can2.h"
#include "KNA/HMI_interface.h"
//#include "KNA/test_spi.h"
#include "KNA/imu.h"
#include "KNA/t_rex_operation.h"
#include "battery_charger.h"
#include "micro_actuator.h"
#include "main_power_contactor.h"

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






/**
 * Run the task loops for the tests
 *
 */
void TEST_Tasks( void )
{
    // actuator test intrudes on actual operation.
//     TEST_Actuator_Tasks();

 //   TEST_ActuatorState_Tasks();
//
//    TEST_dinputs_Tasks();
//
//    TEST_ainputs_Tasks();
//
 //   TEST_Can2_Tasks();
//
 //   TEST_Can1HmiLed_Tasks();
//
 //   TEST_SPI_Tasks();

//
//    // output tests would intrude on operation
//
    
  
//    TEST_IMU_Tasks();
//     
        TEST_sidebroom_Tasks();
        TEST_Brush_motor_Tasks();
//        TEST_Vacuum_Tasks();
//        TEST_traction_change_direction_Tasks();
//        TEST_traction_Tasks();
//        TEST_throttle_Tasks(); 
//        TEST_deck_movement_task();
//     AC_power_detection_test();
//    TEST_micro_Actuator_Tasks();
//    test_micro_actuator();
//    TEST_CONTACTOR_Tasks();
}


void TEST_deck_movement_task(void){
    
    static MS_TICK _cycleStart = 0;
    
    // How long has it been since we started the cycle?
    MS_TICK elapsed = TIMER_Since( _cycleStart );
    static uint8_t count = 0;

   if( elapsed > 1000 )
    {
       _cycleStart = TIMER_Now();
       if(count == 0){
           API_RaiseDeckLift();
       }
       if(count == 1){
           API_DecelAndStopDeckLift();
       }
       if(count == 2){
           API_LowerDeckLift();
       }
       if(count == 3){
           API_DecelAndStopDeckLift();
       }
       count++;
       if(count == 4){
           count = 0;
       }
       _cycleStart = TIMER_Now();
       
    }
}

void TEST_throttle_Tasks( void )
{
    static uint16_t throttle = 0;
    static MS_TICK _cycleStart;
    // How long has it been since we started the cycle?
    MS_TICK elapsed = TIMER_Since( _cycleStart );

   if( elapsed > 1000 )
    {
//       throttle = get_effective_throttle();
       TRACE( ( "Throttle: %d      \r\n", throttle) );
       _cycleStart = TIMER_Now();
    }
}

void TEST_traction_Tasks( void )
{
    static uint16_t throttle = 0;
    static MS_TICK _cycleStart;
    uint8_t pwm;
    uint16_t i;
    static bool rotation;
    uint16_t brake_current;

   MS_TICK elapsed = TIMER_Since( _cycleStart );

   if( elapsed > 1000 )
    {
       brake_current = API_ReadBrkSense();
       TRACE( ( "Brake Current: %d  \r\n", brake_current) );
       //throttle = get_throttle();
       throttle += 200;
       if(throttle  > 4000){
           throttle = 0;
           if(rotation){
               TRACE( ( "REVERSE Rotation \r\n") );
               rotation = false;
               API_CanMotorSetDirection(INDEX_TRACTION,API_REVERSE);          
           } else {
               TRACE( ( "FORWARD Rotation \r\n") );
               rotation = true;
               API_CanMotorSetDirection(INDEX_TRACTION,API_FORWARD); 
           }
       }
       
       API_CanMotorSetThrottle( INDEX_TRACTION, throttle );
       if( true != API_CanMotorPwm( INDEX_TRACTION, &pwm ) )
        {
            TRACE( ( "TRACTION - False return from API_CanMotorPwm\r\n" ) );
            pwm = 0;
        }
       i = pwm;
       TRACE( ( "Throttle: %d     PWM: %d \r\n", throttle,i) );
       _cycleStart = TIMER_Now();
    }
}

void TEST_traction_change_direction_Tasks( void )
{
    static uint16_t throttle = 0;
    static MS_TICK _cycleStart;
    static bool rotation;
    uint16_t brake_current;

   MS_TICK elapsed = TIMER_Since( _cycleStart );

   if( elapsed > 5000 )
    {
       brake_current = API_ReadBrkSense();
       TRACE( ( "Brake Current: %d  \r\n", brake_current) );
       throttle = 0;
       
         
       if(rotation){
           TRACE( ( "REVERSE Rotation \r\n") );
           rotation = false;
           API_CanMotorSetDirection(INDEX_TRACTION,API_REVERSE);          
       } else {
           TRACE( ( "FORWARD Rotation \r\n") );
           rotation = true;
           API_CanMotorSetDirection(INDEX_TRACTION,API_FORWARD); 
       }
       API_CanMotorSetThrottle( INDEX_TRACTION, throttle );
       _cycleStart = TIMER_Now();
    }
}


void TEST_Vacuum_Tasks( void )
{
   static uint16_t throttle = 0;
   static MS_TICK _cycleStart;
   MS_TICK elapsed = TIMER_Since( _cycleStart );

   if( elapsed > 5000 )
    {
       throttle += 500;
       if(throttle  > 4000){
           throttle = 0; 
       }
       API_CanMotorSetThrottle( INDEX_VACUUM, throttle );
       _cycleStart = TIMER_Now();
    }
}


void TEST_Brush_motor_Tasks( void )
{
   static uint16_t throttle = 0;
   static MS_TICK _cycleStart;
   MS_TICK elapsed = TIMER_Since( _cycleStart );

   if( elapsed > 5000 )
    {
       throttle += 200;
       if(throttle  > 4000){
           throttle = 0; 
       }
       API_CanMotorSetThrottle( INDEX_SIDEBROOM, throttle );
       _cycleStart = TIMER_Now();
    }
}

void TEST_sidebroom_Tasks( void )
{
    static uint16_t throttle = 0;
    static MS_TICK _cycleStart;

   MS_TICK elapsed = TIMER_Since( _cycleStart );

   if( elapsed > 1000 )
    {
      throttle += 100;
      API_CanMotorSetThrottle( INDEX_BRUSH, throttle );
       TRACE( ( "Throttle: %d   \r\n", throttle) );
       _cycleStart = TIMER_Now();
    }
}


/**
 * Output the result of a test
 *
 */
void TEST_Complete( const char *pName, bool bPass )
{
    TRACE( ( "Complete: %s: %s\r\n", pName, ( bPass ) ? "PASSED" : "FAILED" ) );
}



