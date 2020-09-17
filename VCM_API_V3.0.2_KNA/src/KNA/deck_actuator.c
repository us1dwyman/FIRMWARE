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
#include "deck_actuator.h"

//------------------------------//----------------------------------------------
// Definitions

//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Local variables

// for keeping track of when we started the test cycle
static MS_TICK _cycleStart;

// to make sure we only report the test result once
//static bool _testComplete = false;

// keep the overcurrent state locally
static bool _overCurrentOccurred = false;

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

#define LIFT_STROKE_TIME              8000

typedef enum deck_status
{ 
    DECK_IDLE,
    DECK_MOVING,
    DECK_WAIT_FOR_STOPPED,
} DK_STATE;


uint8_t _position = DECK_POSITION_IDLE;

// todo need to add this to the lockouts
void APICB_ActuatorOverCurrentOccurred( bool bActive )
{
    if ( true == bActive )
    {
        TRACE( ( "actuator over current active: %d\r\n", bActive ) );
    }
    else
    {
        TRACE( ( "actuator over current inactive: %d\r\n", bActive ) );
    }

    _overCurrentOccurred = bActive;
}


/**
 * Initialize the test runner
 */
void Actuator_Initialize( void )
{
    //API_ConfigureDeckLift( 1, 1, 1, 1, 50, 50 );
    API_ConfigureDeckLift( 100, 100, 100, 100, 90, 90 );
    _cycleStart = TIMER_Now();
}




void set_deck_position(uint8_t position){
    if(position == DECK_POSITION_STOP)
        API_StopDeckLift();
    _position = position;
}


/****************************************************************************
  Function:
      void deck_task(void)

  Description:  This function controls the operation of the deck movement. 
    this function also monitors the motor current and stops the operation if a safety
    related overcurrent event occurs.  There is no feed back to indicate that the 
    actuator has reached the limit of travel. A timer is used to stop energizing 
    the motor at a predetermined time.
  Precondition:
    motor operational parameters should be set up.
  Parameters:
    global variable _position is set/cleared to indicate desired position of the actuator
  Return Values:
   none
  Remarks:
   @todo should call the interlock to indicate an overcurrent event
 ****************************************************************************/
void deck_task(void){
    MS_TICK elapsed;
    MS_TICK elapsed_overCurrent;
    static MS_TICK deck_task_time_stamp;
    static uint8_t current_position = DECK_POSITION_IDLE;
    static uint8_t STATE = DECK_IDLE;
    static MS_TICK overCurrentStart;
    MS_TICK timeSinceOverCurrent;
    static bool overCurrentProcessed = false;
    ApiActuatorState_e overCurrentActuatorState;
    
        // How long has it been since we started the cycle?
//    MS_TICK 
    elapsed_overCurrent = TIMER_Since( _cycleStart );

    if ( _overCurrentOccurred )   
    {
        // if we haven't processed
        if ( overCurrentProcessed == false )
        {    
            overCurrentProcessed = true;
            overCurrentStart = elapsed_overCurrent;
            
            // save what got us here (raising, lowering) - 
            // not necessary - up to user needs
            overCurrentActuatorState = API_getActuatorState();
            
            // test will just stop - up to user
            if ( ( overCurrentActuatorState == API_ACTUATOR_RAISING) ||
                 ( overCurrentActuatorState == API_ACTUATOR_RAISING_DECEL ) )
            {
                API_StopDeckLift();
            }
            else if  ( ( overCurrentActuatorState == API_ACTUATOR_LOWERING) ||
                       ( overCurrentActuatorState == API_ACTUATOR_LOWERING_DECEL ) )
            {
                API_StopDeckLift();
            }
        }
        
        // test waits 1 second then proceeds
        timeSinceOverCurrent = elapsed_overCurrent - overCurrentStart;
        if ( timeSinceOverCurrent > 1000 )
        {
            // reset our local indication that we processed
            overCurrentProcessed = false; 
        }  
    }
    else
    {
        switch(STATE){
            case DECK_IDLE:
                if(current_position != _position){
                    current_position = _position;
                    if(_position == DECK_POSITION_DOWN)
                        API_LowerDeckLift();
                    else if(_position == DECK_POSITION_UP)
                        API_RaiseDeckLift();
                    deck_task_time_stamp = TIMER_Now();
                    STATE = DECK_MOVING;
                }
                break;
            case DECK_MOVING:
                _cycleStart = TIMER_Now();
                elapsed = TIMER_Since( deck_task_time_stamp );
                if( elapsed > LIFT_STROKE_TIME ){
                    API_DecelAndStopDeckLift();
                    STATE =DECK_WAIT_FOR_STOPPED;
                }
                break;
            case DECK_WAIT_FOR_STOPPED:
                if(API_getActuatorState() == API_ACTUATOR_IDLE)
                    STATE = DECK_IDLE;
                break;
            default:
                STATE = DECK_IDLE;
                break;
        }
    }
}



