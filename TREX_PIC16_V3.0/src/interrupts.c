/******************************************************************************
 *
 * Description: Code needed to drive all of the motors inputs
 *
 * Author:      us1jkaze
 * Created:     March 20, 2020, 2:46 PM
 *
 * Changed:     April 30, 2020, 3:15 PM
 * Changed by:  us1jkaze
 * Revision:    Rev: 2.0
 *
 * Copyright (C) 2013-2017 by Alfred Kaercher GmbH & Co. KG.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 ******************************************************************************/

#include <xc.h> // Compiler xc8 (v1.43)
#include <stdio.h>
#include <stdlib.h>
#include "hal.h"
#include "EUSART.h"
#include "buttons.h"
#include "LED.h"
#include "can.h"
#include "system_config.h"
#include "mcp2515_can.h"

TIMER_TICK SYSTEM_tick;

/*******************************************************************************
 * FUNCTION: 
 * DESCRIPTION:
 ******************************************************************************/
void __interrupt () tc_int(void)
{        
    /********************ESART***********************************/
    /*used to send and request data at regular intervals*/
    if( TMR1IF && TMR1IE )
    {
        EUSART_send_data_flag = 1;
        TMR1IF = 0;
    }
    /*interrupts after a byte has been sent and a new byte is ready*/
    if( TX1IF && TX1IE )
    {
        EUSART_Transmit_ISR(); 
    }
    /*interrupts after a byte has been received and is ready to be stored this 
     * may need to be switched to a high priority*/
    if( RC1IF && RC1IE )
    {     
        EUSART_Receive_ISR();
    }
    /********************ESART***********************************/
    
    /********************buttons***********************************/    
    if(IOCIE && PIR0bits.IOCIF)
    {   
        // 'changed' has zeros for any flags that are set. Per the data sheet, ANDing the changed flags
        // to reset the bits in the IOCxF register is the best practice to avoid missed interrupts.
        uint8_t changed = IOCCF ^ 0xFFU;
        IOCIF = 0;
        // CAN communication (only thing not button related)
        if(IOCCFbits.IOCCF1 )
        {
            MCP2515_ISR();
        }
        if(IOCCFbits.IOCCF2)
        {
            button_column_1 = 1;
        }
        if(IOCCFbits.IOCCF3)
        {
            button_column_2 = 1;
        }
        if(IOCCFbits.IOCCF4)
        {
            button_column_3 = 1;
        }
        if(IOCCFbits.IOCCF5)
        {
            button_column_4 = 1;

        }
        IOCCF &= changed;
        button_flag = 1;  
    }
    /********************buttons***********************************/  
    if(PIE0bits.TMR0IE && PIR0bits.TMR0IF)
    {
        SYSTEM_tick++;
        PIR0bits.TMR0IF = 0;
    }
}


