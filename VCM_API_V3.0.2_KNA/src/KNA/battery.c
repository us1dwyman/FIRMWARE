/*
 * File:   hour_meters.c
 * Author: us1jbernath
 *
 * Created on July 27, 2020, 9:10 AM
 * 
 * This code is used to control the hour meters.  There are 16 possible hour 
 * meter each is 32 bit long.  This code will read and write the hour meter 
 * values to the EEPROM, turn each on and off and return the value for each
 * hour meter.  
 */

#include "api.h"
#include "utility.h" 
#include "battery.h"
#include "battery_charger.h"
#include "lockout.h"
#include "HMI_interface.h"
#include "EEPROM.h"
#include <stdio.h>
#include <stdlib.h>
#include "t_rex_operation.h"


/* These index into an array of Phoenix controllers information we are 
communicating with*/
#define INDEX_CURRENT       1
#define INDEX_PWM_VOLTAGE   2

/*This is the standby current of the machine with no motors running. This is not
 real current, it is 1A x 2550*/
#define STANDBY_COMPUTE_CURRENT         0

/*This is used to correct the current which is based on the measured output current 
 * and PWM duty cycle.  The value was selected based on testing that was run on 8/26/2020*/
#define COMPUTE_CURRENT_CORRECTION_WITH_PWM     11

/*this is the compute current correction when not calculating with the controller PWM*/
#define COMPUTE_CURRENT_CORRECTION              2550
/*This is the number of consecutive reading under the current threshold 
 * needed before the battery level will change*/
#define BATTERY_THRESHOLD_WAIT_TIME     20

/*This determines how often the battery level is checked.  1000 = 1 second*/
#define BATTERY_SOC_UPDATE_TIMER_MS 1000

/*Transport mode only time in seconds*/
#define BATTERY_TRANSPORT_ONLY_TIME 900

/*CAN message*/
#define NMT_COMMAND         0x00
#define NMT_OPERATIONAL     0x01
#define LITHIUM_NODE_ID     0x10

#define SDO_ID              0x610

#define WRITE_4_BYTE_SENT   0x23
#define WRITE_3_BYTE_SENT   0x27
#define WRITE_2_BYTE_SENT   0x2B
#define WRITE_1_BYTE_SENT   0x2F

#define TPDO1_LOW           0x90
#define TPDO1_HIGH          0x01
#define TPDO2_LOW           0x90
#define TPDO2_HIGH          0x02
#define TPDO3_LOW           0x90
#define TPDO3_HIGH          0x03
#define TPDO4_LOW           0x90
#define TPDO4_HIGH          0x04

#define TPDO1_COM_PAR_OD_INDX_LOW       0x00   
#define TPDO1_COM_PAR_OD_INDX_HIGH      0x18   
#define TPDO2_COM_PAR_OD_INDX_LOW       0x01   
#define TPDO2_COM_PAR_OD_INDX_HIGH      0x18  
#define TPDO3_COM_PAR_OD_INDX_LOW       0x02   
#define TPDO3_COM_PAR_OD_INDX_HIGH      0x18  
#define TPDO4_COM_PAR_OD_INDX_LOW       0x03   
#define TPDO4_COM_PAR_OD_INDX_HIGH      0x18  

#define TPDO1_MAP_PAR_OD_INDX_LOW       0x00   
#define TPDO1_MAP_PAR_OD_INDX_HIGH      0x1A   
#define TPDO2_MAP_PAR_OD_INDX_LOW       0x01   
#define TPDO2_MAP_PAR_OD_INDX_HIGH      0x1A
#define TPDO3_MAP_PAR_OD_INDX_LOW       0x02   
#define TPDO3_MAP_PAR_OD_INDX_HIGH      0x1A  
#define TPDO4_MAP_PAR_OD_INDX_LOW       0x03   
#define TPDO4_MAP_PAR_OD_INDX_HIGH      0x1A  

#define SUB_INDEX_00h                   0x00
#define SUB_INDEX_01h                   0x01
#define SUB_INDEX_02h                   0x02
#define SUB_INDEX_03h                   0x03
#define SUB_INDEX_04h                   0x04

#define COB_ID_SET_BIT_31_HIGH          0x80
#define COB_ID_SET_BIT_31_LOW           0x00
#define COB_ID_CLEAR_BIT_31_HIGH        0x00
#define COB_ID_CLEAR_BIT_31_LOW         0x00

#define STATE_OF_HEALTH_OBJECT_HIGH     0x20
#define STATE_OF_HEALTH_OBJECT_LOW      0x03
#define STATE_OF_HEALTH_SUB_INDEX       0x02
#define STATE_OF_HEALTH_LENGTH          0x08

#define CELL_MIN_VOLT_OBJECT_HIGH       0x20
#define CELL_MIN_VOLT_OBJECT_LOW        0x13
#define CELL_MIN_VOLT_SUB_INDEX         0x01
#define CELL_MIN_VOLT_LENGTH            0x10

