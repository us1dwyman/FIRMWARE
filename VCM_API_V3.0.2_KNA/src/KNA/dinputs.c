/*
 * File:   dinputs.c
 * Author: US1JBERNATH
 *
 * Created on May 18, 2020, 1:48 PM
 */

/**
 * Here we watch to see if any of the discrete inputs change, and if so
 * we print out a message.  In order to 'pass' we need to see at least one
 * input change state.
 *
 *
 */


#include "api.h"
#include "test.h"
#include "console.h"
#include "lockout.h"
//------------------------------//----------------------------------------------
// Definitions
#define BAG_FLOW_TIMER_MS 10000
//------------------------------//----------------------------------------------
// Global variables

bool TEST_DINPUTS_bagFlowOutputEnabled = false;

//------------------------------//----------------------------------------------
// Local variables
static bool _testComplete = false;
static uint32_t _bagFlowTransitions = 0;
static MS_TICK _cycleStart;

// keep our own copy of the last known state
static ApiInputs_t _lastState;

//------------------------------//----------------------------------------------
// Local functions

//------------------------------//----------------------------------------------
// Public functions

/**
 * Signals a change on the interlock input
 *
 * The API will call this function when change is detected on the
 * interlock line.
 * @param[in] bActive true if the input is in its active state
 */
void APICB_InterlockChanged( bool bActive )
{
    if ( true == bActive )
    {
        TRACE( ( "interlock active: %d\r\n", bActive ) );
        /*reset the lockout for the loop interlock.*/ 
        Lockout_Reset_Condition(LOCKOUT_LOOP_INTERLOCK_BIT);
        /*Add motor shutdown here*/ 
    }
    else
    {
        TRACE( ( "interlock inactive: %d\r\n", bActive ) );
        /*lock the machine out due to an ope interlock*/ 
        Lockout_Set_Condition(LOCKOUT_LOOP_INTERLOCK_BIT);
    }
    TRACE( ( "Lockout: %d\r\n", Lockout_Get_Status()) );
}

/**
 * Handles an external interrupt 2 event.
 *
 * Overrides the (weak) version in ext_int.c.
 * Up to the user to implement functionality.
 * This is plumbed to the Bag/Flow signal on processor RC9.
 *
 * For now we just bump a local variable.
 */
void EX_INT2_CallBack( void )
{
    // Add your custom callback code here
    _bagFlowTransitions++;
}

/**
 * Signals a change on the Brk Rel input
 *
 * The API will call this function when change is detected on the
 * Brk Rel line.
 * @param[in] bActive true if the input is in its active state
 */
void APICB_BrkRelChanged( bool bActive )
{
    if ( true == bActive )
    {
        TRACE( ( "brk rel active: %d\r\n", bActive ) );
        Lockout_Reset_Condition(LOCKOUT_BRAKE_RELEASE_BIT);
        /*Add motor shutdown here*/ 
    }
    else
    {
        TRACE( ( "brk rel inactive: %d\r\n", bActive ) );
        Lockout_Set_Condition(LOCKOUT_BRAKE_RELEASE_BIT);
    }
    TRACE( ( "Lockout: %d\r\n", Lockout_Get_Status()) );
}

/**
 * Signals a change on the GPIN01 input
 *
 * The API will call this function when change is detected on the
 * GPIN01 line.
 * @param[in] bActive true if the input is in its active state
 */
void APICB_GpIn01Changed( bool bActive )
{
    if ( true == bActive )
    {
        TRACE( ( "gpin01 active: %d\r\n", bActive ) );
    }
    else
    {
        TRACE( ( "gpin01 inactive: %d\r\n", bActive ) );
    }
    TRACE( ( "Lockout: %d\r\n", Lockout_Get_Status()) );
}

/**
 * Signals a change on the Dmod Fault input
 *
 * The API will call this function when change is detected on the
 * Dmod Fault line.
 * @param[in] bActive true if the input is in its active state
 */
void APICB_DmodFaultChanged( bool bActive )
{
    if ( true == bActive )
    {
        TRACE( ( "dmod fault active: %d\r\n", bActive ) );
        Lockout_Reset_Condition(LOCKOUT_FAULT_BIT);
        /*Add motor shutdown here*/
    }
    else
    {
        TRACE( ( "dmod fault inactive: %d\r\n", bActive ) );
        Lockout_Set_Condition(LOCKOUT_FAULT_BIT);
    }
    TRACE( ( "Lockout: %d\r\n", Lockout_Get_Status()) );
}

/**
 * Signals a change on the e-stop input
 *
 * The API will call this function when change is detected on the
 * e-stop line.
 * @param[in] bActive true if the input is in its active state
 */
