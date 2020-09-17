/**
 * Defines the test runner interface
 *
 * This module performs tests on the API
 *
 */

#ifndef _TEST_H_
#define _TEST_H_

//------------------------------//----------------------------------------------
// Definitions

//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Public functions

/**
 * Powerup initialization
 *
 * Call once at system start
 */
//void System_Initialize( void );

/**
 * Maintenance tasks loop
 *
 * Call periodically from main application loop
 */
void TEST_Tasks( void );

/**
 * Output the result of a test
 *
 * @param[in]  pName  the name of the test
 * @param[in]  bPass  true if the test passed
 */
void TEST_Complete( const char *pName, bool bPass );



void TEST_traction_Tasks( void );
void TEST_throttle_Tasks( void );
void TEST_sidebroom_Tasks( void );
void TEST_Vacuum_Tasks(void);
void TEST_Brush_motor_Tasks(void);
void TEST_traction_change_direction_Tasks(void);
void TEST_deck_movement_task(void);


#endif