#define MAPPED_OBJECT_TO_0              0x00
#define MAPPED_OBJECT_TO_1              0x01
#define MAPPED_OBJECT_TO_2              0x02
#define MAPPED_OBJECT_TO_3              0x03
#define MAPPED_OBJECT_TO_4              0x04
#define MAPPED_OBJECT_TO_5              0x05
#define MAPPED_OBJECT_TO_6              0x06
#define MAPPED_OBJECT_TO_7              0x07
#define MAPPED_OBJECT_TO_8              0x08

#define LITHIUM_DISCONNECTED true
#define LITHIUM_CONNECTED false

#define MAP_TPDO2_UPDATE_TIMER_MS       50

//111 is based on a current draw of 0.1A
#define AH_CONSUMED_STANDBY             111

//make sure that this value does the running average to roll over.
#define SAMPLE_FOR_RUNNING_AVERAGE      60

#define CHARGE_INDICATION_MASK          0x0F00
#define CHARGE_INDICATION_COMPLETE      0x0400

#define CHARGER_TO_VCM_AH_CONVERSION    1250000
typedef enum
{
  REMAP_TPDO2_DISABLE           = 0,
  REMAP_TPDO2_SET_OBJECT_TO_0   = 1,
  REMAP_TPDO2_ADD_MIN_CELL_VOLT = 2,
  REMAP_TPDO2_ADD_HEALTH        = 3,
  REMAP_TPDO2_OBJECT_QTY        = 4,
  REMAP_TPDO2_ENABLE            = 5

}REMAP_TPDO2_MESSAGES;

int8_t missing_lithium_counter = 2;
bool lithium_status = true;

/*This is used to track the current battery level. There are 8 battery levels to 
 * track 0%, 13%, 25%, 38%, 50% 63%, 75%, 88%, 100%. 8=100% 0=0%. */
static uint8_t battery_SOC_level;

/*This is used to health of the battery and is based on the reported Ah returned 
 * from the last know full charge cycle */
static uint32_t battery_health;
static uint8_t battery_threshold_wait_time_index = 0;

void Battery_Check_Charger_Status(void);
uint32_t Battery_Total_Compute_Current_Get(void);
uint32_t Battery_DMC_Compute_Current_Get(uint8_t motor_index);
bool Battery_DMC_Motor_On_Off_Status(void);
void Battery_Check_Transport_Mode_Only_Time(void);
bool Battery_Check_Voltage_VS_Threshold(uint32_t total_current_draw);
void Battery_Service_SOC_Level(void);

bool parse_TPD01bat_message(uCAN_MSG *battery_MSG);
bool parse_TPD02bat_message(uCAN_MSG *battery_MSG);
bool parse_TPD03bat_message(uCAN_MSG *battery_MSG);
bool parse_TPD04bat_message(uCAN_MSG *battery_MSG);

void set_lithium_connected_countdown_counter(void);
void set_lithium_status(bool is_connected);
bool get_lithium_status(void);
void lithium_detection_task(void);

void lithium_disable_TPDO2 (void);
void lithium_set_TPDO2_to_0_objects (void);
void lithium_add_min_cell_volt_to_TPDO2 (void);
void lithium_add_health_to_TPDO2 (void);
void lithium_set_TPDO2_object_qty (void);
void lithium_enable_TPDO2 (void);

void battery_coulomb_count_set(uint32_t coulomb_count);
void battery_coulomb_count_update(uint32_t total_current_draw);
uint32_t battery_coulomb_count_get(void);
void battery_total_current_draw_mode_running_average_set(uint32_t total_current_draw);

void battery_check_charge_cycle_type(void); 
uint32_t battery_coulomb_value;

/*coulomb counting look up table.  This corrects the Ah consumed at the current 
 * amp draw of the machine back the 125 amp hour rating of the battery, the 
 * of 10,000,000 compute Ah to actual Ah, and the sample rate of 1 second.*/
const uint32_t Ah_consumed_at_specfic_current_draws [101] = {
    0,1674,3789,6108,8572,11149,13821,16573,19396,22283,25227,
    28225,31271,34363,37498,40673,43886,47134,50417,53733,57080,
    60456,63862,67295,70755,74241,77751,81286,84844,88425,92027,
    95652,99297,102962,106648,110352,114076,117818,121578,125355,
    129150,132962,136791,140635,144496,148372,152264,156171,160092,
    164029,167979,171944,175922,179914,183920,187939,191970,196015,
    200072,204142,208224,212318,216424,220542,224672,228813,232965,
    237129,241304,245489,249686,253893,258111,262339,266578,270826,
    275085,279354,283633,287921,292219,296527,300844,305171,309506,
    313851,318206,322569,326941,331322,335712,340110,344517,348933,
    353357,357789,362230,366679,371136,375601,380074,
};

static uint32_t normal_mode_running_current_draw [SAMPLE_FOR_RUNNING_AVERAGE];
static uint8_t normal_mode_running_current_draw_index;

static uint32_t eco_mode_running_current_draw [SAMPLE_FOR_RUNNING_AVERAGE];
static uint8_t eco_mode_running_current_draw_index;

