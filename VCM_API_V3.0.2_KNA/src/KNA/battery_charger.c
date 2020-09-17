
#include "mcc_generated_files/can_types.h"
#include "../../src/api/timer.h"
#include "battery_charger.h"
#include "api.h"
#include "api_canmotor.h"
#include "utility.h"									


#define AC_DETECTION_MASK   0b0000000000010000



bool parse_TPD01_message(uCAN_MSG *charger_MSG);
bool parse_TPD02_message(uCAN_MSG *charger_MSG);
bool parse_TPD03_message(uCAN_MSG *charger_MSG);
bool parse_TPD04_message(uCAN_MSG *charger_MSG);

uint32_t charger_battery_voltage_mv(uint32_t);
uint32_t charger_battery_current_ma(uint32_t);
uint32_t charger_Ah_returned_Ah(uint32_t);
uint16_t charger_Wh_returned_Wh(uint16_t);
uint16_t charger_elapsed_time(uint16_t);
uint32_t charger_current_displayed_fault(uint32_t);
uint16_t charger_AC_voltage(uint16_t );
void record_battery_charge_current(uint16_t);
void record_battery_charge_voltage(uint16_t);
void record_Ah_returned(uint32_t);
void record_Wh_returned(uint16_t );
void record_elapsed_time(uint16_t );
void record_current_displayed_fault(uint32_t);
void record_AC_voltage(uint16_t );								  
bool ac_power_detected(uint16_t);
void record_ac_power_status(bool);
void record_extended_charge_status (uint16_t);
void record_battery_state_of_charge(uint8_t charge_percentage);
void set_charger_connected_countdown_counter(void);
// Local variables
static MS_TICK _cycleStart;
uint16_t CHARGE_CURRENT_MA;     // stores values from the charger CAN message
uint16_t CHARGE_VOLTAGE_MV;     // stores value from the charger CAN message
bool AC_POWER_STATUS;           // stores value from the charger CAN message
uint16_t EXTENDED_CHARGE_STATUS; //
uint8_t STATE_OF_CHARGE;
int8_t missing_counter = 2;
bool charger_ac_status = true;
uint32_t AH_RETURNED_AH;           // stores value from the charger CAN message
uint16_t WH_RETURNED_WH;           // stores value from the charger CAN message
uint16_t ELAPSED_TIME;              // stores value from the charger CAN message
uint32_t CURRENT_DISPLAYED_FAULT;   // stores value from the charger CAN message
uint16_t AC_VOLTAGE;                // stores value from the charger CAN message																				


/****************************************************************************
  Function:
      uint32_t charger_battery_voltage_mv(uint32_t raw_value)

  Description:  This function takes the raw value from the charger message and
 * converts it to a mv value
  Precondition:
   byte order should be swapped  
  Parameters:
   IN 16bit value from the can message
  Return Values:
   32 bit value of the battery voltage
  Remarks:
 need to use 32 bit value to hold the large value while calculating the voltage
 ****************************************************************************/
uint32_t charger_battery_voltage_mv(uint32_t raw_value){
    uint16_t result;
    raw_value = raw_value * 1000; // convert value to millivolts
    result = raw_value/256;       // scaling value according to the delta-Q 
    return result;                // document
}


/****************************************************************************
  Function:
      uint32_t charger_battery_current_ma(uint32_t raw_value)

  Description:  This function takes the raw value from the charger message and
 * converts it to a mv value
  Precondition:
   byte order should be swapped  
  Parameters:
   IN 16bit value from the can message
  Return Values:
   32 bit value of the battery voltage
  Remarks:
 need to use 32 bit value to hold the large value while calculating the voltage
 ****************************************************************************/
uint32_t charger_battery_current_ma(uint32_t raw_value){
    uint32_t result;
    result = raw_value * 1000; // convert value to milli_amperes
    result = result/256;       // scaling value according to the delta-Q
    return result;             // document
}

