/**
 * Multi-channel ADC sampling driver for PIC24
 *
 * This driver continuously samples the enabled ADC channels, and averages the
 * results. It relies on the following configuration in app.h:
 *      ADC_MAX_CHANNELS: must be a power of two, and you can only enable & use
 *          AN inputs between AN0 and AN(ADC_MAX_CHANNELS-1), inclusive.
 *          This driver uses a DMA buffer that is
 *          sized to the ADC_MAX_CHANNELS*ADC_SAMPLES_PER_CHANNEL.
 *      ADC_SAMPLES_PER_CHANNEL: The number of samples collected for all the
 *          enabled channels before the averages are updated.  Decreasing this
 *          reduces the memory footprint and increases the update rate;
 *          however the results may be less stable
 *          since the averages are taken over a shorter window.
 *          The samples per channel value must not be large enough that adding
 *          10- or 12-bit samples together exceeds a 16-bit integer, since
 *          that is the type used for the accumulator.  So this places a
 *          practical limit of 16 samples per channel before you have to
 *          change the averager code to handle larger numbers.
 *
 *  Sampling rate for the A/D converter is determined by the TMR2 clock.  For
 *  example, if the TMR2 clock period is 1ms, there are four channels enabled,
 *  and 8 samples per channel, the averages will be updated every 1*4*8= 32ms.
 *
 *  The TMR2 clock must be appropriately initialized outside this driver.
 *  Also, the ANx pins you want to use must be configured as inputs (which is
 *  probably the power on default), but this driver does not
 *  update the TRIS setting of the pins.
 *
 *  To Enable a channel (eg AN1), call ADC_ChannelEnable(1).
 *
 *
 * Copyright (c) 2018 Control Solutions LLC.  All Rights Reserved.
 */

#ifndef ADC_H
#define ADC_H

#include "api.h"

#include <stdint.h>

//------------------------------//----------------------------------------------
// Definitions

typedef uint16_t ADC_SAMPLE;
typedef uint8_t ADC_CHANNEL;

#define ADC_VAL_MAX 0x3FF

#if( ADC_SAMPLES_PER_CHANNEL > 16 )
#error ADC_SAMPLES_PER_CHANNEL may be too large to allow 16-bit averaging
#endif

//------------------------------//----------------------------------------------
// Global variables

// max speed pot adc value
extern uint16_t ADC_TmpMaxSpeedGlobal;

// A flag that indicates when we update the ADC_value(s).  You can reset
// this after you process the updated values.
extern volatile bool ADC_hasNewSample;

// The most recent sample from each of the ADC_CHANNELs.  Use the channel
// number as the index into this array (eg AN6 value is found in ADC_value[6] )
extern uint16_t ADC_value[];

//------------------------------//----------------------------------------------
// Public Functions

/**
 * Enables all ADC channels
 *
 * Enabling at least one channel turns on the ADC module and starts
 * sampling
 *
 */
void ADC_Initialize( void );

/**
 * Return the collected average reading for the specified A/D channel
 *
 * @param channel - the channel index to return
 */
ADC_SAMPLE ADC_ChannelAverage( ADC_CHANNEL channel );

#endif