static uint32_t tool_mode_running_current_draw [SAMPLE_FOR_RUNNING_AVERAGE];
static uint8_t tool_mode_running_current_draw_index;
/************************************************************************
 * FUNCTION: battery_Initialize();
 *
 * DESCRIPTION: initializes the battery level monitor
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
void Battery_Initialize(void)
{
    uCAN_MSG msg;
    
    battery_SOC_level = (BATTERY_NUMBER_OF_CHARGE_LEVELS - 1);
    
    msg.frame.id = NMT_COMMAND;
    msg.frame.idType = CAN_FRAME_STD;
    msg.frame.dlc = 2;
    msg.frame.data0 = NMT_OPERATIONAL;
    msg.frame.data1 = LITHIUM_NODE_ID;
    
    API_SendCanMessage( &msg );
    
    lithium_remap_TPDO2();
}

/************************************************************************
 * FUNCTION: Battery_Tasks();
 *
 * DESCRIPTION: Check to see if the battery needs to be updated and updates.
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
void Battery_Tasks(void)
{       
    static MS_TICK _batterySOCStart = 0;   
    MS_TICK _updateBatterySOCElapsed;
    _updateBatterySOCElapsed = TIMER_Since( _batterySOCStart );
	uint32_t total_current_draw = 0;
       
    if( _updateBatterySOCElapsed >= BATTERY_SOC_UPDATE_TIMER_MS )
    {
        Battery_Check_Charger_Status();
        
        if(Battery_DMC_Motor_On_Off_Status())
        {
			total_current_draw = Battery_Total_Compute_Current_Get();
            /*battery state of charge*/
            /*check to see if the motor is in transport only mode*/
            if (battery_SOC_level == BATTERY_0_SOC)
            {
                Battery_Check_Transport_Mode_Only_Time();
            }
            else if ( Battery_Check_Voltage_VS_Threshold(total_current_draw))
            {
                Battery_Service_SOC_Level();
            }
        }
		        /*battery coulomb counting*/
        /*check to see if this machine has a charger connected, if 
        not coulomb counting does not work*/
        if (!get_charger_status())
        {
            battery_coulomb_count_update(total_current_draw);
            battery_total_current_draw_mode_running_average_set(total_current_draw);
            battery_check_charge_cycle_type(); 
       }
        _batterySOCStart = TIMER_Now();     
    }
}


/************************************************************************
 * FUNCTION: Battery_Check_Charger_Status();
 *
 * DESCRIPTION: Check the charger status.  If its plugged in reset battery 
 * SOC to 100%
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
void Battery_Check_Charger_Status(void)
{
    if( get_ac_power_status())
    {
        battery_SOC_level = (BATTERY_NUMBER_OF_CHARGE_LEVELS - 1);
    }
}

/************************************************************************
 * FUNCTION: Battery_Total_Current_Get();
 *
 * DESCRIPTION: Calculate the total machine current.
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
uint32_t Battery_Total_Compute_Current_Get(void)
{
    uint32_t total_compute_current;
    
    total_compute_current = Battery_DMC_Compute_Current_Get(INDEX_TRACTION)
        + Battery_DMC_Compute_Current_Get(INDEX_BRUSH)
        + Battery_DMC_Compute_Current_Get(INDEX_VACUUM)
        + Battery_DMC_Compute_Current_Get(INDEX_SIDEBROOM)
        + STANDBY_COMPUTE_CURRENT;

    return total_compute_current;
}

/************************************************************************
 * FUNCTION: Battery_Total_Current_Get();
 *
 * DESCRIPTION: return the current measurements from the DMC and convert to 
 * compute values Compute values are not real current, they are proportional 
 * to the input current to the DMC.  Current values can be negative so 
 * absolute values are needed.
 * checked
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
uint32_t Battery_DMC_Compute_Current_Get(uint8_t motor_index)
{
    uint32_t motor_compute_current;
     motor_compute_current = (uint32_t) abs (API_CanMotor_Return_Status(motor_index,0,INDEX_CURRENT))
        * API_CanMotor_Return_Status(motor_index,0,INDEX_PWM_VOLTAGE)
        * COMPUTE_CURRENT_CORRECTION_WITH_PWM;
    return motor_compute_current;
}

/************************************************************************
 * FUNCTION: Battery_DMC_Motor_On_Off_Status();
 *
 * DESCRIPTION: Check to see if any of the motors are running
 * checked
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
bool Battery_DMC_Motor_On_Off_Status(void)
{  
    if ( API_CanMotor_Return_Status(INDEX_TRACTION,0,INDEX_PWM_VOLTAGE)
        || API_CanMotor_Return_Status(INDEX_BRUSH,0,INDEX_PWM_VOLTAGE)
        || API_CanMotor_Return_Status(INDEX_VACUUM,0,INDEX_PWM_VOLTAGE)
        || API_CanMotor_Return_Status(INDEX_SIDEBROOM,0,INDEX_PWM_VOLTAGE) )
    {
        return true;
    }
    else 
    {
        return false;
    }
}

/************************************************************************
 * FUNCTION: Battery_Check_Transport_Mode_Only_Time();
 *
 * DESCRIPTION: check to see if the machine is in transport only mode. If so
 * count up to a defined time to determine if the machine needs to be shut down
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
void Battery_Check_Transport_Mode_Only_Time(void)
{
    static uint16_t battery_threshold_wait_time_index;
    
    if (++battery_threshold_wait_time_index >= BATTERY_TRANSPORT_ONLY_TIME)
    {
        battery_SOC_level = BATTERY_DEAD;
        Lockout_Set_Condition(LOCKOUT_BATTERY_DEAD);
    }
}

/************************************************************************
 * FUNCTION: Battery_Check_Voltage_VS_Threshold();
 *
 * DESCRIPTION: Check to see if the battery voltage is under the current
 * threshold vs total machine current
 *
 * INPUT:  total current draw for the machine
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
bool Battery_Check_Voltage_VS_Threshold(uint32_t total_current_draw)
{
    if ((battery_y_intercept[battery_SOC_level])
        - (total_current_draw
        / battery_slope[battery_SOC_level]) 
        > (get_battery_charge_voltage() ))
    {
        return true;
    } 
    else 
    {
        battery_threshold_wait_time_index = 0;
        return false;
    }
}

/************************************************************************
 * FUNCTION: Battery_Service_SOC_Level();
 *
 * DESCRIPTION: Update the state of charge level if there have been enough
 * consecutive under threshold events.
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
void Battery_Service_SOC_Level(void)
{
    if (++battery_threshold_wait_time_index > BATTERY_THRESHOLD_WAIT_TIME)
    {
        /*Is the battery a 0% SOC, if so lock it out but allow transport*/
        if (--battery_SOC_level == BATTERY_0_SOC)
        {
            Lockout_Set_Condition(LOCKOUT_BATTERY_0_SOC);
        }
        battery_threshold_wait_time_index = 0;
    }
}

