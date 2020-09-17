/**
 * Hardware initialization
 *
 * Contains initialization and management routines for the processor hardware
 *
 * Copyright (c) 2018 Control Solutions LLC.  All Rights Reserved.
 */

// CONFIG1
#pragma config FEXTOSC = OFF     // External Oscillator mode selection bits (Oscillator not enabled)
#pragma config RSTOSC = HFINT32  // Power-up default value for COSC bits (HFINTOSC with OSCFRQ= 32 MHz and CDIV = 1:1)
#pragma config CLKOUTEN = OFF  // Clock Out Enable bit (CLKOUT function is disabled; i/o or oscillator function on OSC2)
#pragma config CSWEN = ON      // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = OFF     // Fail-Safe Clock Monitor Enable bit (FSCM timer disabled)

// CONFIG2
#pragma config MCLRE = ON     // Master Clear Enable bit (MCLR pin is Master Clear function)
#pragma config PWRTE = OFF    // Power-up Timer Enable bit (PWRT disabled)
#pragma config LPBOREN = OFF  // Low-Power BOR enable bit (ULPBOR disabled)
#pragma config BOREN = ON     // Brown-out reset enable bits (Brown-out Reset Enabled, SBOREN bit is ignored)
#pragma config BORV = LO      // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (VBOR) set to 1.9V on LF,
                              // and 2.45V on F Devices)
#pragma config ZCD = OFF      // Zero-cross detect disable (Zero-cross detect circuit is disabled at POR.)
#pragma config PPS1WAY =                                                                                               \
    OFF  // Peripheral Pin Select one-way control (The PPSLOCK bit can be set and cleared repeatedly by software)
#pragma config STVREN =                                                                                                \
    ON  // Stack Overflow/Underflow Reset Enable bit (Stack Overflow or Underflow will cause a reset)

// CONFIG3
//#pragma config WDTCPS = WDTCPS_31  // WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
//#pragma config WDTE = OFF          // WDT operating mode (WDT Disabled, SWDTEN is ignored)
//#pragma config WDTCWS =                                                                                                \
//    WDTCWS_7  // WDT Window Select bits (window always open (100%); software control; keyed access not required)
//#pragma config WDTCCS = SC  // WDT input clock selector (Software Control)


// CONFIG3
#pragma config WDTCPS = WDTCPS_5    // WDT Period Select bits->Divider ratio 1:1024  TIMEOUT = 33 ms                                  XXXX
#pragma config WDTE = NSLEEP    // WDT operating mode->WDT enabled while sleep=0, suspended when sleep=1; SWDTEN ignored              XXXX
#pragma config WDTCWS = WDTCWS_7    // WDT Window Select bits->window always open (100%); software control; keyed access not required XXXX
#pragma config WDTCCS = LFINTOSC    // WDT input clock selector->WDT reference clock is the 31.0kHz LFINTOSC output                   XXXX




// CONFIG4
#pragma config BBSIZE = BB512  // Boot Block Size Selection bits (512 words boot block size)
#pragma config BBEN = OFF      // Boot Block Enable bit (Boot Block disabled)
#pragma config SAFEN = OFF     // SAF Enable bit (SAF disabled)
#pragma config WRTAPP = OFF    // Application Block Write Protection bit (Application Block not write protected)
#pragma config WRTB = OFF      // Boot Block Write Protection bit (Boot Block not write protected)
#pragma config WRTC = OFF    // Configuration Register Write Protection bit (Configuration Register not write protected)
#pragma config WRTSAF = OFF  // Storage Area Flash Write Protection bit (SAF not write protected)
#pragma config LVP =                                                                                                   \
    ON  // Low Voltage Programming Enable bit (Low Voltage programming enabled. MCLR/Vpp pin function is MCLR.)

// CONFIG5
#pragma config CP = OFF  // UserNVM Program memory code protection bit (UserNVM code protection disabled)

//#include "hmi.h"
#include "can.h"
#include "mcp2515_can.h"
#include "system_config.h"
#include <stdbool.h>
#include <stdint.h>

//------------------------------//----------------------------------------------
// Definitions

//------------------------------//----------------------------------------------
// Global variables

// keeps track of timer ticks, for the purpose of measuring time durations
TIMER_TICK SYSTEM_tick;

//------------------------------//----------------------------------------------
// Local variables

//------------------------------//----------------------------------------------
// Local functions

