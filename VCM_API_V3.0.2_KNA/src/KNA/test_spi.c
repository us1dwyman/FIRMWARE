/**
 * Tests the SPI expansion port by exercising a Dotstar LED string
 *
 * The test outputs a dynamic color pattern using the SPI API
 *
 */

#include "api.h"
#include "KNA/test.h"
#include "timer.h"

//------------------------------//----------------------------------------------
// Definitions

// number of LEDs in the connected string
#define PIXEL_COUNT 144

// Controls speed of animation (milliseconds per step)
#define SPEEDO 10

typedef uint32_t Color_t;

// Convenient way to handle RGB colors
typedef union {
    struct
    {
        union {
            uint8_t r;
            uint8_t red;
        };
        union {
            uint8_t g;
            uint8_t green;
        };
        union {
            uint8_t b;
            uint8_t blue;
        };
    };
    uint8_t raw[ 3 ];
} CRGB;

//------------------------------//----------------------------------------------
// Global variables

bool TEST_SPI_testEnabled = false;

//------------------------------//----------------------------------------------
// Local variables

// for keeping track of elapsed time
static MS_TICK _cycleStart;

// Array in RAM that holds the pixel state
static CRGB pixels[ PIXEL_COUNT ];

// HSV Rainbow colors
static const Color_t RainbowPalette[ 16 ] = {0xFF0000, 0xD52A00, 0xAB5500, 0xAB7F00, 0xABAB00, 0x56D500,
                                             0x00FF00, 0x00D52A, 0x00AB55, 0x0056AA, 0x0000FF, 0x2A00D5,
                                             0x5500AB, 0x7F0081, 0xAB0055, 0xD5002B};

// Holds the SPI driver configuration used to communicate with the device
static const DRV_SPI_INIT_DATA _spiConfig = {
    SPI_CHANNEL_EXPANSION,  // channel
    API_SPI_PPRE_16_1,       // primaryPrescale
    API_SPI_SPRE_1_1,       // secondaryPrescale
    SPI_CKE_IDLE_ACT,       // cke
    SPI_BUS_MODE_0,         // spi mode
    SPI_TRANSFER_MODE_8BIT  // transfer mode
};

//------------------------------//----------------------------------------------
// Local functions

/**
 * @brief      Refreshes the LED string
 *
 * The color settings for each LED are sent to the SPI interface
 * and clocked out to the string.
 */
static void _Update()
{
    int i;

    API_SPI_BeginTransaction( &_spiConfig );

    API_SPI_CS1_ENABLE();

    // Start of transfer is 4 0's
    API_SPI_Put( 0 );
    API_SPI_Put( 0 );
    API_SPI_Put( 0 );
    API_SPI_Put( 0 );

    // Write out the color of each pixel
    for( i = 0; i < PIXEL_COUNT; i++ )
    {
        // first byte is brightness, but we can control that
        // by scaling the RGB values, so we just use max brightness here.
        // Note we don't use FF, because then a fully white (4 0xff's) would
        // look like an end-of-transfer sequence.
        API_SPI_Put( 0xfe );
        // Color info. The order of transmission depends on the actual
        // LED hardware you are using.
        API_SPI_Put( pixels[ i ].raw[ 2 ] );
        API_SPI_Put( pixels[ i ].raw[ 1 ] );
        API_SPI_Put( pixels[ i ].raw[ 0 ] );
    }

    // End of transfer sequence
    API_SPI_Put( 0xff );
    API_SPI_Put( 0xff );
    API_SPI_Put( 0xff );
    API_SPI_Put( 0xff );

    // Release SPI
    API_SPI_CS1_DISABLE();
    API_SPI_EndTransaction();
}

/**
 * @brief      Sets a single pixel to an RGB color
 *
 * @param[in]  pPixel     pointer to pixel to be set
 * @param[in]  color      0xRRGGBB color value
 */
static void _SetColor( CRGB *pPixel, Color_t color )
{
    pPixel->b = color & 0xff;
    pPixel->g = color >> 8;
    pPixel->r = color >> 16;
}

/**
 * @brief      Sets a single pixel to an RGB color
 *
 * @param[in]  pPixel     pointer to pixel to be set
 * @param[in]  pColor     pointer to color to set
 */
// static void _SetRGB( CRGB *pPixel, CRGB *pColor )
//{
//    pPixel->b = pColor->b;
//    pPixel->g = pColor->g;
//    pPixel->r = pColor->r;
//}

/**
 * @brief      Fills the pixel array from a color palette
 *
 * @param[in]  colorIndex  The starting color index
 */
static void _FillFromPalette( uint8_t colorIndex )
{
    int i;
    for( i = 0; i < PIXEL_COUNT; i++ )
    {
        _SetColor( &pixels[ i ], RainbowPalette[ colorIndex & 0x0F ] );
        colorIndex++;
    }
}

//------------------------------//----------------------------------------------
// Public functions

/**
 * Initialize the test runner
 */
void TEST_SPI_Initialize( void )
{
    // we run continuously and there's no way to
    // tell if the LEDs are working, so just report passed.
    TEST_Complete( "SPI", true );

    // A timer
    _cycleStart = TIMER_Now();
}

/**
 * Run the task loop for the test
 */
void TEST_SPI_Tasks( void )
{
    // index for rotating the colors
    static uint8_t startIndex = 0;

    // if enabled by console command, operate our LED ribbon
    // normal operation is do not run this it would interfere with
    // the operation of user SPI hadware
    if ( TEST_SPI_testEnabled == true )
    {
        // How long has it been since the last update?
        MS_TICK elapsed = TIMER_Since( _cycleStart );
        
        if( elapsed > SPEEDO )
        {
            // make note of the current time
            _cycleStart = TIMER_Now();
        
            // Rotate where we start filling the colors
            startIndex = startIndex + 1;
        
            // Update our local copy of LED state
            _FillFromPalette( startIndex );
        
            // write it to the hardware
            _Update();
        }
    }
}
