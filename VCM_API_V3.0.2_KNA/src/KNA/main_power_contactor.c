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
 * Signals actuator over current has been detected
 *
 * The API will call this function when a change is detected in
 * actuator overcurrent.
 *
 * @param[in] bActive true if the overcurrent status has changed
 */



typedef enum CONTACTOR
{ 
    POWER_APPLIED,
    POWER_REMOVED,
} CONTACTOR_STATE;






/**
 * Run the task loop for the test
 */
void TEST_CONTACTOR_Tasks( void )
{
    static MS_TICK contactor_tick = 0;
    static CONTACTOR_STATE actuator_state = POWER_REMOVED;
    

    // How long has it been since we started the cycle?
    MS_TICK elapsed = TIMER_Since( contactor_tick );
    if(elapsed > 5000)
    {
        switch(actuator_state){
            case POWER_APPLIED:
                API_U5_Contactor_Enable_uC(true);
                actuator_state = POWER_REMOVED;
                break;
            case POWER_REMOVED:
                API_U5_Contactor_Enable_uC(false);
                actuator_state = POWER_APPLIED;
                break;
            
            default:
                break;
        }
    contactor_tick = TIMER_Now();
    }
}





