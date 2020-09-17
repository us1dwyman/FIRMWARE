/**
 * Analog to Digital Converter driver for PIC24F using ADC1 and DMA0
 *
 * This driver is configured to sequentially sample a number of analog
 * input pins.  The ADC1 module is used with sequential sampling over multiple
 * channels.
 * Samples are buffered using DMA in scatter/gather mode with continuous
 * ping-pong.
 *
 * References:
 *  http://esca.atomki.hu/PIC24/code_examples/docs/manuallyCreated/Appendix_H_ADC_with_DMA.pdf
 *  http://ww1.microchip.com/downloads/en/DeviceDoc/70225B.pdf
 *  http://ww1.microchip.com/downloads/en/DeviceDoc/39742A.pdf
 *
 * Copyright (c) 2018 Control Solutions LLC. All Rights Reserved
 */

#include "adc.h"
#include "app.h"
//#include "mcc_generated_files/adc1.h"
#include <assert.h>
#include <stdint.h>

static void _ADC_ChannelEnable( ADC_CHANNEL channel );

#include "app.h"
#include <assert.h>
#include <stdint.h>

#include "adc.h"

//------------------------------//----------------------------------------------
// Definitions

// Decide on a value needed by the DMA initialization
#if( ADC_SAMPLES_PER_CHANNEL == 2 )
#define DMABL_VALUE 1
#elif( ADC_SAMPLES_PER_CHANNEL == 4 )
#define DMABL_VALUE 2
#elif( ADC_SAMPLES_PER_CHANNEL == 8 )
#define DMABL_VALUE 3
#elif( ADC_SAMPLES_PER_CHANNEL == 16 )
#define DMABL_VALUE 4
#else
#error I think the samples per channel has to be a power of 2
#endif

//------------------------------//----------------------------------------------
// Global variables

// max speed pot adc value
uint16_t ADC_TmpMaxSpeedGlobal = ADC_VAL_MAX;

// Our latest reading from each channel
uint16_t ADC_value[ ADC_MAX_CHANNELS ];

// Flag that lets caller know we have updated the ADC_values.  We only set
// this flag; the caller can clear it as desired.
volatile bool ADC_hasNewSample;

//------------------------------//----------------------------------------------
// Local variables

// Keeps track of which DMA buffer set is being used (for ping-pong mode)
static unsigned int _dmaBuffer = 0;

// Lower and upper word of the channel select register. This basically keeps
// track of
// which ADC channels are enabled.
static uint16_t _csslMap = 0x0000;
static uint16_t _csshMap = 0x0000;

// The ADC hardware fills these buffers with samples
ADC_SAMPLE BufferA[ ADC_MAX_CHANNELS ][ ADC_SAMPLES_PER_CHANNEL ]
    __attribute__( ( aligned( ADC_MAX_CHANNELS * ADC_SAMPLES_PER_CHANNEL * 2 ) ) );
ADC_SAMPLE BufferB[ ADC_MAX_CHANNELS ][ ADC_SAMPLES_PER_CHANNEL ]
    __attribute__( ( aligned( ADC_MAX_CHANNELS * ADC_SAMPLES_PER_CHANNEL * 2 ) ) );

//------------------------------//----------------------------------------------
// Local Functions

/**
 * Called on ADC ISR to evaluate a buffer of samples for a channel
 *
 * We do some basic filtering to produce an average that is stored
 * and available for global use
 */
static ADC_SAMPLE _ChannelAverage( ADC_SAMPLE *pBuf )
{
    ADC_SAMPLE max = 0;
    ADC_SAMPLE min = 0x3FF;
    uint8_t i = ADC_SAMPLES_PER_CHANNEL;
    ADC_SAMPLE sum = 0;

    while( i > 0 )
    {
        if( *pBuf > max )
            max = *pBuf;
        if( *pBuf < min )
            min = *pBuf;
        sum += *pBuf;
        pBuf++;
        i--;
    }

    return ( sum - max - min ) / ( ADC_SAMPLES_PER_CHANNEL - 2 );
}