/****************************************************************************
  Function:
      uint32_t charger_Ah_returned_Ah(uint32_t raw_value)

  Description:  This function takes the raw value from the charger message and
 * converts it 
  Precondition:
   byte order should be swapped  
  Parameters:
   IN 32bit value from the can message
  Return Values:
   32 bit value of the battery voltage
  Remarks:
 need to use 32 bit value to hold the large value
 ****************************************************************************/
uint32_t charger_Ah_returned_Ah(uint32_t raw_value){
    uint32_t result;
    result = raw_value;
    return result;             // document
}


/****************************************************************************
  Function:
      uint32_t charger_Wh_returned_Wh(uint32_t raw_value)

  Description:  This function takes the raw value from the charger message and
 * converts it to a mv value
																		 
  Precondition:
   byte order should be swapped  
  Parameters:
   IN 32bit value from the can message
  Return Values:
   32 bit value of the battery voltage
  Remarks:
 need to use 32 bit value to hold the large value while calculating the voltage
 ****************************************************************************/
uint16_t charger_Wh_returned_Wh(uint16_t raw_value){
    uint16_t result;
    result = raw_value;
    return result;             // document
}


/****************************************************************************
  Function:
      uint16_t charger_elapsed_time(uint16_t raw_value){




  Description:  This function takes the raw value from the charger message and
 * converts it to a elapsed time
  Precondition:
   byte order should be swapped  
  Parameters:
   IN 16bit value from the can message
  Return Values:
   16 bit value of the elapsed time
  Remarks:
 need to use 
 ****************************************************************************/
uint16_t charger_elapsed_time(uint16_t raw_value){
    uint16_t result;
    result = raw_value;
    return result;             // document
}



/****************************************************************************
  Function:
      uint32_t charger_current_displayed_fault(uint32_t raw_value)

  Description:  This function takes the raw value from the charger message and
 * converts it 
  Precondition:
   byte order should be swapped  
  Parameters:
   IN 32bit value from the can message
  Return Values:
   32 bit value of the battery voltage
  Remarks:
 need to use 32 bit value to hold current displayed fault
 ****************************************************************************/
uint32_t charger_current_displayed_fault(uint32_t raw_value){
    uint32_t result;
    result = raw_value;
    return result;             // document
}


/****************************************************************************
  Function:
      uint16_t charger_AC_voltage(uint16_t raw_value){

  Description:  This function takes the raw value from the charger message and
 * converts it to a AC voltage
																		 
  Precondition:
   byte order should be swapped  
  Parameters:
   IN 16bit value from the can message
  Return Values:
   16 bit value of the AC voltage
  Remarks:
 need to use 
 ****************************************************************************/
uint16_t charger_AC_voltage(uint16_t raw_value){
    uint16_t result;
    result = raw_value;
    return result;             // document
}



/****************************************************************************
  Function:
      bool handle_charger_can_messages(uCAN_MSG *charger_MSG)

  Description:  This function parses message from the charger
   and extracts the battery voltage, current and AC power status in global variables.  
   this function also keeps the connected count down timer from expiring.
  Precondition:
   none  
  Parameters:
   IN uCAN_MSG *charger_MSG pointer
  Return Values:
   none
  Remarks:
 variables are accessed with get functions
 ****************************************************************************/
bool handle_charger_can_messages(uCAN_MSG *charger_MSG){
    bool parse_success = false;
    set_charger_connected_countdown_counter();											  
    switch(charger_MSG->frame.id){
        case TPD01:
            parse_success = parse_TPD01_message(charger_MSG);
            break;
        case TPD02:
            parse_success = parse_TPD02_message(charger_MSG);
            break;
        case TPD03:
            parse_success = parse_TPD03_message(charger_MSG);
            break;
        case TPD04:
            parse_success = parse_TPD04_message(charger_MSG);
            break;
        default:
            break;    
    }
     return parse_success;           
}