//------------------------------//----------------------------------------------
// Public functions

/**
 * Powerup Initialization
 */
void SYSTEM_Initialize( void )
{

    // DISABLE INTERUPTS
    INTCONbits.GIE = 0;
    INTCONbits.PEIE = 0;

    //------------------------------//----------------------------------------------
    // IO port setup

    // PORT A: LED row outputs & other
    // A7 = 3.3V enable
    // a6 = horn
    LATA = _LATA_LATA7_MASK;
    ANSELA = 0;
    TRISA = 0;

    // PORT B
    // B0: FAULT (input+pullup or output),
    // CAN chip interface
    ANSELB = 0;
    LATB = 0B00000001;
    TRISB = 0B00000100;
    WPUB = _WPUB_WPUB0_MASK;

    // PORT C: matrix switch column inputs plus
    // RC1: CAN_INT
    // RC0: ENABLE (pulled up))
    // RC6: TX1
    // RC7: RX1
    ANSELC = 0;
    LATC = 0;
    TRISC = 0B10111111;
    WPUC = _WPUC_WPUC0_MASK;

    // PORT D: switch row outputs, extra switch inputs
    ANSELD = 0;
    LATD = 0;
    TRISD = 0B11100000;

    // PORTE: LED 25,26,27 off to start
    ANSELE = 0;
    LATE = 0b00000111;
    TRISE = 0;

    // PORT F: LED column outputs
    ANSELF = 0;
    LATF = 0;
    TRISF = 0;

    //------------------------------//----------------------------------------------
    // peripheral pin routing
    
    // new modified hardware
         
    RX1DTPPS = 22;   //RC6->EUSART1:RX1;
    RC7PPS   = 0x0F;   //RC7->EUSART1:TX1;

//    RX1DTPPS = 0X17;  // RC7 IS UART1 RX
//    RC6PPS = 0X0F;    // RC6 IS UART1 TX

    SSP1DATPPS = 0XA;  // SPI1 MISO RB2
    RB1PPS = 0X15;     // SCK RB1
    RB3PPS = 0X16;     // SPI1 MOSI RB3

    PMD2 = 0B01100111;  //"" DAC, ADC, CMP1, CMP2, ZC
    PMD3 = 0B00111111;  //"" PWM3-6, CCP1-2
    PMD4 = 0B10100001;  //"" UART2,SPI2,CWG1
    PMD5 = 0B00111110;  //"" CLC1-4
    
    //------------------------------//----------------------------------------------
    // TIMER: 1ms interrupt given FOSC=32MHz

    // T0CS FOSC/4; T0CKPS 1:8; T0ASYNC synchronised;
    // T0OUTPS 1:8; T0EN enabled; T016BIT 8-bit;
    // Interrupt enabled
    T0CON1 = 0x43;
    T0CON0 = 0x87;
    TMR0H = 0x7C;
    TMR0L = 0x00;

    //------------------------------//----------------------------------------------
}

/**
 * @brief      Hardware to low-power mode
 *
 * This function does not exit until a valid pin-change interrupt is detected
 */
void SYSTEM_Sleep( void )
{
    // disable peripheral interrupts
    INTCONbits.PEIE = 0;

    // interrupt only on negative edge of RC0 (ENABLE))
    IOCCN = _IOCCN_IOCCN0_MASK;
    PIR0bits.IOCIF = 0;
    PIE0bits.IOCIE = 1;

    // kill 3.3v supply to LCD module
    OPIN_3V_ENABLE = 0;

    // IDLE, not DOZE
    CPUDOZEbits.IDLEN = 1;

//    asm( "SLEEP" );

    CAN_Sleep();
    SLEEP();
}

/**
 * @brief      Undo the low-power mode
 *
 */
void SYSTEM_Wakeup( void )
{
    // restore supply to LCD module
    OPIN_3V_ENABLE = 1;
    CAN_Wakeup();

    // enable timer interrupt
    PIR0bits.TMR0IF = 0;
    PIE0bits.TMR0IE = 1;

    // interrupt on negative edge of RC1 (CAN_INT)) or RC0 (ENABLE))
    IOCCN = _IOCCN_IOCCN0_MASK | _IOCCN_IOCCN1_MASK;

    PIR0bits.IOCIF = 0;
    PIE0bits.IOCIE = 1;

    // enable interrupts
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
}


