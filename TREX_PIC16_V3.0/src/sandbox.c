#include <xc.h>
#include "system_config.h"
#include "LED.h"
#include "can.h"




void blink_forward_led(void){
    static TIMER_TICK tick = 0;
    static bool toggle = true;
    
    if( TIMER_Since( tick ) >= 1000 ){
        if(toggle){
            toggle = false;
            LED_column_array[1] |= LED_FORWARD;  // turns led on
        }else{
            toggle = true;
            LED_column_array[1] &= ~LED_FORWARD;   // turns led off
        }
        tick = TIMER_Now();
    }
}


//void can_test_message(can_t *message){
    
void can_test_message(can_t *message){
    
//            message->id |= CANADDR_HMI_SERVICE_LOG;
//            
//            message->buf[1]++;
//         
//            message->buf[2] = 0x01;
//            message->buf[3] = 0x02;
//            message->buf[4] = 0x03;
//            message->buf[5] = 0x04;
//            message->buf[6] = 0x05;
//            message->buf[7] = 0x06;
}


