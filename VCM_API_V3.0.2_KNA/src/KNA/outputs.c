/**
 * Tests the Outputs
 *
 * Initially this will contain some test initialization code to 
 * explicitly set the state of the outputs. 
 * Tests will be unimplemented as they would interfere with 
 * opeation of the unit.
 * All outputs are supported by console command in the case that 
 * control is desired in a test environment.
 *
 */

#include "api.h"
#include "test.h"


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
 * Initialize the test runner
 */
void Outputs_Initialize( void )
{
 //   API_AccOut1Enable( false );
    API_AccOut2Enable( false );
    API_AccOut3Enable( false );
 //   API_AccOut1Enable( false );
    API_AccEnOutEnable( false );
    
    API_HornEnable( false, (uint8_t) 100 );
    
    API_ConfigureBrake( (uint8_t) 100 );
    API_SetBrake();
    
//    API_DmodEnClr();
//    API_CsLedClr();
}