/**
 * Run the task loop for the test
 */
void TEST_Actuator_Tasks( void )
{
//    bool bPassed = false;
    static bool overCurrentProcessed = false;
    ApiActuatorState_e overCurrentActuatorState;
    static MS_TICK overCurrentStart;
    MS_TICK timeSinceOverCurrent;


    // How long has it been since we started the cycle?
    MS_TICK elapsed = TIMER_Since( _cycleStart );

    if ( _overCurrentOccurred )    // todo incorporate this into protection task
    {
        // if we haven't processed
        if ( overCurrentProcessed == false )
        {    
            overCurrentProcessed = true;
            overCurrentStart = elapsed;
            
            // save what got us here (raising, lowering) - 
            // not necessary - up to user needs
            overCurrentActuatorState = API_getActuatorState();
            
            // test will just stop - up to user
            if ( ( overCurrentActuatorState == API_ACTUATOR_RAISING) ||
                 ( overCurrentActuatorState == API_ACTUATOR_RAISING_DECEL ) )
            {
                API_StopDeckLift();
            }
            else if  ( ( overCurrentActuatorState == API_ACTUATOR_LOWERING) ||
                       ( overCurrentActuatorState == API_ACTUATOR_LOWERING_DECEL ) )
            {
                API_StopDeckLift();
            }
        }
        
        // test waits 1 second then proceeds
        timeSinceOverCurrent = elapsed - overCurrentStart;
        if ( timeSinceOverCurrent > 1000 )
        {
            // reset our local indication that we processed
            overCurrentProcessed = false; 
        }
        
    }
//    else
//    {
//        if( elapsed < 10000 )
//        {
//            // simulate pressing the 'up' button
//            API_RaiseDeckLift();
//        }
//        else if ( elapsed < 20000 )
//        {
//            API_DecelAndStopDeckLift();
//        }
//        else if ( elapsed < 30000 )
//        {
//            API_LowerDeckLift();
//        }
//        else if ( elapsed < 40000 )
//        {
//            API_DecelAndStopDeckLift();
//        }
//        else if( elapsed < 50000 )
//        {
//            API_RaiseDeckLift();
//        }
//        else if ( elapsed < 60000 )
//        {
//            API_StopDeckLift();
//        }
//        else if ( elapsed < 70000 )
//        {
//            API_LowerDeckLift();
//        }
//        else if ( elapsed < 80000 )
//        {
//            API_StopDeckLift();
//        }
//        else 
//        {
//            if( false == _testComplete )
//            {
//                bPassed = true;
//                // only report one test result
//                TEST_Complete( "Actuator", bPassed );
//                _testComplete = true;
//            }    
//            // start a new cycle
            _cycleStart = TIMER_Now();
//        }
//    }
}

/**
 * Run the task loop for the actautor state test
 */
void TEST_ActuatorState_Tasks( void )
{
    static ApiActuatorState_e lastState = API_ACTUATOR_IDLE;
    ApiActuatorState_e thisState;

    thisState = API_getActuatorState();

    if ( thisState != lastState )
    {
        lastState = thisState;
        if ( thisState == API_ACTUATOR_IDLE )
        {
            TRACE( ( "Actuator: API_ACTUATOR_IDLE\r\n" ) );
        }
        else if ( thisState == API_ACTUATOR_RAISING )
        {
            TRACE( ( "Actuator: API_ACTUATOR_RAISING\r\n" ) );
        }
        else if ( thisState == API_ACTUATOR_RAISING_DECEL )
        {
            TRACE( ( "Actuator: API_ACTUATOR_RAISING_DECEL\r\n" ) );
        }
        else if ( thisState == API_ACTUATOR_LOWERING )
        {
            TRACE( ( "Actuator: API_ACTUATOR_LOWERING\r\n" ) );
        }
        else if ( thisState == API_ACTUATOR_LOWERING_DECEL )
        {
            TRACE( ( "Actuator: API_ACTUATOR_LOWERING_DECEL\r\n" ) );
        }
        else
        {
            TRACE( ( "Actuator: Unrecognized State\r\n" ) );
        }
    }
}