/************************************************************************
 * FUNCTION: Battery_SOC_Get();
 *
 * DESCRIPTION: Gets the battery state of charge level 
 * checked
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
uint8_t Battery_SOC_Get(void)
{
    return battery_SOC_level;
}

/************************************************************************
 * FUNCTION: Battery_SOC_Get();
 *
 * DESCRIPTION: Sets the battery state of charge level 
 * checked
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
void Battery_SOC_Set(uint8_t new_battery_SOC_level)
{
    battery_SOC_level = new_battery_SOC_level;
}
/****************************************************************************
  Function:
      bool handle_battery_can_messages(uCAN_MSG *charger_MSG)

  Description:  This function parses message from the battery
   and extracts the data from the battery
   none  
  Parameters:
   IN uCAN_MSG *charger_MSG pointer
  Return Values:
   none
  Remarks:
 variables are accessed with get functions
 ****************************************************************************/
bool handle_battery_can_messages(uCAN_MSG *battery_MSG){
    bool parse_success = false;
    set_lithium_connected_countdown_counter();											  
    switch(battery_MSG->frame.id){
        case TPD01BAT:
            parse_success = parse_TPD01bat_message(battery_MSG);
            break;
        case TPD02BAT:
            parse_success = parse_TPD02bat_message(battery_MSG);
            break;
        case TPD03BAT:
            parse_success = parse_TPD03bat_message(battery_MSG);
            break;
        case TPD04BAT:
            parse_success = parse_TPD04bat_message(battery_MSG);
            break;
        default:
            break;    
    }
     return parse_success;           
}

void set_lithium_connected_countdown_counter(void){
    missing_lithium_counter = 2;
}

void set_lithium_status(bool is_connected){
    lithium_status = is_connected;
}

// true charger is disconnected, false otherwise
bool get_lithium_status(void){
    return lithium_status;
}

// charger will continue to send messages for about 20 seconds after the 
// AC is removed
void lithium_detection_task(void){
    static MS_TICK cycleStart_lithium_detection = 0;
    
    MS_TICK elapsed;
    
    elapsed = TIMER_Since( cycleStart_lithium_detection );
    
    // every 1000 ms check the status of the charger
    if( elapsed >= 1000 ){
        if(missing_lithium_counter <= 0)
            set_lithium_status(LITHIUM_DISCONNECTED);
        else{
            missing_lithium_counter--;
            set_lithium_status(LITHIUM_CONNECTED);
        }
        cycleStart_lithium_detection  = TIMER_Now(); 
    }
}

/****************************************************************************
  Function:
      bool parse_TPD01bat_message(uCAN_MSG *charger_MSG)

  Description:  This function parses the TPD01 message from the battery
   and extracts date:
   none  
  Parameters:
   IN uCAN_MSG *charger_MSG pointer
  Return Values:
   none
  Remarks:
 variables are accessed with get functions
 ****************************************************************************/
bool parse_TPD01bat_message(uCAN_MSG *battery_MSG){

    return true;
}
/****************************************************************************
  Function:
      bool parse_TPD02bat_message(uCAN_MSG *charger_MSG)

  Description:  This function parses the TPD01 message from the battery
   and extracts date:
   none  
  Parameters:
   IN uCAN_MSG *charger_MSG pointer
  Return Values:
   none
  Remarks:
 variables are accessed with get functions
 ****************************************************************************/
