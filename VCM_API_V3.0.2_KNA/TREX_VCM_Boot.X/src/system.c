/**
 * System (hardware) support functions
 *
 * @copyright Copyright (c) 2019 Control Solutions LLC.  All Rights Reserved
 * @private
 *
 */

// FICD
#pragma config ICS = PGD1    // ICD Communication Channel Select bits->Communicate on PGEC1 and PGED1
#pragma config JTAGEN = OFF  // JTAG Enable bit->JTAG is disabled

// FPOR
#pragma config ALTI2C1 = OFF   // Alternate I2C1 pins->I2C1 mapped to SDA1/SCL1 pins
#pragma config ALTI2C2 = OFF   // Alternate I2C2 pins->I2C2 mapped to SDA2/SCL2 pins
#pragma config WDTWIN = WIN25  // Watchdog Window Select bits->WDT Window is 25% of WDT period

// FWDT
#pragma config WDTPOST = PS256  // Watchdog Timer Postscaler bits->1:256
#pragma config WDTPRE = PR128   // Watchdog Timer Prescaler bit->1:128
#pragma config PLLKEN =                                                                                                \
    ON  // PLL Lock Enable bit->Clock switch to PLL source will wait until the PLL lock signal is valid.
#pragma config WINDIS = OFF  // Watchdog Timer Window Enable bit->Watchdog Timer in Non-Window mode
#pragma config FWDTEN = OFF  // Watchdog Timer Enable bit->Watchdog timer enabled/disabled by user software

// FOSC
#pragma config POSCMD = EC    //Primary Oscillator Mode Select bits->EC (External Clock) Mode
#pragma config OSCIOFNC = ON  // OSC2 Pin Function bit->OSC2 is general purpose digital I/O pin
#pragma config IOL1WAY = OFF  // Peripheral pin select configuration->Allow muliple reconfiguration
#pragma config FCKSM =                                                                                                 \
    CSECMD  // Clock Switching Mode bits->Clock switching is enabled,Fail-safe Clock Monitor is disabled

// FOSCSEL
#pragma config FNOSC = FRC   // Oscillator Source Selection->FRC
#pragma config PWMLOCK = ON  // PWM Lock Enable bit->Certain PWM registers may only be written after key sequence
#pragma config IESO = ON     // Two-speed Oscillator Start-up Enable bit->Start up device with FRC, then switch to
                             // user-selected oscillator source

// FGS
#pragma config GWRP = OFF  // General Segment Write-Protect bit->General Segment may be written
#pragma config GCP = OFF   // General Segment Code-Protect bit->General Segment Code protect is Disabled

#include "system.h"
#include "can.h"
#include "dma.h"

//------------------------------//----------------------------------------------
// Definitions

//------------------------------//----------------------------------------------
// Global variables

//------------------------------//----------------------------------------------
// Local variables

//------------------------------//----------------------------------------------
// Local functions

//------------------------------//----------------------------------------------
// Public functions