/**
 * Interrupt service routine for DMA.  Called when one of the DMA buffers
 * is full.
 */
void __attribute__( ( __interrupt__, auto_psv ) ) _DMA0Interrupt( void )
{
    // this is for debugging purposes to validate the sampling period using
    // Debug LED 2 on the MCLV-2 dev kit.
    // measured 80mS between ints = 10mS per sample
    // PORTD = PORTD ^ 0x0040;  // toggle Debug LED 2 every time out of sleep

    // Point to the correct buffer
    ADC_SAMPLE( *pBuf )[ ADC_SAMPLES_PER_CHANNEL ] = ( _dmaBuffer ) ? BufferB : BufferA;

    // Iterate through all enabled channels and calculate the average
    uint16_t mask = 0x0001;
    unsigned int i;

    for( i = 0; i < 16; i++ )
    {
        if( _csslMap & mask )
        {
            // channel is enabled
            ADC_value[ i ] = _ChannelAverage( pBuf[ i ] );
        }

        if( _csshMap & mask )
        {
            // channel is enabled
            ADC_value[ i + 16 ] = _ChannelAverage( pBuf[ i + 16 ] );
        }
        mask <<= 1;
    }

    _dmaBuffer ^= 1;

    ADC_hasNewSample = true;

    IFS0bits.DMA0IF = 0;  // Clear the DMA0 Interrupt Flag
}

/**
 * Calculates the number of enabled channels
 *
 */
static unsigned int _NumberOfEnabledChannels( void )
{
    uint16_t mask = 0x0001;
    unsigned int i;
    unsigned int numChannels = 0;

    for( i = 0; i < 16; i++ )
    {
        if( _csslMap & mask )
        {
            numChannels++;
        }
        if( _csshMap & mask )
        {
            numChannels++;
        }
        mask <<= 1;
    }

    return numChannels;
}

/**
 * Set up the DMA
 */
static void _InitDma0( void )
{

    DMA0CONbits.AMODE = 2;  // Configure DMA for Peripheral indirect mode
    DMA0CONbits.MODE = 2;   // Configure DMA for Continuous Ping-Pong mode
    DMA0PAD = 0x0300;       // Point DMA to ADC1BUF0 (see Table 8-1 DS70000657H)

    // Number of words to accept before generating the DMA interrupt.
    // i.e. a full set of samples for each enabled channel
    DMA0CNT = ( _NumberOfEnabledChannels() * ADC_SAMPLES_PER_CHANNEL ) - 1;

    DMA0REQ = 13;  // Select ADC1 (only ADC channel on dsPIC33)  (see Table 8-1 DS70000657H)

    // set up the buffer regs (Figure 22-7 70348c.pdf)
    DMA0STAH = 0x0000;
    DMA0STAL = (volatile unsigned int)&BufferA;
    DMA0STBH = 0x0000;
    DMA0STBL = (volatile unsigned int)&BufferB;

    IFS0bits.DMA0IF = 0;   // Clear the DMA interrupt flag bit
    IEC0bits.DMA0IE = 1;   // Set the DMA interrupt enable bit
    DMA0CONbits.CHEN = 1;  // Enable DMA
}

/**
 * Set or update the driver configuration
 */