bool parse_TPD02bat_message(uCAN_MSG *battery_MSG){

    return true;
}
/****************************************************************************
  Function:
      bool parse_TPD03bat_message(uCAN_MSG *charger_MSG)

  Description:  This function parses the TPD01 message from the battery
   and extracts date:
   none  
  Parameters:
   IN uCAN_MSG *charger_MSG pointer
  Return Values:
   none
  Remarks:
 variables are accessed with get functions
 ****************************************************************************/
bool parse_TPD03bat_message(uCAN_MSG *battery_MSG){

    return true;
}
   
/****************************************************************************
  Function:
      bool parse_TPD04bat_message(uCAN_MSG *charger_MSG)

  Description:  This function parses the TPD01 message from the battery
   and extracts date:
   none  
  Parameters:
   IN uCAN_MSG *charger_MSG pointer
  Return Values:
   none
  Remarks:
 variables are accessed with get functions
 ****************************************************************************/
bool parse_TPD04bat_message(uCAN_MSG *battery_MSG){

    return true;
}

/****************************************************************************
  Function:
      remap_TPDO2(void)

  Description:  remap TPDO2 to include minimum cell voltage and state of health
   none  
  Parameters:
  Return Values:
   none
  Remarks:
 variables are accessed with get functions
 ****************************************************************************/
void lithium_remap_TPDO2 (void)
{
    static uint8_t _remap_TPDO2_steps = 0;
    static MS_TICK _remapTPDO2Start = 0;   
    MS_TICK _remapTPDO2Elapsed;
    _remapTPDO2Elapsed = TIMER_Since( _remapTPDO2Start );
       
    if( _remapTPDO2Elapsed >= MAP_TPDO2_UPDATE_TIMER_MS && _remap_TPDO2_steps < 6)
    {
        switch (_remap_TPDO2_steps)
        {
            case REMAP_TPDO2_DISABLE:
                lithium_disable_TPDO2();
                break;
            case REMAP_TPDO2_SET_OBJECT_TO_0:
                lithium_set_TPDO2_to_0_objects();
                break;
            case REMAP_TPDO2_ADD_MIN_CELL_VOLT:
                lithium_add_min_cell_volt_to_TPDO2();
                break;
            case REMAP_TPDO2_ADD_HEALTH:
                lithium_add_health_to_TPDO2();
                break;
            case REMAP_TPDO2_OBJECT_QTY:
                lithium_set_TPDO2_object_qty();
                break;
            case REMAP_TPDO2_ENABLE:
                lithium_enable_TPDO2();
                break;
            default:break;
        }
        _remap_TPDO2_steps++;
        _remapTPDO2Start = TIMER_Now(); 
    }
}

/****************************************************************************
  Function:
      lithium_disable_TPDO2(void)

  Description:  Disable TPDO2 to start remapping.
   none  
  Parameters:
  Return Values:
   none
  Remarks:
 variables are accessed with get functions
 ****************************************************************************/
void lithium_disable_TPDO2 (void){
   
    uCAN_MSG msg;
        
    msg.frame.id = SDO_ID;
    msg.frame.idType = CAN_FRAME_STD;
    msg.frame.dlc = 8;
    msg.frame.data0 = WRITE_4_BYTE_SENT;
    msg.frame.data1 = TPDO2_COM_PAR_OD_INDX_LOW;
    msg.frame.data2 = TPDO2_COM_PAR_OD_INDX_HIGH;
    msg.frame.data3 = SUB_INDEX_01h;
    msg.frame.data4 = TPDO2_LOW;
    msg.frame.data5 = TPDO2_HIGH;
    msg.frame.data6 = COB_ID_SET_BIT_31_LOW;
    msg.frame.data7 = COB_ID_SET_BIT_31_HIGH;
    
    API_SendCanMessage( &msg );
}

/****************************************************************************
  Function:
      lithium_set_TPDO2_to_0(void)

  Description:  Clear TPDO2.
   none  
  Parameters:
  Return Values:
   none
  Remarks:
 variables are accessed with get functions
 ****************************************************************************/
void lithium_set_TPDO2_to_0_objects (void){
   
    uCAN_MSG msg;
        
    msg.frame.id = SDO_ID;
    msg.frame.idType = CAN_FRAME_STD;
    msg.frame.dlc = 8;
    msg.frame.data0 = WRITE_1_BYTE_SENT;
    msg.frame.data1 = TPDO2_MAP_PAR_OD_INDX_LOW;
    msg.frame.data2 = TPDO2_MAP_PAR_OD_INDX_HIGH;
    msg.frame.data3 = SUB_INDEX_00h;
    msg.frame.data4 = MAPPED_OBJECT_TO_0;
    msg.frame.data5 = MAPPED_OBJECT_TO_0;
    msg.frame.data6 = MAPPED_OBJECT_TO_0;
    msg.frame.data7 = MAPPED_OBJECT_TO_0;
    
    API_SendCanMessage( &msg );
}

