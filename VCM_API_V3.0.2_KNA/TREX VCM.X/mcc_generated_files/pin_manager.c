/**
  PIN MANAGER Generated Driver File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.c

  @Summary:
    This is the generated manager file for the PIC24 / dsPIC33 / PIC32MM MCUs device.  This manager
    configures the pins direction, initial state, analog setting.
    The peripheral pin select, PPS, configuration is also handled by this manager.

  @Description:
    This source file provides implementations for PIN MANAGER.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.95-b-SNAPSHOT
        Device            :  dsPIC33EP128MC506
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.36
        MPLAB 	          :  MPLAB X v5.10
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
    Section: Includes
*/

#include "pin_manager.h"
#include <stdio.h>
#include <xc.h>

/**
 Section: File specific functions
*/

/**
 Section: Driver Interface Function Definitions
*/
void PIN_MANAGER_Initialize( void )
{
    /****************************************************************************
     * Setting the Output Latch SFR(s)
     ***************************************************************************/
    LATA = 0x0000;
    LATB = 0x0980;
    LATC = 0x2000;
    LATD = 0x0100;
    LATE = 0x1000;
    LATF = 0x0001;
    LATG = 0x0280;

    /****************************************************************************
     * Setting the GPIO Direction SFR(s)
     ***************************************************************************/
    TRISA = 0x1F83;
    TRISB = 0x023F;
    TRISC = 0x1F78;    // make rc15 output revb hw was 0x9F78 in rev A
    TRISD = 0x0060;
    TRISE = 0x8000;
    TRISF = 0x0002;
    TRISG = 0x0300;

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
    CNPUG = 0x0200;

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

    RPOR2bits.RP38R = 0x000E;    //RB6->ECAN1:C1TX
    RPINR0bits.INT1R = 0x001B;    //RA11->EXT_INT:INT1
    RPINR19bits.U2RXR = 0x0029;    //RB9->UART2:U2RX
    RPINR1bits.INT2R = 0x0039;    //RC9->EXT_INT:INT2
    RPINR26bits.C1RXR = 0x0025;    //RB5->ECAN1:C1RX
    RPOR3bits.RP40R = 0x0003;    //RB8->UART2:U2TX

    // Don't let MCC delete these assignments!
    RPOR6bits.RP56R = 0x0009;    // SPI2:SCLK
    RPINR22bits.SDI2R = 0x0036;  // RC6->SPI2:SDI2
    RPOR5bits.RP55R = 0x0008;    // RC7->SPI2:SDO2
    //----------------------------------------

    __builtin_write_OSCCONL( OSCCON | 0x40 );  // lock PPS
}
