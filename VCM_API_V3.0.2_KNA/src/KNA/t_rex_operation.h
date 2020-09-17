/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  

// TODO Insert appropriate #include <>

// TODO Insert C++ class definitions if appropriate

// TODO Insert declarations

// Comment a function and leverage automatic documentation with slash star star
/**
    <p><b>Function prototype:</b></p>
  
    <p><b>Summary:</b></p>

    <p><b>Description:</b></p>

    <p><b>Precondition:</b></p>

    <p><b>Parameters:</b></p>

    <p><b>Returns:</b></p>

    <p><b>Example:</b></p>
    <code>
 
    </code>

    <p><b>Remarks:</b></p>
 */
// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation


#define INDEX_TRACTION         0
#define INDEX_BRUSH            1
#define INDEX_VACUUM           2
#define INDEX_SIDEBROOM        3

#define THROTTLE_BRUSH         3000
#define THROTTLE_VACUUM        1000
#define THROTTLE_SIDEBRUSH     3000
#define THROTTLE_BRUSH_ECO     3000
#define THROTTLE_VACUUM_ECO    2500
#define THROTTLE_VACUUM_TOOL   3000
#define THROTTLE_SIDEBRUSH_ECO 3000

#define NO_VACCUUM_DELAY       false
#define VACUUM_DELAY           true



/****************************************************************************
  Function:
      void machine_task(void) 

  Description:  This function is a high level machine operation.  This state machine
 * maintains safe machine operation.  
  Precondition:
    
  Parameters:
 IN:
 * uint8_t mode machine operation. off,normal,eco,transport,wand,maintaince                  
 * uint32_t effective_throttle,   lower value of speed limit and throttle
 * uint32_t throttle, controlled by the foot
 * bool fault, any lock out condition. true stops the machine
 * API_OpPresence_e operator  operator present, not pressent, switch faults
  Return Values:
   none
  Remarks:
 none
 ****************************************************************************/
void machine_task(uint8_t,uint32_t,uint32_t,bool,API_OpPresence_e);


uint32_t get_effective_throttle(void);  // returns low value of throttle or limit
API_OpPresence_e get_operator_status(void);
void turn_off_vacuum(bool);
void vacuum_task(void);
void horn_task(void);
void T_rex_Can1_Initialize( void );
void update_system_messages_task(void);


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

