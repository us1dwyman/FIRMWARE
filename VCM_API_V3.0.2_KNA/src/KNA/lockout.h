/*
 * File:   lockout.h
 * Author: us1jbernath
 *
 * Created on April 13, 2020, 8:21 AM
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

#ifndef _LOCKOUT_H_
#define _LOCKOUT_H_


/*These are a list of Lockouts for the machine. The defined values are the
 *masks for the lockouts bit. */ 
#define LOCKOUT_ESTOP_BIT                           0x0001
#define LOCKOUT_KEY_SWITCH_BIT                      0x0002
#define LOCKOUT_LOOP_INTERLOCK_BIT                  0x0004
#define LOCKOUT_BAG_FLOW_BIT                        0x0008
#define LOCKOUT_BRAKE_RELEASE_BIT                   0x0010
#define LOCKOUT_OPERATOR_PRESENCE_BIT               0x0020
#define LOCKOUT_OPERATOR_PRESENCE_INVALID_BIT       0x0040
#define LOCKOUT_FAULT_BIT                           0x0080
#define LOCKOUT_AC_POWER                            0x0100
#define LOCKOUT_BATTERY_0_SOC                       0x0200  //JMB 7/30/2020
#define LOCKOUT_BATTERY_DEAD                        0x0400  //JMB 7/30/2020

/*The LOCKOUT_DEFAULT is used to set the starting default state 
 *of the lockout word. Some lockouts will need to be reset 
 *before operating the machine.  */
#define LOCKOUT_DEFAULT                         0x0076 // All lockouts except LOCKOUT_FAULT_BIT are set

/*The LOCKOUT_TRANSPORT is used to set which bits are able to lock out the 
 * transport of the machine.  */
#define LOCKOUT_TRANSPORT                       0x02FF// All will lock out the transport //JMB 7/30/2020

/*The LOCKOUT_FAULTS is used to set which bits are faults conditions.  */
#define LOCKOUT_FAULTS                          0x0080// LOCKOUT_FAULT_BIT is currently the only fault

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

void Lockout_Initialize(void);

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

bool Lockout_Get_Status(void);

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

uint16_t Lockout_Get_Flags(void);

/************************************************************************
 * FUNCTION: Lockout_Get_Status_Transport();
 *
 * DESCRIPTION: Used to check if there is any lockout the transport mode
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/

bool Lockout_Get_Status_Transport(void);

/************************************************************************
 * FUNCTION: Lockout_Get_Status_Fault();
 *
 * DESCRIPTION: Used to check if there is fault locking the machine out
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/

bool Lockout_Get_Status_Fault(void);

/************************************************************************
 * FUNCTION: Lockout_Set_Condition();
 *
 * DESCRIPTION: Used to set the each lockout condition independently bit.  
 * This needs to be called when a condition tripped.
 *
 * INPUT:  condition - this is the bit that will be set
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 * Example: Lockout_Set_Condition(LOCKOUT_ESTOP_BIT); This will set the lockout 
 * bit for the estop, and must be called when the estop is set
 ************************************************************************/

void Lockout_Set_Condition(uint16_t condition);

/************************************************************************
 * FUNCTION: Lockout_Reset_Condition();
 *
 * DESCRIPTION: Used to reset the each lockout condition independently bit.  
 * This needs to be called when a condition reset.
 *
 * INPUT:  condition - this is the bit that will be reset
 * REQUIRES:
 * GUARANTEES:
 * SECRETS: 
 * Example: Lockout_Reset_Condition(LOCKOUT_ESTOP_BIT); This will reset the 
 * lockout bit for the estop, and must be called when the estop is reset or 
 * during when it has been confirmed that the estop is not set.
 ************************************************************************/

void Lockout_Reset_Condition(uint16_t condition);

/************************************************************************
 * FUNCTION: Lockout_Get_Condition();
 *
 * DESCRIPTION: Used to check the status of the lockout condition bit.  
 * This does not check the physical switch.
 *
 * INPUT:  condition - this is the bit that will be read
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 * Example: Lockout_Get_Condition(LOCKOUT_ESTOP_BIT); This can be used to pole
 * each bit to determine which lockout bit are set.  This must be called for 
 * each bit that is or concern.
 ************************************************************************/

bool Lockout_Get_Condition(uint16_t condition);
void lock_out_task(void);


void test_lockout_code(void);
#endif