/****************************************************************************
  Function:
      bool parse_TPD01_message(uCAN_MSG *charger_MSG)

  Description:  This function parses the TPD01 message from the charger
   and extracts the battery voltage, current and AC power status in global variables
  Precondition:
   none  
  Parameters:
   IN uCAN_MSG *charger_MSG pointer
  Return Values:
   none
  Remarks:
 variables are accessed with get functions
 ****************************************************************************/
bool parse_TPD01_message(uCAN_MSG *charger_MSG){
    uint32_t raw_value;
    uint16_t current_ma;
    uint16_t voltage_mv;
    bool AC_power_status;
    
    // get charging current
    raw_value = swap_bytes(charger_MSG->frame.data0, charger_MSG->frame.data1);
    current_ma = charger_battery_current_ma(raw_value);
    record_battery_charge_current(current_ma);
    // get battery voltage
    raw_value = swap_bytes(charger_MSG->frame.data2, charger_MSG->frame.data3);
    voltage_mv = charger_battery_voltage_mv(raw_value);
    record_battery_charge_voltage(voltage_mv);
    // get extended charge status
    raw_value = swap_bytes(charger_MSG->frame.data4, charger_MSG->frame.data5);
    AC_power_status = ac_power_detected(raw_value);
    record_ac_power_status(AC_power_status);
	record_extended_charge_status(raw_value);
    
//        TRACE( ( " %x",  motors  ));
 
    return true;
}

bool parse_TPD02_message(uCAN_MSG *charger_MSG){
    uint32_t raw_value;
    uint16_t elapsed_time;
    uint32_t Ah_returned_Ah;
    uint16_t Wh_returned_Wh;
    //get elapsed time
    raw_value = swap_bytes(charger_MSG->frame.data0,charger_MSG->frame.data1); 
    elapsed_time = charger_elapsed_time(raw_value);
    record_elapsed_time(elapsed_time); 
    // get Ah returned
    raw_value = swap_bytes_long(charger_MSG->frame.data2,
            charger_MSG->frame.data3, 
            charger_MSG->frame.data4, 
            charger_MSG->frame.data5);
    Ah_returned_Ah = charger_Ah_returned_Ah(raw_value);
    record_Ah_returned(Ah_returned_Ah);   
    // get Wh returned
    raw_value = swap_bytes(charger_MSG->frame.data6,charger_MSG->frame.data7); 
    Wh_returned_Wh = charger_Wh_returned_Wh(raw_value);
    record_Wh_returned(Wh_returned_Wh);  
    return true;
}

bool parse_TPD03_message(uCAN_MSG *charger_MSG){
    // get charger state of charge
    record_battery_state_of_charge(charger_MSG->frame.data3);
            
    return true;
}



bool parse_TPD04_message(uCAN_MSG *charger_MSG){
    uint32_t raw_value;
    uint16_t current_ma;
    uint16_t voltage_mv;
    bool AC_power_status;
    
    // get charging current
    raw_value = swap_bytes(charger_MSG->frame.data0, charger_MSG->frame.data1);
    current_ma = charger_battery_current_ma(raw_value);
    record_battery_charge_current(current_ma);
    // get battery voltage
    raw_value = swap_bytes(charger_MSG->frame.data2, charger_MSG->frame.data3);
    voltage_mv = charger_battery_voltage_mv(raw_value);
    record_battery_charge_voltage(voltage_mv);
    // get extended charge status
    raw_value = swap_bytes(charger_MSG->frame.data4, charger_MSG->frame.data5);
    AC_power_status = ac_power_detected(raw_value);
    record_ac_power_status(AC_power_status);
    
    return true;
}


/****************************************************************************
  Function:
      void record_ac_power_status(bool ac_status)

  Description:  This is a local function to store the AC power status.
   
  Precondition:
   none  
  Parameters:
   IN bool ac_status: true if AC power is detected; false otherwise
  Return Values:
   none
  Remarks:
   this function is private
 ****************************************************************************/
