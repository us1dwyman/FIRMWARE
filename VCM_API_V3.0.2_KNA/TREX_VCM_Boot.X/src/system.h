/*
 * Defines system (hardware) management interface
 *
 *
 * Copyright (c) 2019 Control Solutions LLC. All Rights Reserved
 */

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "system_config.h"

//------------------------------//----------------------------------------------
// Definitions

//------------------------------//----------------------------------------------
// Public Functions
//
//

/**
 * @brief      Powerup initialization of hardware
 */
void SYSTEM_Initialize( void );

/**
 * @brief      Prepare for shutdown
 */
void SYSTEM_Deinitialize( void );

/**
 * @brief      Put system in low power mode
 */
void SYSTEM_Sleep( void );

#endif