/****************************************************************************
  Function:
      lithium_ADD_MIN_CELL_VOLT_TO_TPDO2(void)

  Description:  Add minimum cell voltage to TPDO2.
   none  
  Parameters:
  Return Values:
   none
  Remarks:
 variables are accessed with get functions
 ****************************************************************************/
void lithium_add_min_cell_volt_to_TPDO2 (void){
   
    uCAN_MSG msg;
        
    msg.frame.id = SDO_ID;
    msg.frame.idType = CAN_FRAME_STD;
    msg.frame.dlc = 8;
    msg.frame.data0 = WRITE_4_BYTE_SENT;
    msg.frame.data1 = TPDO2_MAP_PAR_OD_INDX_LOW;
    msg.frame.data2 = TPDO2_MAP_PAR_OD_INDX_HIGH;
    msg.frame.data3 = SUB_INDEX_01h;
    msg.frame.data4 = CELL_MIN_VOLT_LENGTH;
    msg.frame.data5 = CELL_MIN_VOLT_SUB_INDEX;
    msg.frame.data6 = CELL_MIN_VOLT_OBJECT_LOW;
    msg.frame.data7 = CELL_MIN_VOLT_OBJECT_HIGH;
    
    API_SendCanMessage( &msg );
}

/****************************************************************************
  Function:
      lithium_add_health_to_TPDO2(void)

  Description:  Add battery health to TPDO2.
   none  
  Parameters:
  Return Values:
   none
  Remarks:
 variables are accessed with get functions
 ****************************************************************************/
void lithium_add_health_to_TPDO2 (void){
   
    uCAN_MSG msg;
        
    msg.frame.id = SDO_ID;
    msg.frame.idType = CAN_FRAME_STD;
    msg.frame.dlc = 8;
    msg.frame.data0 = WRITE_4_BYTE_SENT;
    msg.frame.data1 = TPDO2_MAP_PAR_OD_INDX_LOW;
    msg.frame.data2 = TPDO2_MAP_PAR_OD_INDX_HIGH;
    msg.frame.data3 = SUB_INDEX_01h;
    msg.frame.data4 = STATE_OF_HEALTH_LENGTH;
    msg.frame.data5 = STATE_OF_HEALTH_SUB_INDEX;
    msg.frame.data6 = STATE_OF_HEALTH_OBJECT_LOW;
    msg.frame.data7 = STATE_OF_HEALTH_OBJECT_HIGH;
    
    API_SendCanMessage( &msg );
}

/****************************************************************************
  Function:
      lithium_set_TPDO2_object_qty(void)

  Description:  Clear TPDO2.
   none  
  Parameters:
  Return Values:
   none
  Remarks:
 variables are accessed with get functions
 ****************************************************************************/
void lithium_set_TPDO2_object_qty (void){
   
    uCAN_MSG msg;
        
    msg.frame.id = SDO_ID;
    msg.frame.idType = CAN_FRAME_STD;
    msg.frame.dlc = 8;
    msg.frame.data0 = WRITE_1_BYTE_SENT;
    msg.frame.data1 = TPDO2_MAP_PAR_OD_INDX_LOW;
    msg.frame.data2 = TPDO2_MAP_PAR_OD_INDX_HIGH;
    msg.frame.data3 = SUB_INDEX_00h;
    msg.frame.data4 = MAPPED_OBJECT_TO_2;
    msg.frame.data5 = MAPPED_OBJECT_TO_0;
    msg.frame.data6 = MAPPED_OBJECT_TO_0;
    msg.frame.data7 = MAPPED_OBJECT_TO_0;
    
    API_SendCanMessage( &msg );
}

/****************************************************************************
  Function:
      lithium_enable_TPDO2(void)

  Description:  Disable TPDO2 to start remapping.
   none  
  Parameters:
  Return Values:
   none
  Remarks:
 variables are accessed with get functions
 ****************************************************************************/
void lithium_enable_TPDO2 (void){
   
    uCAN_MSG msg;
        
    msg.frame.id = SDO_ID;
    msg.frame.idType = CAN_FRAME_STD;
    msg.frame.dlc = 8;
    msg.frame.data0 = WRITE_4_BYTE_SENT;
    msg.frame.data1 = TPDO2_COM_PAR_OD_INDX_LOW;
    msg.frame.data2 = TPDO2_COM_PAR_OD_INDX_HIGH;
    msg.frame.data3 = SUB_INDEX_01h;
    msg.frame.data4 = TPDO2_LOW;
    msg.frame.data5 = TPDO2_HIGH;
    msg.frame.data6 = COB_ID_CLEAR_BIT_31_LOW;
    msg.frame.data7 = COB_ID_CLEAR_BIT_31_HIGH;
    
    API_SendCanMessage( &msg );
}

