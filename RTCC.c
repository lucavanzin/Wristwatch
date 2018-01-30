#include <stdint.h>
#include <xc.h>
#include "RTCC.h"




void unlock_RTCC(void)
{   
#asm
    MOVLB 0x0F          //sequence followed for unlocking RTCC
    MOVLW 0x55
    MOVWF EECON2
    MOVLW 0xAA
    MOVWF EECON2
    BSF RTCCFG, 5, 1
#endasm
}

#define lock_RTCC()      RTCWREN = 0;

uint8_t BCDtoDEC (uint8_t BCDByte)
{
    return(((BCDByte & 0xF0) >> 4) * 10) + (BCDByte &0x0F);
}

uint8_t DECtoBCD (uint8_t DECByte){
    return(((DECByte / 10) << 4) | (DECByte %10));
}

void SetupRTCC (void)
{
    OSCTUNE = 0b10000000; //device CLK from INTRC
    OSCCON = 0b01100000; //intOSC System clock
    REFOCON = 0b00000000; //enable oscout at RB2
    T1CON = 0b00001000;
    ANCON1 = 0b00011111;  
    PADCFG1 = 0b00000010;  
    unlock_RTCC();
    RTCCFG = 0b10000000;
    lock_RTCC();
}

//Reading/Writing RTCVALH RTCPTR1:0 is automatically decremented by 1 until reaches 00
void GetDateTime (datetime *pdt)
{
    while (RTCCFGbits.RTCSYNC);
    unlock_RTCC();
    RTCCFGbits.RTCPTR0 = 1;     //RTCVALH   RTCVALL
    RTCCFGbits.RTCPTR1 = 1;     //Res.      Anno 
    pdt->year = (RTCVALL >> 4)*10 +  (RTCVALL & 0x0F);
    uint8_t dummy = RTCVALH;
    pdt->day = (RTCVALL >> 4)*10 +  (RTCVALL & 0x0F);
    pdt->month = (RTCVALH >> 4)*10 +  (RTCVALH & 0x0F);
    pdt->hour = (RTCVALL >> 4)*10 +  (RTCVALL & 0x0F);
    pdt->weekday = RTCVALH;
    pdt->second = (RTCVALL >> 4)*10 +  (RTCVALL & 0x0F);   
    pdt->minute = (RTCVALH >> 4)*10 +  (RTCVALH & 0x0F);
    lock_RTCC();


   

  

}

void SetDateTime (datetime dt)
{
    unlock_RTCC();
    RTCCFGbits.RTCPTR0 = 0;     //RTCVALH   RTCVALL
    RTCCFGbits.RTCPTR1 = 0;     //Minuti    Secondi
    RTCVALH = DECtoBCD(dt.minute);
    RTCVALL = DECtoBCD(dt.second);
    RTCCFGbits.RTCPTR0 = 1;     //RTCVALH   RTCVALL
    RTCCFGbits.RTCPTR1 = 0;     //Gg sett    ore
    RTCVALH = DECtoBCD(dt.weekday);
    RTCVALL = DECtoBCD(dt.hour);
    RTCCFGbits.RTCPTR0 = 0;     //RTCVALH   RTCVALL
    RTCCFGbits.RTCPTR1 = 1;     //Mese      giorno
    RTCVALH = DECtoBCD(dt.month);
    RTCVALL = DECtoBCD(dt.day);
    RTCCFGbits.RTCPTR0 = 1;     //RTCVALH   RTCVALL
    RTCCFGbits.RTCPTR1 = 1;     //Res.      Anno
    RTCVALL = DECtoBCD(dt.year); 
    lock_RTCC();   
}
