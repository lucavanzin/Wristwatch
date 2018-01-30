/*
 * File:   Display.c
 * Author: Luca
 * 
 * Created on 28 dicembre 2017, 17.37
 */
#include <stdlib.h>

#include "Display.h"

#define BRIGHT_MIN 10   //Minimun brightness
#define BRIGHT_MAX 255  //Maximum brightness

void (*DigitController)(uint8_t state);

void (*BrightDigit1)(uint8_t bright);
void (*BrightDigit2)(uint8_t bright);
void (*BrightDigit3)(uint8_t bright);
void (*BrightDigit4)(uint8_t bright);

uint8_t GetDigit1 (void);
uint8_t GetDigit2 (void);
uint8_t GetDigit3 (void);
uint8_t GetDigit4 (void);
//void (*DigitController)(uint8_t digit, bit state, uint8_t brightness);

uint8_t digit1, digit2, digit3, digit4, dp;
unsigned char *pPort; //pointer to output port for segments 
bit BrightnessReset;  //Brighness is disomogeneous and need to be resetted


uint8_t digit;

/*This is the convention
 * Segment, Port bit
   a        0
 * b        1
 * c        2
 * d        3
 * e        4
 * f        5
 * g        7
 * dp       8 */

const uint8_t Decode[16] =
{
    0b00111111,     //0
    0b00000110,     //1
    0b01011011,     //2
    0b01001111,     //3    
    0b01100110,     //4
    0b01101101,     //5
    0b01111101,     //6
    0b00000111,     //7
    0b01111111,     //8
    0b01101111,     //9
    0b01110111,     //A
    0b01111100,     //b
    0b01011000,     //c
    0b01011110,     //d
    0b01111001,     //E
    0b01110001      //F
            
};


void SetupOutputPort (unsigned char *port)
{
    pPort = port;
}
void SetupDigitController (void (*controller)(uint8_t state))
{
    DigitController = controller;    
}




void DisplayRefresh (void)
{
    if (pPort)
    {
        uint8_t dgt;
        switch (digit)
        {
            case 0:
                if (DigitController)
                    DigitController(0b00000001);
                dgt = GetDigit4();
                if (dp & 0b00000001)
                    dgt |= 0b10000000;
                else 
                    dgt &= 0b01111111;                 
                break;
            case 1:
                if (DigitController)
                    DigitController(0b0000010);
                dgt = GetDigit3();
                if (dp & 0b00000010)
                    dgt |= 0b10000000;
                else 
                    dgt &= 0b01111111; 
                break;
            case 2:
                if (DigitController)
                    DigitController(0b0000100);
                dgt = GetDigit2();
                if (dp & 0b00000100)
                    dgt |= 0b10000000;
                else 
                    dgt &= 0b01111111;         
                break;
            case 3:
                if (DigitController)
                    DigitController(0b00001000);
                dgt = GetDigit1(); 
                if (dp & 0b00001000)
                    dgt |= 0b10000000;
                else 
                    dgt &= 0b01111111; 
                break;
            default:
                break;      
        }
        *pPort = dgt;
        digit ++;
        if (digit > 3)
            digit = 0;
    }
}

void DPOn (uint8_t dp_n)
{
    switch (dp_n)
    {
        case 1:
            dp |= 0b00000001;
            break;
        case 2:
            dp |= 0b00000010;
            break;
        case 3:
            dp |= 0b00000100;
            break;            
        case 4:
            dp |= 0b00001000;
            break;
        default:
            break;
    }
}

void DPOff (uint8_t dp_n)
{
    switch (dp_n)
    {
        case 1:
            dp &= 0b11111110;
            break;
        case 2:
            dp &= 0b11111101;
            break;
        case 3:
            dp &= 0b11111011;
            break;            
        case 4:
            dp &= 0b11110111;
            break;
        default:
            break;

    }
}
uint8_t DisplayBCD (uint8_t digit12, uint8_t digit34)
{
    digit1 = digit12 >> 4;
    digit2 = digit12 & 0x0f;
    digit3 = digit34 >> 4;
    digit4 = digit34 & 0x0f;
}

uint8_t DisplayUnsignedInt (uint16_t n)
{
    const char *pDecode = &Decode[0]; 
    if (n > 9999)
        return -1;
    uint8_t x1000 = 0;
    while (n > 999)
    {
        n -= 1000;
        x1000++;
    }
    digit1 = *(pDecode + x1000);
    
    uint8_t x100 = 0;
    while (n > 99)
    {
        n -= 100;
        x100++;
    } 
    digit2 = *(pDecode + x100);
    uint8_t x10 = 0;
    while (n > 9)
    {
        n -= 10;
        x10++;
    }
    digit3 = *(pDecode + x10);    
    uint8_t x1 = 0;
    while (n > 0)
    {
        n -= 1;
        x1++;
    }
    digit4 = *(pDecode + x1);
    return 0;
}

uint8_t DisplayInt (int16_t n)
{
    int abs_n = abs(n);
    const uint8_t *pDecode = &Decode[0]; 
    if (n < -999 || n > 9999)
        return -1;
    if (n < 0)
        digit1 = 0b00000001;
    else
    {
       uint8_t x1000 = 0;
       while (abs_n > 999)
       {
           abs_n -= 1000;
           x1000++;
       }
       digit1 = *(pDecode + x1000);     
    }
    uint8_t x100 = 0;
    while (abs_n > 99)
    {
        abs_n -= 100;
        x100++;
    } 
    digit2 = *(pDecode + x100);
    uint8_t x10 = 0;
    while (abs_n > 9)
    {
        abs_n -= 10;
        x10++;
    }
    digit3 = *(pDecode + x10);    
    uint8_t x1 = 0;
    while (abs_n > 0)
    {
        abs_n -= 1;
        x1++;
    }
    digit4 = *(pDecode + x1);
    return 0;
}

uint8_t SetCouples (uint8_t left, uint8_t right)
{    
    const uint8_t *pDecode = &Decode[0]; 
    uint8_t lx10 = 0;
    uint8_t rx10 = 0;
    uint8_t lx1 = 0;
    uint8_t rx1 = 0;
    if (left < 99)
    {       
      while (left > 9)
      {
          left -= 10;
          lx10++;
      }
      digit1 = *(pDecode + lx10);
      while (left > 0)
      {
          left -= 1;
          lx1++;
      } 
      digit2 = *(pDecode + lx1);      
    }
    
    if (right < 99)
    {       
      while (right > 9)
      {
          right -= 10;
          rx10++;
      }
      digit3 = *(pDecode + rx10);
      while (right > 0)
      {
          right -= 1;
          rx1++;
      } 
      digit4 = *(pDecode + rx1);      
    } 
    return 0;
}

uint8_t GetDigit1 (void)
{
    return digit1;
}
uint8_t GetDigit2 (void)
{
    return digit2;
}
uint8_t GetDigit3 (void)
{
    return digit3;
}
uint8_t GetDigit4 (void)
{
    return digit4;
}



void SetDigitBrightness (uint8_t digit, uint8_t bright)
{
            if (BrightDigit1)
            {
                BrightDigit1(bright);
                BrightnessReset = 1;
            }
}




void SetDigitController (void (*Controller)(uint8_t digit, bit state, uint8_t brightness))
{
    DigitController = Controller;
}