/************************************************************************
 * FUNCTION: battery_coulomb_count_motor_set();
 *
 * DESCRIPTION: Determines how much Ah to remove from the battery.
 *
 * INPUT:  total_current_draw
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
void battery_coulomb_count_set(uint32_t coulomb_count)
{
    battery_coulomb_value = coulomb_count;

} 
/************************************************************************
 * FUNCTION: battery_coulomb_count_motor_set();
 *
 * DESCRIPTION: Determines how much Ah to remove from the battery.
 *
 * INPUT:  total_current_draw
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
void battery_coulomb_count_update(uint32_t total_current_draw)
{
    battery_coulomb_value = battery_coulomb_value 
            - Ah_consumed_at_specfic_current_draws [total_current_draw / COMPUTE_CURRENT_CORRECTION]
            - AH_CONSUMED_STANDBY;
}  
/************************************************************************
 * FUNCTION: battery_coulomb_count_get();
 *
 * DESCRIPTION: returns current coulomb count
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
uint32_t battery_coulomb_count_get(void)
{
    return battery_coulomb_value;
}

/************************************************************************
 * FUNCTION: battery_total_current_draw_mode_running_average_set();
 *
 * DESCRIPTION: check the current operating mode, and adds a new current value 
 * to it this is a running average and uses FIFO
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
void battery_total_current_draw_mode_running_average_set(uint32_t total_current_draw)
{   
    uint8_t operating_mode;
    
    operating_mode = get_operating_mode();
    
    switch (operating_mode)
    {
        case MODE_DEEP:
            if (++normal_mode_running_current_draw_index == SAMPLE_FOR_RUNNING_AVERAGE)
            {
                normal_mode_running_current_draw_index = 0;
            }
            normal_mode_running_current_draw [normal_mode_running_current_draw_index] 
                    = total_current_draw;
            break;
            
        case MODE_ECO:
            if (++eco_mode_running_current_draw_index == SAMPLE_FOR_RUNNING_AVERAGE)
            {
                eco_mode_running_current_draw_index = 0;
            }
            eco_mode_running_current_draw [eco_mode_running_current_draw_index] 
                    = total_current_draw;
            break;
            
        case MODE_TOOL:
            if (++tool_mode_running_current_draw_index == SAMPLE_FOR_RUNNING_AVERAGE)
            {
                tool_mode_running_current_draw_index = 0;
            }
            eco_mode_running_current_draw [tool_mode_running_current_draw_index] 
                    = total_current_draw;
            break;
            
        default:break;
    }
}

/************************************************************************
 * FUNCTION: battery_runtime_get();
 *
 * Returns the runtime in seconds
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
uint32_t battery_runtime_get(void)
{
    uint32_t total_current_draw = 0; 
    uint32_t battery_runtime = 0;  
    uint8_t operating_mode;
    uint8_t i;
    
    operating_mode = get_operating_mode();
    
    switch (operating_mode)
    {
        case MODE_DEEP:
            for(i=0; i++; i<SAMPLE_FOR_RUNNING_AVERAGE)
            {
                total_current_draw += normal_mode_running_current_draw[i];
            }
            break;
            
        case MODE_ECO:
            for(i=0; i++; i<SAMPLE_FOR_RUNNING_AVERAGE)
            {
                total_current_draw += eco_mode_running_current_draw[i];
            }
            break;
            
        case MODE_TOOL:
            for(i=0; i++; i<SAMPLE_FOR_RUNNING_AVERAGE)
            {
                total_current_draw += tool_mode_running_current_draw[i];
            }
            break;
            
        default:break;
    }
    total_current_draw = total_current_draw 
            / SAMPLE_FOR_RUNNING_AVERAGE
            / COMPUTE_CURRENT_CORRECTION;
    
    battery_runtime = battery_coulomb_count_get()/
            Ah_consumed_at_specfic_current_draws [total_current_draw];
    
    return battery_runtime;
}

/************************************************************************
 * FUNCTION: Battery_Health_Get();
 *
 * DESCRIPTION: Gets the battery health 
 * checked
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
uint32_t Battery_Health_Get(void)
{
    return battery_health;
}

/************************************************************************
 * FUNCTION: Battery_Health_Set();
 *
 * DESCRIPTION: Sets the battery health
 * checked
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
void Battery_Health_Set(uint32_t new_battery_health)
{
    battery_health = new_battery_health;
}

/************************************************************************
 * FUNCTION: battery_check_charge_cycle_type();
 *
 * Returns the runtime in seconds
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
void battery_check_charge_cycle_type(void)
{
    static bool full_charge_cycle_start_flag;
    static bool complete_charge_cycle_flag;
    static bool charge_cycle_detected;
    
    if( Battery_SOC_Get() == BATTERY_0_SOC )
    {
        full_charge_cycle_start_flag = true;
    }
    if(get_ac_power_status())
    {
        charge_cycle_detected = true;
         if ((get_extended_charge_status() & CHARGE_INDICATION_MASK) == CHARGE_INDICATION_COMPLETE)
         {
             complete_charge_cycle_flag = true;
         }
    }
    if(!(get_ac_power_status()) && (charge_cycle_detected) )
    {
        if( full_charge_cycle_start_flag && complete_charge_cycle_flag )
        {
            Battery_Health_Set(( (uint32_t) get_record_Ah_returned() * CHARGER_TO_VCM_AH_CONVERSION ));
            battery_coulomb_count_set (Battery_Health_Get());
        }
        else if ( complete_charge_cycle_flag )
        {
            battery_coulomb_count_set (Battery_Health_Get());
        }
        else
        {
            if (Battery_Health_Get() < ((get_record_Ah_returned() * CHARGER_TO_VCM_AH_CONVERSION ) + (battery_coulomb_count_get())))
            {
                battery_coulomb_count_set (Battery_Health_Get());
            }
            else
            {
                battery_coulomb_count_set ((uint32_t) (get_record_Ah_returned() * CHARGER_TO_VCM_AH_CONVERSION ) + (battery_coulomb_count_get()) );
            }
        }
        full_charge_cycle_start_flag = false;
        complete_charge_cycle_flag = false;
        charge_cycle_detected = false;
    }  
}

/************************************************************************
 * FUNCTION: Battery_Read_EEPROM();
 *
 * Returns the runtime in seconds
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
void Battery_Read_EEPROM(void)
{
    uint8_t battery_byte_array[ BATTERY_COULOMB_COUNT_BYTE_LENGTH   
                + BATTERY_HEALTH_BYTE_LENGTH              
                + BATTERY_SOC_BYTE_LENGTH ];
    static uint8_t battery_SOC_temporary;
    static uint32_t battery_health_temporary;
    static uint32_t battery_coulomb_value_temporary;
    int16_t i;
    API_ReadNvram( BATTERY_EEPROM_PAGE_ADDRESS * PAGE_LENGTH
            , battery_byte_array
            , BATTERY_COULOMB_COUNT_BYTE_LENGTH   
                + BATTERY_HEALTH_BYTE_LENGTH              
                + BATTERY_SOC_BYTE_LENGTH );
    
    for(i = BATTERY_COULOMB_COUNT_BYTE_LENGTH + BATTERY_HEALTH_BYTE_LENGTH + BATTERY_SOC_BYTE_LENGTH; 
            i > BATTERY_COULOMB_COUNT_BYTE_LENGTH + BATTERY_HEALTH_BYTE_LENGTH ; i--)
    {
        battery_SOC_temporary += battery_byte_array[i-1]; 
    }
    for(i = BATTERY_COULOMB_COUNT_BYTE_LENGTH + BATTERY_HEALTH_BYTE_LENGTH; 
            i >= BATTERY_COULOMB_COUNT_BYTE_LENGTH ; i--)
    {
        battery_health_temporary = battery_health_temporary << BYTE_LENGTH;
        battery_health_temporary += battery_byte_array[i]; 
    }
    for(i = BATTERY_COULOMB_COUNT_BYTE_LENGTH; i >= 0; i--)
    {
        battery_coulomb_value_temporary = battery_coulomb_value_temporary << BYTE_LENGTH;
        battery_coulomb_value_temporary += battery_byte_array[i]; 
    }
    Battery_SOC_Set(battery_SOC_temporary);
    Battery_Health_Set(battery_health_temporary);
    battery_coulomb_count_set(battery_coulomb_value_temporary);
}

/************************************************************************
 * FUNCTION: Battery_Write_EEPROM();
 *
 * Writes battery information to the EEPROM
 *
 * INPUT:  
 * REQUIRES:
 * GUARANTEES:
 * SECRETS:
 ************************************************************************/