void _SetConfiguration( void )
{
    AD1CON1bits.ADON = 0;
    IEC0bits.DMA0IE = 0;   // Enable the DMA interrupt enable bit
    DMA0CONbits.CHEN = 0;  // Disable DMA

    AD1CON1bits.FORM = 0b00;  // result as unsigned integer
    AD1CON1bits.SSRC = 2;     // Use GP Timer (3) for timing
    AD1CON1bits.ASAM = 1;     // ADC Sample Control: Sampling begins
                              // immediately after conversion
    AD1CON1bits.AD12B = 0;    // 10-bit ADC operation
    AD1CON1bits.SIMSAM = 0;   // Sample channels in sequence

    // Always starts filling the buffer from the start address
    AD1CON2bits.BUFM = 0;

    // Scan Input Selections for CH0+ during Sample A bit
    AD1CON2bits.CSCNA = 1;
    AD1CON2bits.CHPS = 0;   // Converts CH0
    AD1CON3bits.ADRC = 0;   // ADC Clock is derived from Systems Clock
    AD1CON3bits.ADCS = 63;  // ADC Conversion Clock, TAD = TCY+(ADCS+1)

    // AD1CHS0: A/D Input Select Register
    AD1CHS0bits.CH0SA = 13;  // MUXA +ve input selection (AN13) for CH0
    AD1CHS0bits.CH0NA = 0;   // MUXA -ve input selection (Vref-) for CH0

    // AD1CHS123: A/D Input Select Register
    AD1CHS123bits.CH123SA = 0;  // MUXA +ve input selection (AIN0) for CH1
    AD1CHS123bits.CH123NA = 0;  // MUXA -ve input selection (Vref-) for CH1

    // AD1CSSH/AD1CSSL: A/D Input Scan Selection Register
    // determine which channels are enabled
    AD1CSSH = _csshMap;
    AD1CSSL = _csslMap;

    // DMA scatter/gather mode.  See also DMA init code
    AD1CON1bits.ADDMABM = 0;

    // One less than the number of channels we are scanning.
    AD1CON2bits.SMPI = ( _NumberOfEnabledChannels() - 1 );

    // DMA buffer size (determined by ADC_SAMPLES_PER_CHANNEL)
    AD1CON4bits.DMABL = DMABL_VALUE;

    // Enable DMA for ADC1
    AD1CON4bits.ADDMAEN = 1;

    // Clear the A/D interrupt flag bit
    IFS0bits.AD1IF = 0;

    // Do Not Enable A/D interrupt, we are using DMA
    IEC0bits.AD1IE = 0;

    // Turn on the A/D converter
    AD1CON1bits.ADON = 1;

    _InitDma0();
}

/**
 * Enable the specified A/D channel
 *
 * Sets the pin as an input, and an analog input at that.
 *
 */
static void _ADC_ChannelEnable( ADC_CHANNEL channel )
{
    TRACE( ( "Enabling ADC Channel %u\n", channel ) );

    // Enable the channel in the ADC scan list
    if( channel > 15 )
    {
        uint16_t mask = ( 1 << ( channel - 16 ) );
        _csshMap |= mask;
    }
    else
    {
        uint16_t mask = ( 1 << channel );
        _csslMap |= mask;
    }

    _SetConfiguration();
}

//------------------------------//----------------------------------------------
// Public Functions

/**
 * Initialize all A/D channels
 *
 * Enable all desired channels here.
 *
 */
void ADC_Initialize( void )
{
    _ADC_ChannelEnable( ADC_CHANNEL_ANIN0 );
    _ADC_ChannelEnable( ADC_CHANNEL_ANIN1 );
    _ADC_ChannelEnable( ADC_CHANNEL_THROTTLE );
    _ADC_ChannelEnable( ADC_CHANNEL_BATTLEVEL );
    _ADC_ChannelEnable( ADC_CHANNEL_VCONTROL_LEVEL );
    _ADC_ChannelEnable( ADC_CHANNEL_ACTUATOR_CURRENT );
    _ADC_ChannelEnable( ADC_CHANNEL_BRK_SENSE );
}

/**
 * Return the collected average reading for the specified A/D channel
 *
 */
ADC_SAMPLE ADC_ChannelAverage( ADC_CHANNEL channel )
{
    // TRACE( ( "Returning Average for  ADC Channel %u\n", channel ) );

    return ADC_value[ channel ];
}