void record_ac_power_status(bool ac_status){
    AC_POWER_STATUS = ac_status;
}



bool get_ac_power_status(void){
    return AC_POWER_STATUS;
}


/****************************************************************************
  Function:
      void record_extended_charge_status (uint16_t extended_status){

  Description:  This is a local function to store the extended charger status.
   
  Precondition:
   none  
  Parameters:
   IN uint16_t charger status_status:

  Remarks:
   this function is private
 ****************************************************************************/
void record_extended_charge_status (uint16_t extended_charge_status){
    EXTENDED_CHARGE_STATUS = extended_charge_status;
}

uint16_t get_extended_charge_status(void){
    return EXTENDED_CHARGE_STATUS;
}
/****************************************************************************
  Function:
      bool ac_power_detected(uint16_t can_object_2006)

  Description:  This is a private function that takes the can object 2006
    and extracts the AC power detection bit.
   
  Precondition:
   bytes should be swapped.  
  Parameters:
   IN can object 2006
  Return Values:
   bool ac_status: true if AC power is detected; false otherwise
  Remarks:
   this function is private
 ****************************************************************************/
bool ac_power_detected(uint16_t can_object_2006){
    bool result = false;
    if(can_object_2006&AC_DETECTION_MASK)
        result = true;
    return result;
}









/****************************************************************************
  Function:
      record_battery_charge_current(current ma)

  Description:  This is a private function that saves the value to a global 
    variable
   
  Precondition:
   none  
  Parameters:
   IN uint16_t current_ma   current value in units of ma
  Return Values:
   none
  Remarks:
   this function is private
 ****************************************************************************/
void record_battery_charge_current(uint16_t current_ma){
    CHARGE_CURRENT_MA = current_ma;
}




uint16_t get_battery_charge_current(void){
   return CHARGE_CURRENT_MA;
}

void record_battery_charge_voltage(uint16_t voltage_mv){
    CHARGE_VOLTAGE_MV = voltage_mv;
}

uint16_t get_battery_charge_voltage(void){
   return CHARGE_VOLTAGE_MV;
}



void record_battery_state_of_charge(uint8_t charge_percentage){
    if(charge_percentage == 0xff)
       STATE_OF_CHARGE = 0;
   else
       STATE_OF_CHARGE = charge_percentage;
}
   
    
uint8_t get_battery_state_of_charge(void){
   return STATE_OF_CHARGE;
}

void record_Ah_returned(uint32_t Ah_returned_Ah){
    AH_RETURNED_AH = Ah_returned_Ah;
}

uint32_t get_record_Ah_returned(void){
   return AH_RETURNED_AH;
}

void record_Wh_returned(uint16_t Wh_returned_Wh){
    WH_RETURNED_WH = Wh_returned_Wh;
}

uint16_t get_record_Wh_returned(void){
   return WH_RETURNED_WH;
}

void record_elapsed_time(uint16_t elapsed_time){
    ELAPSED_TIME = elapsed_time;
}

uint16_t get_record_elapsed_time(void){
   return ELAPSED_TIME;
}

void record_current_displayed_fault(uint32_t current_displayed_fault){
    CURRENT_DISPLAYED_FAULT = current_displayed_fault;
}

uint32_t get_record_current_displayed_fault(void){
   return CURRENT_DISPLAYED_FAULT;
}

void record_AC_voltage(uint16_t AC_voltage){
    AC_VOLTAGE = AC_voltage;
}

uint16_t get_record_AC_voltage(void){
   return AC_VOLTAGE;
}

//////////////////////////////////////// TEST CODE ////////////////////////////