void APICB_EstopChanged( bool bActive )
{
    if ( true == bActive )
    {
       // TRACE( ( "Estop active: %d\r\n", bActive ) );
       //Lockout_Set_Condition(LOCKOUT_ESTOP_BIT);
    }
    else
    {
        TRACE( ( "Estop inactive: %d\r\n", bActive ) );
        Lockout_Reset_Condition(LOCKOUT_ESTOP_BIT);
    }
    Lockout_Reset_Condition(LOCKOUT_FAULT_BIT);
    TRACE( ( "Lockout bool: %d\r\n", Lockout_Get_Status()) );
    TRACE( ( "Lockout word: %X\r\n", Lockout_Get_Flags()) );
    TRACE( ( "Lockout transport: %d\r\n", Lockout_Get_Status_Transport()) );
    TRACE( ( "Lockout fault: %d\r\n", Lockout_Get_Status_Fault()) );
    TRACE( ( "Estop Lockout condition: %d\r\n", Lockout_Get_Condition(LOCKOUT_ESTOP_BIT)) );
}

/**
 * Signals a change on the keyswitch input
 *
 * The API will call this function when change is detected on the
 * keyswitch line.
 * @param[in] bActive true if the input is in its active state
 */
void APICB_KeySwChanged( bool bActive )
{
    if ( true == bActive )
    {
        TRACE( ( "keysw active: %d\r\n", bActive ) );
        Lockout_Reset_Condition(LOCKOUT_KEY_SWITCH_BIT);//JMB 4/15/20
        /*Add motor shutdown here*/ //JMB 4/15/20
    }
    else
    {
        TRACE( ( "keysw inactive: %d\r\n", bActive ) );
        Lockout_Set_Condition(LOCKOUT_KEY_SWITCH_BIT);//JMB 4/15/20
        
        //CONSOLE_WaitForTxIdle();
        
        // reset for now, so we can get to the bootloader.  this logic
        // might change in the future
        //RESET();
    }
    TRACE( ( "Lockout: %d\r\n", Lockout_Get_Status()) );
}

/** \ingroup callback
 * Signals a change on the Spare In input
 *
 * The API will call this function when change is detected on the
 * Spare In line.
 * @param[in] bActive true if the input is in its active state
 */
void APICB_SpareInChanged( bool bActive )
{
    if ( true == bActive )
    {
        TRACE( ( "sparein active: %d\r\n", bActive ) );
    }
    else
    {
        TRACE( ( "sparein inactive: %d\r\n", bActive ) );
    }
}

/**
 * Initialize the test runner
 */
void dinputs_Initialize( void )
{
    // Initialize our copy of the state
    _lastState.word = API_GetDinputWord();
    
    // start a bag/flow time interval
    _cycleStart = TIMER_Now();
}

/**
 * Run the task loop for the test
 */
void TEST_dinputs_Tasks( void )
{
    ApiInputs_t thisState;
    
    static uint32_t prevBagFlowCount = 0;

    MS_TICK elapsed = TIMER_Since( _cycleStart );
    
    if ( elapsed >= BAG_FLOW_TIMER_MS )
    {
        if ( true == TEST_DINPUTS_bagFlowOutputEnabled )
        {
            TRACE( ( "bag/flow rising edge int count in the last %d mS: %lu\r\n", BAG_FLOW_TIMER_MS, _bagFlowTransitions ) );
        }
        _bagFlowTransitions = 0;
        _cycleStart = TIMER_Now();
    }    
    if ( prevBagFlowCount != _bagFlowTransitions )
    {
        prevBagFlowCount = _bagFlowTransitions;
    }
    
    thisState.word = API_GetDinputWord();
    
    // no test complete until a change in state occurs
    if( ( _lastState.word ^ thisState.word ) > 0 )
    {

        // update our copy of the most recent state
        _lastState.word = thisState.word;

        if( !_testComplete )
        {
            _testComplete = true;
            TEST_Complete( "dinputs", true );
        }
    }
}

/**
 * Signals a change on the oppres inputs
 *
 * The API will call this function when change is detected on the
 * oppres lines.
 * @param[in] opPres reflects state of oppres inputs
 */
void APICB_OpPresChanged( API_OpPresence_e opPres )
{
    
    set_operator_status(opPres);
    if( OPERATOR_INVALID1 == opPres )
    {
        
        TRACE( ( "OpPresence: OPERATOR_INVALID1\r\n" ) );
        Lockout_Set_Condition(LOCKOUT_OPERATOR_PRESENCE_INVALID_BIT);
    }
    if( OPERATOR_ON == opPres )
    {
        TRACE( ( "OpPresence: OPERATOR_ON\r\n" ) );
        Lockout_Reset_Condition(LOCKOUT_OPERATOR_PRESENCE_BIT);
        Lockout_Reset_Condition(LOCKOUT_OPERATOR_PRESENCE_INVALID_BIT);
    }
    if( OPERATOR_OFF == opPres )
    {
        TRACE( ( "OpPresence: OPERATOR_OFF\r\n" ) );
        Lockout_Set_Condition(LOCKOUT_OPERATOR_PRESENCE_BIT);
        Lockout_Reset_Condition(LOCKOUT_OPERATOR_PRESENCE_INVALID_BIT);
    }
    if( OPERATOR_INVALID2 == opPres )
    {
        TRACE( ( "OpPresence: OPERATOR_INVALID2\r\n" ) );
        Lockout_Set_Condition(LOCKOUT_OPERATOR_PRESENCE_INVALID_BIT);
    }
}
