/*
 * File:   lockout.c
 * Author: us1jbernath
 *
 * Created on April 13, 2020, 8:20 AM
 * 
 * This code is used to control the word used to lock the machine out from clean 
 * or transporting.  Each bit in the word is a different reason to lock the machine
 * out.  This code set, reset and get each bit bit and can used to check if the 
 * word is true or false.  If the work is true the machine cannot operate.
 * 
 * This code does not check any of reasons for locking the machine out.  This 
 * will need to be done in different sections of the code.
 */


#include "api.h"
#include "lockout.h"
#include "battery_charger.h"


/*This word will contain the lockout bits for each reason to lockout the machine.  
 * This includes the estop, key switch, oppresents, bag loop interlocks, this can 
 * also include warnings and errors*/
static uint16_t lockout_status;

/************************************************************************
 * FUNCTION: Lockout_Initialize();
 *
 * DESCRIPTION: checks all of the lockout conditions and sets them.
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/

void Lockout_Initialize(void)
{
    /*Set the lockouts.  These will need to be reset before the machine can 
     * run.*/
    lockout_status = 0;//LOCKOUT_DEFAULT;
}

/************************************************************************
 * FUNCTION: Lockout_Get_Status();
 *
 * DESCRIPTION: Used to check if there is any lockout condition present.  
 * This does not check the physical switches.
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/

bool Lockout_Get_Status(void)
{
    return lockout_status;
}

/************************************************************************
 * FUNCTION: uint16_t Lockout_Get_Flags(void)();
 *
 * DESCRIPTION: Returns the value of the lockout word.  This will need to be 
 * decoded to determine each active lockout condition. 
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/

uint16_t Lockout_Get_Flags(void)
{
    return lockout_status;
}

/************************************************************************
 * FUNCTION: Lockout_Get_Status_Transport();
 *
 * DESCRIPTION: Used to check if there is any lockout the transport mode
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/

bool Lockout_Get_Status_Transport(void)
{
    return lockout_status & LOCKOUT_TRANSPORT;
}

/************************************************************************
 * FUNCTION: Lockout_Get_Status_Fault();
 *
 * DESCRIPTION: Used to check if there is fault locking the machine out
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/

bool Lockout_Get_Status_Fault(void)
{
    return lockout_status & LOCKOUT_FAULTS;
}

/************************************************************************
 * FUNCTION: Lockout_Set_Condition();
 *
 * DESCRIPTION: Used to set the each lockout condition independently bit.  
 * This needs to be called when a condition tripped.
 *
 * INPUT:  condition - this is the bit that will be set.  #defines for the 
 * conditions are in lockuout.h.  ie. LOCKOUT_ESTOP_BIT is for the Estop.
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/

void Lockout_Set_Condition(uint16_t lockout_condition)
{
    lockout_status |= lockout_condition;
    return;
}

/************************************************************************
 * FUNCTION: Lockout_Reset_Condition();
 *
 * DESCRIPTION: Used to reset the each lockout condition independently bit.  
 * This needs to be called when a condition reset.
 *
 * INPUT:  condition - this is the bit that will be set.  #defines for the 
 * conditions are in lockuout.h.  ie. LOCKOUT_ESTOP_BIT is for the Estop.
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/

void Lockout_Reset_Condition(uint16_t lockout_condition)
{
    lockout_status &= ~lockout_condition; //The ~ is used to flip the bit
    return;
}

/************************************************************************
 * FUNCTION: Lockout_Get_Condition();
 *
 * DESCRIPTION: Used to check the status of the lockout condition bit.  
 * This does not check the physical switch.
 *
 * INPUT:  condition - this is the bit that will be set.  #defines for the 
 * conditions are in lockuout.h.  ie. LOCKOUT_ESTOP_BIT is for the Estop.
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/

bool Lockout_Get_Condition(uint16_t lockout_condition)
{
    return lockout_status & lockout_condition;
}



void test_lockout_code(void){
    static bool status = false;
    bool current_status;
    
    current_status = Lockout_Get_Status();
    if(current_status != status){
        status = current_status;
        if(status){
            TRACE( ( "Machine locked out\r\n" ) );
        }       
        else{
            TRACE( ( "Machine ready\r\n" ) );
        }    
    }
}


void lock_out_task(void){
    ApiInputs_t digital_inputs;
    static MS_TICK lock_out_task_timer = 0;
    MS_TICK Elapsed; 

    Elapsed = TIMER_Since( lock_out_task_timer );

    if( Elapsed >= 250 ){
        digital_inputs.word = API_GetDinputWord();
        
       
        if(digital_inputs.bits.estop)
            Lockout_Set_Condition(LOCKOUT_ESTOP_BIT);
        else
            Lockout_Reset_Condition(LOCKOUT_ESTOP_BIT);
        
        if(digital_inputs.bits.interlock)
            Lockout_Set_Condition(LOCKOUT_LOOP_INTERLOCK_BIT);
        else
            Lockout_Reset_Condition(LOCKOUT_LOOP_INTERLOCK_BIT);
       
//        if(digital_inputs.bits.keysw)
//            Lockout_Set_Condition(LOCKOUT_KEY_SWITCH_BIT);
//        else
//            Lockout_Reset_Condition(LOCKOUT_KEY_SWITCH_BIT);
        // signal pulled high, active low signal to create a fault
        if(!digital_inputs.bits.dmodfault)
            Lockout_Set_Condition(LOCKOUT_FAULT_BIT);
        else
            Lockout_Reset_Condition(LOCKOUT_FAULT_BIT);
        
        
       
        
        // true charger is disconnected, false otherwise
       if(get_charger_status() == CHARGER_DISCONNECTED)
        Lockout_Reset_Condition(LOCKOUT_AC_POWER);
       else{   // recieving messages
           if(get_ac_power_status())   // for chargers that continue to send messages when unplugged from the AC
              Lockout_Set_Condition(LOCKOUT_AC_POWER);
        else
            Lockout_Reset_Condition(LOCKOUT_AC_POWER);
       }
        
        
        
        lock_out_task_timer = TIMER_Now(); 
    }
}

