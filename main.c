/*
 * File:   main.c
 * Author: Luca
 *
 * Created on 29 dicembre 2017, 23.19
 */
#include <xc.h>
#include <stdint.h>
#include "Display.h" 
#include "RTCC.h"

#define _XTAL_FREQ  8000000

// PIC18F26J11 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1L
#pragma config WDTEN = OFF      // Watchdog Timer (Disabled - Controlled by SWDTEN bit)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset (Enabled)
#pragma config XINST = OFF       // Extended Instruction Set (Enabled)

// CONFIG1H
#pragma config CP0 = OFF        // Code Protect (Program memory is not code-protected)

// CONFIG2L
#pragma config OSC = INTOSC     // Oscillator (INTOSC)
#pragma config T1DIG = OFF      // T1OSCEN Enforcement (Secondary Oscillator clock source may not be selected)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator (High-power operation)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor (Disabled)
#pragma config IESO = OFF        // Internal External Oscillator Switch Over Mode (Enabled)

// CONFIG2H
#pragma config WDTPS = 32768    // Watchdog Postscaler (1:32768)

// CONFIG3L
#pragma config DSWDTOSC = INTOSCREF// DSWDT Clock Select (DSWDT uses INTRC)
#pragma config RTCOSC = T1OSCREF// RTCC Clock Select (RTCC uses INTRC)
#pragma config DSBOREN = ON     // Deep Sleep BOR (Enabled)
#pragma config DSWDTEN = ON     // Deep Sleep Watchdog Timer (Enabled)
#pragma config DSWDTPS = G2     // Deep Sleep Watchdog Postscaler (1:2,147,483,648 (25.7 days))

// CONFIG3H
#pragma config IOL1WAY = ON     // IOLOCK One-Way Set Enable bit (The IOLOCK bit (PPSCON<0>) can be set once)
#pragma config MSSP7B_EN = MSK7 // MSSP address masking (7 Bit address masking mode)

// CONFIG4L
#pragma config WPFP = PAGE_63   // Write/Erase Protect Page Start/End Location (Write Protect Program Flash Page 63)
#pragma config WPEND = PAGE_WPFP// Write/Erase Protect Region Select (valid when WPDIS = 0) (Page WPFP<5:0> through Configuration Words erase/write protected)
#pragma config WPCFG = OFF      // Write/Erase Protect Configuration Region (Configuration Words page not erase/write-protected)

// CONFIG4H
#pragma config WPDIS = OFF      // Write Protect Disable bit (WPFP<5:0>/WPEND region ignored)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


datetime dt; 
const datetime ctordt; 
void SetPwm (uint8_t duty);






void SetupSystem (void)
{
    
}

void SetupPWM (void)
{
    PPSCONbits.IOLOCK = 0;
    RPOR15 = 14;
    RPOR16 = 15;
    RPOR17 = 16;
    RPOR18 = 17;
    PPSCONbits.IOLOCK = 1;
    PR2 = 0x0A;
    T2CON = 0b00000100;
    CCP1CON = 0b00001100;    
    PSTR1CON = 0b00000000;
}

void SetPwm (uint8_t duty)
{
          if (duty & 0x01)
            CCP1CONbits.DC1B0 = 1;
        else
            CCP1CONbits.DC1B0 = 0;
        if (duty & 0x02)
            CCP1CONbits.DC1B1 = 1;
        else
            CCP1CONbits.DC1B1 = 0;    
        
        CCPR1L = duty>>2; 
}

void SetupIOPins (void)
{
    TRISC = 0b00001111;
    TRISB = 0b00000000;
}

void DisplayController (uint8_t digits)
{
    if (digits & 0x01)
        PSTR1CONbits.STRA = 1;
    else 
        PSTR1CONbits.STRA = 0;
    if (digits & 0x02)
        PSTR1CONbits.STRB = 1;
    else 
        PSTR1CONbits.STRB = 0;
    if (digits & 0x04)
        PSTR1CONbits.STRC = 1;
    else 
        PSTR1CONbits.STRC = 0;
    if (digits & 0x08)
        PSTR1CONbits.STRD = 1;
    else 
        PSTR1CONbits.STRD = 0;
  
}


void main(void) {
    SetupPWM();
    SetupIOPins();
    SetupOutputPort(&PORTB);
    SetupDigitController(DisplayController);
    SetupRTCC();
    PORTC = 0;
    CCPR1L= 0x0F;
    int bright = 10;
    int digit = 0;
    int brightnessdelay = 0;
    int displaycounter = 0;
    
    while(1)
    {
        

        if (RTCCFGbits.HALFSEC)
        //    PORTBbits.RB0 = 1;
            DPOn(1);
        else
        //     PORTBbits.RB0 = 0;
            DPOff(1);
        DisplayRefresh();
        
        if (brightnessdelay >60)
        {
        if (bright > 0x2B)
            bright = 1;
        if (bright & 0x01)
            CCP1CONbits.DC1B0 = 1;
        else
            CCP1CONbits.DC1B0 = 0;
        if (bright & 0x02)
            CCP1CONbits.DC1B1 = 1;
        else
            CCP1CONbits.DC1B1 = 0;    
        
        SetPwm(2);
        
        bright += 5;
        brightnessdelay = 0;
        //DisplayUnsignedInt(displaycounter);
        //SetCouples(displaycounter, 100-displaycounter);
        GetDateTime(&dt);
        SetCouples(dt.hour, dt.minute);
        displaycounter++;
        if (displaycounter > 99)
            displaycounter = 0;
        }
        brightnessdelay ++;
         __delay_ms(3);           
    };
    return;
}


