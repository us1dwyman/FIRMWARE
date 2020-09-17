/**
 * Debounces digital inputs
 *
 *
 * Copyright (c) 2018 Control Solutions LLC.  All Rights Reserved.
 */

#include "api.h"
#include "timer.h"

//------------------------------//----------------------------------------------
// Definitions

// The bits we look at for debouncing samples.
// a mask of 0x0F looks at the 4 LSBs
// a Mask of 0x07 would look at the three LSBs
#define SWITCH_SAMPLE_MASK 0x000F

// A convenience macro that checks the sampled bits and determines
// the input's debounced state.  It's a macro since since similar code
// is repeated multiple times.
#define EVALUATE( pin, bit )                                                                                           \
    s = _sample##pin & SWITCH_SAMPLE_MASK;                                                                             \
    if( SWITCH_SAMPLE_MASK == s )                                                                                      \
        bit = 1;                                                                                                       \
    else if( 0 == s )                                                                                                  \
        bit = 0;

//------------------------------//----------------------------------------------
// Global variables


//------------------------------//----------------------------------------------
// Local variables
static ApiInputs_t API_dinputs;
volatile bool _newSwitchSample = false;

// Storage to keep track of sampled hardware inputs

volatile unsigned int _sampleINTERLOCK = 0;
volatile unsigned int _sampleOPPRES1 = 0;
volatile unsigned int _sampleOPPRES2 = 0;
volatile unsigned int _sampleBRKREL = 0;
volatile unsigned int _sampleGPIN01 = 0;
volatile unsigned int _sampleDMODFAULT = 0;
volatile unsigned int _sampleESTOP = 0;
volatile unsigned int _sampleKEYSW = 0;
volatile unsigned int _sampleCSLED = 0;


//------------------------------//----------------------------------------------
// Local functions

/**
 * Called by the systick interrupt
 *
 * This is called on an ISR context
 */
static void _Sample_ISR( void )
{
    _sampleINTERLOCK = ( _sampleINTERLOCK << 1 ) | Interlock_IO_RF1_GetValue();
    _sampleOPPRES1 = ( _sampleOPPRES1 << 1 ) | OpPres1_GetValue();
    _sampleOPPRES2 = ( _sampleOPPRES2 << 1 ) | OpPres2_GetValue();
    _sampleBRKREL = ( _sampleBRKREL << 1 ) | Brk_Rel_IO_RG8_GetValue();
    _sampleGPIN01 = ( _sampleGPIN01 << 1 ) | GPIN01_IO_RC10_GetValue();
    _sampleDMODFAULT = ( _sampleDMODFAULT << 1 ) | IO_RA7_GetValue();
    _sampleESTOP = ( _sampleESTOP << 1 ) | ESTOP_uC_IO_RD6_GetValue();
    _sampleKEYSW = ( _sampleKEYSW << 1 ) | IO_RA10_GetValue();
    _sampleCSLED = ( _sampleCSLED << 1 ) | CS_LED_IO_RD5_GetValue();

    _newSwitchSample = true;
}

//------------------------------//----------------------------------------------
// Public functions

/**
 * Powerup Initialization
 */
void API_Dinput_Initialize( void )
{
    // Request a periodic interrupt for sampling
    TIMER_Request( &_Sample_ISR, 10000 / TIMER_TICK_PERIOD );  // 10ms

    // Powerup state of the debounced inputs
    // the powerup state should be aligned with the hardware pullups
    // to avoid detecting transitions on powerup (if we initialize to zero
    // but the line is pulled up in hardware, the bit in API_dinputs will transition to 0-1 shortly after startup)
    API_dinputs.word = 0xFFFF;
}

/**
 * Module task loop
 */
void API_Dinput_Tasks( void )
{
    // If we sampled the inputs, evaluate them
    if( _newSwitchSample )
    {
        ApiInputs_t previous_dinputs;
        int s;

        _newSwitchSample = false;
        
        previous_dinputs.word = API_dinputs.word;

        EVALUATE( INTERLOCK, API_dinputs.bits.interlock );
        EVALUATE( OPPRES1, API_dinputs.bits.oppres1 );
        EVALUATE( OPPRES2, API_dinputs.bits.oppres2 );
        EVALUATE( BRKREL, API_dinputs.bits.brkrel );
        EVALUATE( GPIN01, API_dinputs.bits.gpin01 );
        EVALUATE( DMODFAULT, API_dinputs.bits.dmodfault );
        EVALUATE( ESTOP, API_dinputs.bits.estop );
        EVALUATE( KEYSW, API_dinputs.bits.keysw );
        EVALUATE( CSLED, API_dinputs.bits.csled );
        
        if ( previous_dinputs.word != API_dinputs.word )
        {
            if ( previous_dinputs.bits.interlock != API_dinputs.bits.interlock )
            {
                APICB_InterlockChanged( !( (bool) API_dinputs.bits.interlock ) );
            }
            if ( ( previous_dinputs.bits.oppres1 != API_dinputs.bits.oppres1 ) ||
                 ( previous_dinputs.bits.oppres2 != API_dinputs.bits.oppres2 ) )
            {
                if ( API_dinputs.bits.oppres1 )
                {
                    if ( API_dinputs.bits.oppres2 )
                    {
                        APICB_OpPresChanged( OPERATOR_INVALID2 );
                    }
                    else
                    {
                        //APICB_OpPresChanged( OPERATOR_ON );
                        APICB_OpPresChanged( OPERATOR_INVALID1 );
                    }
                }
                else
                {
                    if ( API_dinputs.bits.oppres2 )
                    {
                        //APICB_OpPresChanged( OPERATOR_INVALID1 );
                        APICB_OpPresChanged( OPERATOR_ON );
                    }
                    else
                    {
                        APICB_OpPresChanged( OPERATOR_OFF );
                    }
                }       
            }
            if ( previous_dinputs.bits.brkrel != API_dinputs.bits.brkrel )
            {
                APICB_BrkRelChanged( !( (bool) API_dinputs.bits.brkrel ) );
            }
            if ( previous_dinputs.bits.gpin01 != API_dinputs.bits.gpin01 )
            {
                APICB_GpIn01Changed( !( (bool) API_dinputs.bits.gpin01 ) );
            }
            if ( previous_dinputs.bits.dmodfault != API_dinputs.bits.dmodfault )
            {
                APICB_DmodFaultChanged( !( (bool) API_dinputs.bits.dmodfault ) );
            }
            if ( previous_dinputs.bits.estop != API_dinputs.bits.estop )
            {
                APICB_EstopChanged( !( (bool) API_dinputs.bits.estop ) );
            }
            if ( previous_dinputs.bits.keysw != API_dinputs.bits.keysw )
            {
                APICB_KeySwChanged( !( (bool) API_dinputs.bits.keysw ) );
            }
//            if ( previous_dinputs.bits.csled != API_dinputs.bits.csled )
//            {
//                //( !( (bool) API_dinputs.bits.csled ) );                           // DRW NEED TO LOOK INTO THIS
//            }
        }
    }
}

/**
 * Return the Current state of all digital inputs
 *
 * @return     value of the dinput word
 */
uint16_t API_GetDinputWord( void )
{
    return ( API_dinputs.word );
}