void SYSTEM_Initialize( void )
{
    // no interrupts in bootloader; our IVT is used by the main app
    __builtin_disable_interrupts();

    // Why we were reset
    RCON = 0;

    //------------------------------//----------------------------------------------
    // I/O pins

    /****************************************************************************
     * Setting the Output Latch SFR(s)
     ***************************************************************************/
    LATA = 0x0000;
    LATB = 0x1980;
    LATC = 0x2000;
    LATD = 0x0000;
    LATE = 0x1000;
    LATF = 0x0000;
    LATG = 0x0000;    // RG6 def to 0 now micro actuator REV signal

    /****************************************************************************
     * Setting the GPIO Direction SFR(s)
     ***************************************************************************/
    TRISA = 0x1F83;
    TRISB = 0xE73F;
    TRISC = 0x1F78;    // make rc15 output revb hw was 0x9F78 in rev A
    TRISD = 0x0060;    // rd5 now input 
    TRISE = 0x8000;
    TRISF = 0x0002;
    TRISG = 0x0300;    // rg7 now output

    /****************************************************************************
     * Setting the Weak Pull Up and Weak Pull Down SFR(s)
     ***************************************************************************/
    CNPDA = 0x0000;
    CNPDB = 0x0000;
    CNPDC = 0x0000;
    CNPDD = 0x0000;
    CNPDE = 0x0000;
    CNPDF = 0x0000;
    CNPDG = 0x0000;
    CNPUA = 0x0000;
    CNPUB = 0x0000;
    CNPUC = 0x0800;
    CNPUD = 0x0000;
    CNPUE = 0x0000;
    CNPUF = 0x0000;
    CNPUG = 0x0000;

    /****************************************************************************
     * Setting the Open Drain SFR(s)
     ***************************************************************************/
    ODCA = 0x0000;
    ODCB = 0x0000;
    ODCC = 0x0000;
    ODCD = 0x0000;
    ODCE = 0x0000;
    ODCF = 0x0000;
    ODCG = 0x0000;

    /****************************************************************************
     * Setting the Analog/Digital Configuration SFR(s)
     ***************************************************************************/
    ANSELA = 0x1013;
    ANSELB = 0x0003;
    ANSELC = 0x0800;
    ANSELE = 0x8000;

    /****************************************************************************
     * Set the PPS
     ***************************************************************************/
    __builtin_write_OSCCONL( OSCCON & 0xbf );  // unlock PPS

    RPINR26bits.C1RXR = 0x0025;  // RB5->ECAN1:C1RX
//    Allow GPIO control of RG6,7 no longer dedicated to UART
//    RPINR18bits.U1RXR = 0x0077;  // RG7->UART1:U1RX
//    RPOR8bits.RP118R = 0x0001;   // RG6->UART1:U1TX
    RPOR2bits.RP38R = 0x000E;    // RB6->ECAN1:C1TX

    __builtin_write_OSCCONL( OSCCON | 0x40 );  // lock PPS

    //------------------------------//----------------------------------------------
    // System clock

    // FRCDIV FRC/2; PLLPRE 2; DOZE 1:8; PLLPOST 1:4; DOZEN disabled; ROI disabled; 
    CLKDIV = 0x3140;
    // TUN Center frequency;
    OSCTUN = 0x00;
    // ROON disabled; ROSEL FOSC; RODIV 0; ROSSLP disabled;
    REFOCON = 0x00;
    // PLLDIV 254; 
    PLLFBD = 0xFE;
    // AD1MD enabled; PWMMD enabled; T3MD enabled; T4MD enabled; T1MD enabled; U2MD enabled; T2MD enabled; U1MD enabled;
    // QEI1MD enabled; SPI2MD enabled; SPI1MD enabled; C1MD enabled; T5MD enabled; I2C1MD enabled;
    PMD1 = 0x00;
    // IC4MD enabled; IC3MD enabled; OC1MD enabled; IC2MD enabled; OC2MD enabled; IC1MD enabled; OC3MD enabled; OC4MD
    // enabled;
    PMD2 = 0x00;
    // CMPMD enabled; CRCMD enabled; I2C2MD enabled;
    PMD3 = 0x00;
    // CTMUMD enabled; REFOMD enabled;
    PMD4 = 0x00;
    // PWM2MD enabled; PWM1MD enabled; PWM3MD enabled;
    PMD6 = 0x00;
    // PTGMD enabled; DMA0MD enabled;
    PMD7 = 0x00;
    // CF no clock failure; NOSC PRIPLL; CLKLOCK unlocked; OSWEN Switch is Complete;
    __builtin_write_OSCCONH( ( uint8_t )( 0x03 ) );
    __builtin_write_OSCCONL( ( uint8_t )( 0x01 ) );
    // Wait for Clock switch to occur
    while( OSCCONbits.OSWEN != 0 )
        ;
    while( OSCCONbits.LOCK != 1 )
        ;

    //------------------------------//----------------------------------------------
    // Watchdog
    RCONbits.SWDTEN = 1;

    //------------------------------//----------------------------------------------
    // Timer 1 - for event timing

    TMR1 = 0x00;

    PR1 = TIMER_PRELOAD;
    // TCKPS 1:256; TON enabled; TSIDL disabled; TCS FOSC/2; TSYNC disabled; TGATE disabled;
    T1CON = 0x8030;

    //------------------------------//----------------------------------------------
    // Timer 2/3 used for determining when to launch the main application if it's valid

    // TMR3 0;
    TMR3 = 0x00;
    // PR3 4;
    PR3 = 0x04;
    // TMR2 0;
    TMR2 = 0x00;
    // Period = 2 s; Frequency = 40000000 Hz; PR2 50356;
    PR2 = 0xC4B4;
    // TCKPS 1:256; T32 32 Bit; TON enabled; TSIDL disabled; TCS FOSC/2; TGATE disabled;
    T2CON = 0x8038;

    //------------------------------//----------------------------------------------

    DMA_Initialize();

    //------------------------------//----------------------------------------------
    // CPU core control registers
    CORCON = ( CORCON & 0x00F2 ) | 0x0020;
}

/**
 * @brief      Prepare for shutdown
 */
void SYSTEM_Deinitialize( void )
{
    //RCONbits.SWDTEN = 0;

    // restore timers to power-on defaults
    T1CON = T2CON = T3CON = 0;
    
    // Not sure of the effects of leaving CAN enabled while the processor
    // boots the application. Seems safer to turn it off
    CAN_Sleep();
    
}

/**
 * @brief      Put system in low power mode
 */
void SYSTEM_Sleep( void )
{
    //    CAN_Sleep();
}