void charger_can_bus_heartbeat_task(void){
    
//    uCAN_MSG test_msg;
    uCAN_MSG msg;
    MS_TICK elapsed = TIMER_Since( _cycleStart );
    
    // every 1000 ms we adjust duty cycle if necessary
    if( elapsed >= 1000 ){
            

    msg.frame.id = CAN_PING;
    msg.frame.idType = CAN_FRAME_STD;
    msg.frame.dlc = 1;
    msg.frame.data0 = 0xAA;

    API_SendCanMessage( &msg );
        
//        test_msg.frame.idType = CAN_FRAME_STD;
//        test_msg.frame.id = 0x123;
//        test_msg.frame.msgtype = CAN_MSG_DATA;
//                
//        test_msg.frame.dlc   = 8;
//        test_msg.frame.data0 = 0xff;
//        test_msg.frame.data1 = 0xff;
//        test_msg.frame.data2 = 0xff;
//        test_msg.frame.data3 = 0xff;
//        test_msg.frame.data4 = 0xff;
//        test_msg.frame.data5 = 0xff;
//        test_msg.frame.data6 = 0xff;
//        test_msg.frame.data7 = 0xff;
//                
//     
//        API_SendCanMessage(&test_msg);
//        printf("battery test message sent\n");
        _cycleStart = TIMER_Now();
        
    }  
}




void set_charger_connected_countdown_counter(void){
    missing_counter = 2;
}




void set_charger_status(bool is_connected){
    charger_ac_status = is_connected;
}


// true charger is disconnected, false otherwise
bool get_charger_status(void){
    return charger_ac_status;
}

// charger will continue to send messages for about 20 seconds after the 
// AC is removed
void charger_detection_task(void){
    static MS_TICK cycleStart_charger_detection = 0;
    
    MS_TICK elapsed;
    
    elapsed = TIMER_Since( cycleStart_charger_detection );
    
    // every 1000 ms check the status of the charger
    if( elapsed >= 1000 ){
        if(missing_counter <= 0)
            set_charger_status(CHARGER_DISCONNECTED);
        else{
            missing_counter--;
            set_charger_status(CHARGER_CONNECTED);
        }
        cycleStart_charger_detection  = TIMER_Now(); 
    }
}



void battery_test_code(void){
    uCAN_MSG test_msg;
    uint16_t voltage_mv;
    uint16_t current_ma;
//    bool ac_power_detect;
    uint8_t percent_charge;
    
    test_msg.frame.idType = CAN_FRAME_STD;
    test_msg.frame.id = TPD01;
    test_msg.frame.msgtype = CAN_MSG_DATA;  
    test_msg.frame.dlc   = 8;
    test_msg.frame.data0 = 0xf7;    //2002h Charging Current 964 ma
    test_msg.frame.data1 = 0x00;
    test_msg.frame.data2 = 0xd1;    //2101h Battery Voltage 13816 mv
    test_msg.frame.data3 = 0x0d;
    test_msg.frame.data4 = 0x70;    //2006 Extended Charge Status
    test_msg.frame.data5 = 0x12;
    test_msg.frame.data6 = 0x00;
    test_msg.frame.data7 = 0x00;
    
    handle_charger_can_messages(&test_msg);
    test_msg.frame.id = TPD03;
    test_msg.frame.data5 = 0x32;     //50 percent 
    handle_charger_can_messages(&test_msg);
    
    
    percent_charge = get_battery_state_of_charge();
    voltage_mv = get_battery_charge_voltage();
    current_ma = get_battery_charge_current();
//    ac_power_detect = AC_POWER_LOCKOUT();
    
    
}

void AC_power_detection_test(void){
    bool ac_detected;
    static bool previous_ac_detection = false;
    
    ac_detected = get_ac_power_status();
    
    if(ac_detected != previous_ac_detection){
        previous_ac_detection = ac_detected;
        if(ac_detected){
            TRACE( ( " AC power\r\n" ) );
        }
        else{
            TRACE( ( " AC removed\r\n" ) );
        }            
    }
}