#include "api.h"
#include "../app.h"
#include "timer.h"
#include "../api/api_canmotor.h"







uint8_t get_motor_current(uint8_t motor_index){
    uint8_t current_amps = 0;
// from is the motor index 0,1,2,3
// pgn is the status index 0,1,2,3  code is limiting recording only 4 status messages
// data is frame data
//    current_amps = API_CanMotor_Return_Status ( motor_index, 0, 1 );     
    return current_amps;     
}




