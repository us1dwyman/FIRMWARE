/**
 * Application entry point
 *
 * This file intializes the app and runs the main task loop
 *
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "api.h"
#include "app.h"
#include "console.h"
#include "KNA/test.h"
#include "KNA/battery_charger.h"
#include "KNA/t_rex_operation.h"
#include "KNA/Quality_testing.h"
#include "KNA/fault.h"
#include "KNA/lockout.h"
#include "KNA/deck_actuator.h"
#include "KNA/dinputs.h"
#include "KNA/ainputs.h"
#include "KNA/nvram.h"
#include "KNA/outputs.h"
#include "KNA/imu.h"
#include "KNA/battery.h"
#include "KNA/EEPROM.h"
#include "KNA/hour_meters.h"
#include "brake.h"
#include "HMI_interface.h"
#include "micro_actuator.h"


void API_System_Initialize( void );

//------------------------------//----------------------------------------------
// Definitions

//------------------------------//----------------------------------------------
// Global variables
bool _turn_on_contactor = true;
MS_TICK contactor_on_delay_time_stamp;
//------------------------------//----------------------------------------------
// Local variables
//

//------------------------------//----------------------------------------------
// Local functions

//------------------------------//----------------------------------------------
// Public functions




// TODO

#define LOWER_POT_SAFETY_LIMIT 100

uint32_t get_throttle(void){
    int32_t temp = 0;
    uint16_t pot_value;
    pot_value = API_ReadThrottle();
    if(pot_value >LOWER_POT_SAFETY_LIMIT)
        temp = pot_value - LOWER_POT_SAFETY_LIMIT;
    
    return (uint32_t)temp;
    
}





void contactor_task(void){
    MS_TICK elapsed;
   
    if(_turn_on_contactor){
        elapsed = TIMER_Since( contactor_on_delay_time_stamp );
        if( elapsed > 500 ){
            _turn_on_contactor = false;
            API_U5_Contactor_Enable_uC(true);  // turn on contactor
        }
    }  
}




/**
 * Application entry point.  Never returns
 */
int main( void )
{
    uint8_t mode;
    uint32_t throttle = 0;
    uint32_t effective_throttle = 0;
    API_OpPresence_e operator;
    bool machine_lockout = false;
    
    

    API_Initialize();         // Initialize system variables 
    API_System_Initialize();
    Lockout_Initialize();
    API_U5EnableClr();        // start out with the can bus enabled
    contactor_on_delay_time_stamp = TIMER_Now(); 
//	Battery_Initialize();       
//    Hour_Meters_Initialize();   
//    EEPROM_Initialize(); 							
    while( 1 )
    {
        API_Tasks();                                 // Maintenance of the API
        contactor_task();
//        CONSOLE_Tasks();                             // respond to messages 
#ifdef RUN_TESTS
        TEST_Tasks();                                // code snippets to test hardware and code
#else
//        test_lockout_code();                         // print statements to indicate lockouts 
        

        
#ifdef SIMULATION
        QC_test_task(&mode,&effective_throttle,&throttle,&machine_lockout,&operator);  // this will overide any values from the HMI
#else
        mode = get_operating_mode();                   // HMI sends mode, parse can message                 
        effective_throttle = get_effective_throttle(); // returns lower of limit or the throttle
        throttle = get_throttle();//API_ReadThrottle();                 // used for safety when the operator first gets on the machine
        operator = get_operator_status();              // is operator on the platform
        
#endif
        
        machine_lockout = Lockout_Get_Status();
        machine_task(mode,effective_throttle,throttle,machine_lockout,operator);
        vacuum_task();       // delays the turn off of the vacuum
        deck_task();         // controls the up/down with current overload protection
        horn_task();         // drives horn according to can message from HMI
        brake_task();        // ensures motor voltage is off before applying the parking brake
        lock_out_task();     // check lock out every 250 ms
        charger_detection_task();  // handle the case where the charger stops 
                                   // sending messages when unplugged from AC
        micro_Actuator_Tasks();  // move actuator to position and apply brake
        update_system_messages_task(); // status updates over the can bus every 250ms
//        Battery_Tasks();        // Maintain battery tasks  
//        lithium_remap_TPDO2();  // Remap TPDO2  This  needs to happen at start up 6 times 
//        Hour_Meters_Task();     // Maintain hour meter tasks  
//        EEPROM_Task();          // Maintain EEPROM tasks													  
#endif
        KICK_WATCHDOG();     // Don't let watchdog timer reset us
    }
}


void API_System_Initialize( void )
{
    Actuator_Initialize();
    dinputs_Initialize();
    ainputs_Initialize();
    T_rex_Can1_Initialize();
//    Can2_Initialize();
//    TEST_SPI_Initialize();
    NVRAM_Initialize();
    Outputs_Initialize();
    IMU_Initialize();
}
