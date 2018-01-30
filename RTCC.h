/* 
 * File:   RTCC.h
 * Author: Luca
 *
 * Created on 27 gennaio 2018, 0.29
 */

#ifndef RTCC_H
#define	RTCC_H



typedef struct  {
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t weekday;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
}datetime;

void SetupRTCC(void);
void GetDateTime (datetime *pdt);
void SetDateTime (datetime dt);


#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* RTCC_H */