void Battery_Write_EEPROM(void)
{
    uint8_t battery_byte_array[ BATTERY_COULOMB_COUNT_BYTE_LENGTH   
                + BATTERY_HEALTH_BYTE_LENGTH              
                + BATTERY_SOC_BYTE_LENGTH ];
    uint16_t i;
    
    for (i = 0; i <BATTERY_COULOMB_COUNT_BYTE_LENGTH; i++)
    {
        battery_byte_array[i] = battery_coulomb_count_get() >> (i * BYTE_LENGTH);
    }
    for (i = BATTERY_COULOMB_COUNT_BYTE_LENGTH; 
            i <BATTERY_COULOMB_COUNT_BYTE_LENGTH + BATTERY_HEALTH_BYTE_LENGTH; i++)
    {
        battery_byte_array[i] = Battery_Health_Get() >> ((i-BATTERY_COULOMB_COUNT_BYTE_LENGTH) * BYTE_LENGTH);
    }
    for (i = BATTERY_COULOMB_COUNT_BYTE_LENGTH + BATTERY_HEALTH_BYTE_LENGTH; 
            i <BATTERY_COULOMB_COUNT_BYTE_LENGTH + BATTERY_HEALTH_BYTE_LENGTH + BATTERY_SOC_BYTE_LENGTH; 
            i++)
    {
        battery_byte_array[i] = Battery_SOC_Get() >> ((i-(BATTERY_COULOMB_COUNT_BYTE_LENGTH + BATTERY_HEALTH_BYTE_LENGTH)) * BYTE_LENGTH);
    }
            
    API_WriteNvram( BATTERY_EEPROM_PAGE_ADDRESS * PAGE_LENGTH
            , battery_byte_array
            , BATTERY_COULOMB_COUNT_BYTE_LENGTH   
                + BATTERY_HEALTH_BYTE_LENGTH              
                + BATTERY_SOC_BYTE_LENGTH );
}
